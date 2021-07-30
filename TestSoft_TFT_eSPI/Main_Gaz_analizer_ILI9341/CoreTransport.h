#pragma once
//--------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "TinyVector.h"
#include "CONFIG.h"
//--------------------------------------------------------------------------------------------------------------------------------
// максимальная длина одного пакета к вычитке прежде, чем подписчику придёт уведомление о пакете данных
#define TRANSPORT_MAX_PACKET_LENGTH 128
//--------------------------------------------------------------------------------------------------------------------------------
class CoreTransportClient;
class CoreESPTransport;
//--------------------------------------------------------------------------------------------------------------------------------
// Коды ошибок транспорта
//--------------------------------------------------------------------------------------------------------------------------------
#define CT_ERROR_NONE             0 // нет ошибки
#define CT_ERROR_CANT_CONNECT     1 // не удалось установить соединение
#define CT_ERROR_CANT_WRITE       2 // ошибка записи данных из клиента в поток
//--------------------------------------------------------------------------------------------------------------------------------
// типы событий
//--------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  etConnect,      // соединено
  etDataWritten,    // данные записаны
  etDataAvailable   // доступны входящие данные
  
} ClientEventType;
//---------------------------------------------------------------------------------------------------------------------------------
struct IClientEventsSubscriber
{
 
};
//--------------------------------------------------------------------------------------------------------------------------------
class RecursionCounter
{
  private:
    uint16_t* thisCntr;
  public:
    RecursionCounter(uint16_t* cntr)
    {
      thisCntr = cntr;
      (*thisCntr)++;
    }
   ~RecursionCounter()
   {
     (*thisCntr)--;
   }
  
};
//--------------------------------------------------------------------------------------------------------------------------------
typedef Vector<IClientEventsSubscriber*> ClientSubscribers;
#define NO_CLIENT_ID 0xFF
//--------------------------------------------------------------------------------------------------------------------------------
class CoreTransport
{
  public:
  
    CoreTransport(uint8_t clientsPoolSize);
    virtual ~CoreTransport();
    
    // обновляем состояние транспорта
    virtual void update() = 0; 

    // начинаем работу
    virtual void begin() = 0; 

    // проверяет, готов ли транспорт к работе (например, проведена ли первичная инициализация)
    virtual bool ready() = 0; 

   // подписка на события клиентов
   void subscribe(IClientEventsSubscriber* subscriber);
   
   // отписка от событий клиентов
   void unsubscribe(IClientEventsSubscriber* subscriber);


private:

    ClientSubscribers subscribers;
    Vector<CoreTransportClient*> pool;
    Vector<bool> status;

    Vector<CoreTransportClient*> externalClients;
    bool isExternalClient(CoreTransportClient& client);

protected:

    void initPool();

  friend class CoreTransportClient;

  bool connected(uint8_t socket);

  void doWrite(CoreTransportClient& client); // начинаем писать в транспорт с клиента
  void doConnect(CoreTransportClient& client, const char* ip, uint16_t port); // начинаем коннектиться к адресу
  void doDisconnect(CoreTransportClient& client); // начинаем отсоединение от адреса
  
  CoreTransportClient* getClient(uint8_t socket);

  // вызов событий для клиента
  void notifyClientConnected(CoreTransportClient& client, bool connected, int16_t errorCode);  
  void notifyDataWritten(CoreTransportClient& client, int16_t errorCode);
  void notifyDataAvailable(CoreTransportClient& client, uint8_t* data, size_t dataSize, bool isDone);
      
    
  virtual void beginWrite(CoreTransportClient& client) = 0; // начинаем писать в транспорт с клиента
  virtual void beginConnect(CoreTransportClient& client, const char* ip, uint16_t port) = 0; // начинаем коннектиться к адресу
  virtual void beginDisconnect(CoreTransportClient& client) = 0; // начинаем отсоединение от адреса
  
};
//--------------------------------------------------------------------------------------------------------------------------------
class CoreTransportClient
{
  public:

   CoreTransportClient();
   ~CoreTransportClient();
  
  bool connected();
  void accept(CoreTransport* _parent);
     
  operator bool()
  {
    return  (socket != NO_CLIENT_ID); 
  }

  bool operator==(const CoreTransportClient& rhs)
  {
    return (rhs.socket == socket);
  }

  bool operator!=(const CoreTransportClient& rhs)
  {
    return !(operator==(rhs));
  }

  void connect(const char* ip, uint16_t port);
  void disconnect();
  
  bool write(uint8_t* buff, size_t sz);  


 protected:

    friend class CoreTransport;    
    friend class CoreESPTransport;
    
    CoreTransport* parent;

    void releaseBuffer()
    {
      dataBuffer = NULL;
      dataBufferSize = 0;
    }

    // установка ID клиента транспортом
    void bind(uint8_t _socket)
    {
      socket = _socket;
    }
    void release()
    {
      socket = NO_CLIENT_ID;
    }

    void clear();
    uint8_t* getBuffer(size_t& sz)
    {
      sz =  dataBufferSize;
      return dataBuffer;
    }

 private:

    CoreTransportClient(const CoreTransportClient& rhs);
    CoreTransportClient& operator=(const CoreTransportClient& rhs);

    uint8_t* dataBuffer;
    size_t dataBufferSize;
    uint8_t socket;
    
};
//--------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  
  actionDisconnect, // запрошено отсоединение клиента
  actionConnect, // запрошено подсоединение клиента
  actionWrite, // запрошена запись из клиента в транспорт
  
} TransportClientAction;
//--------------------------------------------------------------------------------------------------------------------------------
struct TransportClientQueueData // данные по клиенту в очереди
{
  TransportClientAction action; // действие, которое надо выполнить с клиентом
  CoreTransportClient* client; // ссылка на клиента
  char* ip; // IP для подсоединения
  uint16_t port; // порт для подсоединения
  size_t dataLength;
  uint8_t* data;

  TransportClientQueueData()
  {
   client = NULL;
   ip = NULL;
   data = NULL;
  }
   
};
//--------------------------------------------------------------------------------------------------------------------------------
typedef Vector<TransportClientQueueData> TransportClientsQueue; // очередь клиентов на совершение какой-либо исходящей операции (коннект, дисконнект, запись)
//--------------------------------------------------------------------------------------------------------------------------------
typedef Vector<uint8_t> TransportReceiveBuffer;

