#include "CoreTransport.h"
#include "Settings.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------------------------------------------
#define CIPSEND_COMMAND F("AT+CIPSENDBUF=")
//--------------------------------------------------------------------------------------------------------------------------------------
// CoreTransportClient
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient::CoreTransportClient()
{
  socket = NO_CLIENT_ID;
  dataBuffer = NULL;
  dataBufferSize = 0;
  parent = NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient::~CoreTransportClient()
{
  clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::accept(CoreTransport* _parent)
{
  parent = _parent;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::clear()
{
    delete [] dataBuffer; 
    dataBuffer = NULL;
    dataBufferSize = 0;
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::disconnect()
{
  if(!parent)
    return;
  
  if(!connected())
    return;

  parent->doDisconnect(*this);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransportClient::connect(const char* ip, uint16_t port)
{
  if(!parent)
    return;
  
  if(connected()) // уже присоединены, нельзя коннектится до отсоединения!!!
    return;
          
  parent->doConnect(*this,ip,port);
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransportClient::write(uint8_t* buff, size_t sz)
{
  if(!parent)
    return false;
  
    if(!sz || !buff || !connected() || socket == NO_CLIENT_ID)
    {
      DBGLN(F("CoreTransportClient - CAN'T WRITE!"));
      /*
      if(!connected())
      {
        DBGLN(F("NOT CONNECTED!!!"));
      }
      if(socket == NO_CLIENT_ID)
      {
        DBGLN(F("socket == NO_CLIENT_ID!!!"));        
      }
      */
      return false;
    }

  clear();
  dataBufferSize = sz; 
  if(dataBufferSize)
  {
      dataBuffer = new  uint8_t[dataBufferSize];
      memcpy(dataBuffer,buff,dataBufferSize);
  }

    parent->doWrite(*this);
    
   return true;
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransportClient::connected() 
{
  if(!parent || socket == NO_CLIENT_ID)
    return false;
    
  return parent->connected(socket);
}
//--------------------------------------------------------------------------------------------------------------------------------------
// CoreTransport
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransport::CoreTransport(uint8_t clientsPoolSize)
{
  for(uint8_t i=0;i<clientsPoolSize;i++)
  {
    CoreTransportClient* client = new CoreTransportClient();
    client->accept(this);
    client->bind(i);
    
    pool.push_back(client);
    status.push_back(false);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransport::~CoreTransport()
{
  for(size_t i=0;i<pool.size();i++)
  {
    delete pool[i];
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::initPool()
{

  //DBGLN(F("ESP: INIT CLIENTS POOL..."));
  //
  //Vector<CoreTransportClient*> tmp = externalClients;
  //for(size_t i=0;i<tmp.size();i++)
  //{
  //  notifyClientConnected(*(tmp[i]),false,CT_ERROR_NONE);
  //  tmp[i]->release();
  //}
  //
  //for(size_t i=0;i<status.size();i++)
  //{
  //  status[i] = false;
  //  pool[i]->clear(); // очищаем внутренних клиентов
  //}
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransport::connected(uint8_t socket)
{
  return status[socket];
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doWrite(CoreTransportClient& client)
{
  if(!client.connected())
  {
    client.clear();
    return;
  }

   beginWrite(client); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doConnect(CoreTransportClient& client, const char* ip, uint16_t port)
{
  if(client.connected())
    return;

   // запоминаем нашего клиента
   client.accept(this);

  // если внешний клиент - будем следить за его статусом соединения/подсоединения
   if(isExternalClient(client))
    externalClients.push_back(&client);

   beginConnect(client,ip,port); 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::doDisconnect(CoreTransportClient& client)
{
  if(!client.connected())
    return;

    beginDisconnect(client);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::subscribe(IClientEventsSubscriber* subscriber)
{
  for(size_t i=0;i<subscribers.size();i++)
  {
    if(subscribers[i] == subscriber)
      return;
  }

  subscribers.push_back(subscriber);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::unsubscribe(IClientEventsSubscriber* subscriber)
{
  for(size_t i=0;i<subscribers.size();i++)
  {
    if(subscribers[i] == subscriber)
    {
      for(size_t k=i+1;k<subscribers.size();k++)
      {
        subscribers[k-1] = subscribers[k];
      }
      subscribers.pop();
      break;
    }
  }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
bool CoreTransport::isExternalClient(CoreTransportClient& client)
{
  // если клиент не в нашем пуле - это экземпляр внешнего клиента
  for(size_t i=0;i<pool.size();i++)
  {
    if(pool[i] == &client)
      return false;
  }

  return true;
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyClientConnected(CoreTransportClient& client, bool connected, int16_t errorCode)
{
   // тут надо синхронизировать с пулом клиентов
   if(client.socket != NO_CLIENT_ID)
   {
      status[client.socket] = connected;
   }
  
    for(size_t i=0;i<subscribers.size();i++)
    {
    //  subscribers[i]->OnClientConnect(client,connected,errorCode);
    }

      // возможно, это внешний клиент, надо проверить - есть ли он в списке слежения
      if(!connected) // пришло что-то типа 1,CLOSED
      {         
        // клиент отсоединился, надо освободить его сокет
        for(size_t i=0;i<externalClients.size();i++)
        {
          if(externalClients[i]->socket == client.socket)
          {
            externalClients[i]->clear();
            
			DBG(F("RELEASE SOCKET ON OUTGOING CLIENT #"));
			DBGLN(String(client.socket));
            
            externalClients[i]->release(); // освобождаем внешнему клиенту сокет
            
            for(size_t k=i+1;k<externalClients.size();k++)
            {
              externalClients[k-1] = externalClients[k];
            }
            externalClients.pop();
            break;
          }
        } // for
      } // if(!connected)
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyDataWritten(CoreTransportClient& client, int16_t errorCode)
{
    for(size_t i=0;i<subscribers.size();i++)
    {
//      subscribers[i]->OnClientDataWritten(client,errorCode);
    } 
}
//--------------------------------------------------------------------------------------------------------------------------------------
void CoreTransport::notifyDataAvailable(CoreTransportClient& client, uint8_t* data, size_t dataSize, bool isDone)
{
    for(size_t i=0;i<subscribers.size();i++)
    {
     // subscribers[i]->OnClientDataAvailable(client,data,dataSize,isDone);
    }  
}
//--------------------------------------------------------------------------------------------------------------------------------------
CoreTransportClient* CoreTransport::getClient(uint8_t socket)
{
  if(socket != NO_CLIENT_ID)
    return pool[socket];

  for(size_t i=0;i<pool.size();i++)
  {
    if(!pool[i]->connected())
      return pool[i];
  }

  return NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------

