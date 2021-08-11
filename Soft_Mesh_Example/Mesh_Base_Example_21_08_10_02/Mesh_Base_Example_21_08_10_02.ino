/*
 
 ESP32 Dev Module


*/

#include "Globals.h"

#include "CommandBuffer.h"
#include "CommandParser.h"
#include "ModuleController.h"
#include "AlertModule.h"
#include "ZeroStreamListener.h"
#include "Memory.h"
#include "InteropStream.h"
#include "PowerManager.h"

//===============================
#include "SPI.h"
#include <TFT_eSPI.h>
#include <SD.h>
#include "OneWire.h"
#include "Configuration_ESP32.h"
#include "Settings.h"
#include "Sensor.h"
#include <Wire.h>

//=== Новые файлы =================

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
//================================

#ifdef USE_TFT_MODULE
#include "TFTModule.h"
#endif



#ifdef USE_TFT_MODULE
TFTModule tftModule;
#endif


#include "LogicManageModule.h"
#include "EEPROMSettingsModule.h"

#include "DelayedEvents.h"
#include "OneWire.h"

// Ждем команды из сериала
CommandBuffer commandsFromSerial(&Serial);

// Парсер команд
CommandParser commandParser;

// Контроллер модулей
ModuleController controller;

///////////////////////////// КОД СБРОСА ДИНАМИЧЕСКИХ ДАТЧИКОВ ////////////////////////////////////////
#ifdef USE_DYNAMIC_SENSORS_RESET_MODULE
DynamicSensorsResetModule dynamicResetSensors;
#endif
///////////////////////////// КОД СБРОСА ДИНАМИЧЕСКИХ ДАТЧИКОВ ////////////////////////////////////////

LogicManageModuleClass logicManageModule;
EEPROMSettingsModule eepromSettingsModule;
ZeroStreamListener zeroStreamModule;
AlertModule alertsModule;

//--------------------------------------------------------------------------------------------------------------------------------
bool canCallYield = false;
//--------------------------------------------------------------------------------------------------------------------------------
void initI2C()
{
	//  // инициализируем стандартными методами
	Wire.begin();

}
//--------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  canCallYield = false;


  Serial.begin(SERIAL_BAUD_RATE);   // запускаем Serial на нужной скорости
  while(!Serial);                   // ждём инициализации Serial

  Settings.setup(); // настраиваем хранилище в EEPROM
  
  Sensor.setup();
  
#ifdef USE_TFT_MODULE 
 tftModule.Setup();
#endif


 /*RTC_DS3231  rtc = controller.GetClock();
 DateTime now = rtc.now();*/




/*
  // тест EEPROM
 Serial.println();
 Serial.print("Controller ID = ");
 Serial.println(Settings.GetControllerID());
 Settings.SetControllerID(100);
  Serial.println();

*/
  
 canCallYield = true;
  
}
//--------------------------------------------------------------------------------------------------------------------------------

void loop() 
{

#ifdef USE_TFT_MODULE
	tftModule.Update();
#endif 


}

//--------------------------------------------------------------------------------------------------------------------------------
void yield()
{
 
  if(!canCallYield)
    return;
    
// отсюда можно добавлять любой сторонний код, который надо вызывать, когда МК чем-то долго занят (например, чтобы успокоить watchdog)


// до сюда можно добавлять любой сторонний код


// отсюда можно добавлять любой сторонний код, который надо вызывать, когда МК чем-то долго занят (например, чтобы успокоить watchdog)

// до сюда можно добавлять любой сторонний код

}
//--------------------------------------------------------------------------------------------------------------------------------
