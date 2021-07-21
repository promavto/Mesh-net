/*
 
 ESP32 Dev Module


*/


#include "SPI.h"
#include <TFT_eSPI.h>
#include "TFTMenu.h"


#define USE_TFT_MODULE


#ifdef USE_BUZZER
#include "Buzzer.h"
#endif

#ifdef ESP32_START_DEBUG
#define START_LOG(x) {Serial.println((x)); Serial.flush(); }
#else
#define START_LOG(x) (void) 0
#endif


//--------------------------------------------------------------------------------------------------------------------------------
bool canCallYield = false;
//--------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  canCallYield = false;


  Serial.begin(115200);   // запускаем Serial на нужной скорости
  while(!Serial);                   // ждём инициализации Serial
  
  START_LOG(1);
    
  
  #ifdef USE_BUZZER
  Buzzer.begin();
  #endif
  
  // регистрируем модули  
  #ifdef USE_PIN_MODULE  
  controller.RegisterModule(&pinModule);
  #endif

 

  //controller.begin(); // начинаем работу

 START_LOG(2);
 //tftModule.Setup();
 Screen.setup();

#ifdef USE_TFT_MODULE
 //controller.RegisterModule(&tftModule);
#endif





  /*#ifdef USE_DS3231_REALTIME_CLOCK
  
   RealtimeClock rtc = controller.GetClock();
   RTCTime tm = rtc.getTime();

   Serial.print(F(", "));
   Serial.print(rtc.getDayOfWeekStr(tm));
   Serial.print(F(" "));
   Serial.print(rtc.getDateStr(tm));
   Serial.print(F(" - "));
   Serial.print(rtc.getTimeStr(tm));
      
  #endif */

  Serial.println();


  
 canCallYield = true;
  
}
//--------------------------------------------------------------------------------------------------------------------------------
// эта функция вызывается после обновления состояния каждого модуля.
// передаваемый параметр - указатель на обновлённый модуль.
// если модулю предстоит долгая работа - помимо этого инструмента
// модуль должен дёргать функцию yield, если чем-то долго занят!
//--------------------------------------------------------------------------------------------------------------------------------
//void ModuleUpdateProcessed(AbstractModule* module)
//{
//  UNUSED(module);
//
//  CoreDelayedEvent.update();
//
//  // используем её, чтобы проверить состояние порта UART для WI-FI-модуля - вдруг надо внеочередно обновить
//    #ifdef USE_WIFI_MODULE
//    // модуль Wi-Fi обновляем каждый раз после обновления очередного модуля
//     if(module != &wifiModule) 
//	 {
//		 ESP.update();
//	 }
//    #endif
//
//   #ifdef USE_SMS_MODULE
//   // и модуль GSM тоже тут обновим
//    if(module != &smsModule)
//	{
//		SIM800.update();
//	}
//   #endif     
//}
////--------------------------------------------------------------------------------------------------------------------------------
//#ifdef USE_EXTERNAL_WATCHDOG
//void updateExternalWatchdog()
//{
//  static unsigned long watchdogLastMillis = millis();
//  unsigned long watchdogCurMillis = millis();
//
//  uint16_t dt = watchdogCurMillis - watchdogLastMillis;
//  watchdogLastMillis = watchdogCurMillis;
//
//      watchdogSettings.timer += dt;
//      switch(watchdogSettings.state)
//      {
//        case WAIT_FOR_TRIGGERED:
//        {
//          if(watchdogSettings.timer >= WATCHDOG_WORK_INTERVAL)
//          {
//            watchdogSettings.timer = 0;
//            watchdogSettings.state = WAIT_FOR_NORMAL;
//            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_TRIGGERED_LEVEL);
//          }
//        }
//        break;
//
//        case WAIT_FOR_NORMAL:
//        {
//          if(watchdogSettings.timer >= WATCHDOG_PULSE_DURATION)
//          {
//            watchdogSettings.timer = 0;
//            watchdogSettings.state = WAIT_FOR_TRIGGERED;
//            digitalWrite(WATCHDOG_REBOOT_PIN, WATCHDOG_NORMAL_LEVEL);
//          }          
//        }
//        break;
//      }  
//  
//}
//#endif
////--------------------------------------------------------------------------------------------------------------------------------
//void processCommandsFromSerial()
//{
// // смотрим, есть ли входящие команды
//   if(commandsFromSerial.HasCommand())
//   {
//    // есть новая команда
//    Command cmd;
//    if(commandParser.ParseCommand(commandsFromSerial.GetCommand(), cmd))
//    {
//       Stream* answerStream = commandsFromSerial.GetStream();
//      // разобрали, назначили поток, с которого пришла команда
//        cmd.SetIncomingStream(answerStream);
//
//      // запустили команду в обработку
//       controller.ProcessModuleCommand(cmd);
// 
//    } // if
//    else
//    {
//      // что-то пошло не так, игнорируем команду
//    } // else
//    
//    commandsFromSerial.ClearCommand(); // очищаем полученную команду
//   } // if  
//}
////--------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
// отсюда можно добавлять любой сторонний код
//uint32_t start = millis();
// до сюда можно добавлять любой сторонний код

	//tftModule.Update();
  //// смотрим, есть ли входящие команды
  // processCommandsFromSerial();
  //  
  // // обновляем состояние всех зарегистрированных модулей
  // controller.UpdateModules(ModuleUpdateProcessed);

  // CoreDelayedEvent.update();

   
// отсюда можно добавлять любой сторонний код
//uint32_t end = millis() - start;
//Serial.print("One loop() pass time, ms: ");
//Serial.println(end);
// до сюда можно добавлять любой сторонний код

}
//--------------------------------------------------------------------------------------------------------------------------------
//void esp_sim800_call()
//{
//   #ifdef USE_WIFI_MODULE
//    ESP.readFromStream();
//    #endif
//}
//--------------------------------------------------------------------------------------------------------------------------------
// обработчик простоя, используем и его. Если сторонняя библиотека устроена правильно - она будет
// вызывать yield периодически - этим грех не воспользоваться, чтобы избежать потери данных
// в портах UART. 
//--------------------------------------------------------------------------------------------------------------------------------
void yield()
{
 
  if(!canCallYield)
    return;
    
// отсюда можно добавлять любой сторонний код, который надо вызывать, когда МК чем-то долго занят (например, чтобы успокоить watchdog)


// до сюда можно добавлять любой сторонний код

   #ifdef USE_EXTERNAL_WATCHDOG
     updateExternalWatchdog();
   #endif // USE_EXTERNAL_WATCHDOG

   //esp_sim800_call();

   //CoreDelayedEvent.update();


// отсюда можно добавлять любой сторонний код, который надо вызывать, когда МК чем-то долго занят (например, чтобы успокоить watchdog)

// до сюда можно добавлять любой сторонний код

}
//--------------------------------------------------------------------------------------------------------------------------------
