#include <Adafruit_Si7021.h>
#include <OneWire_TRACKER.h>
#include <Arduino.h>
#include <wiring_private.h>
#include "CONFIG.h"
#include "UTFTMenu.h"
#include "DS3231.h"                 // подключаем часы
#include "AT24CX.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подключаем наши экраны
#include "MainScreen.h"              // Главный экран

#include "Screen4.h"              // Вызов меню установки времени и даты
#include "Screen5.h"              // Вызов установки времени
#include "Screen6.h"              // Вызов установки даты

#include "Buzzer.h"
#include "Buttons.h"                 // наши железные кнопки
#include "FileUtils.h"
#include "Settings.h"
#include "CoreCommandBuffer.h"
#include "Logger.h"
#include "CoreTransport.h"



//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t screenIdleTimer = 0;
uint32_t backlightTimer = 0;
const int ledPin = 11;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void screenAction(AbstractTFTScreen* screen)
{
   // какое-то действие на экране произошло.
   // тут просто сбрасываем таймер ничегонеделанья.
   screenIdleTimer = millis();

   backlightTimer = millis();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Uart Serial2(&sercom2, 3, 4, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Подключить Serial2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SERCOM2_Handler() // Подключить Serial2
{
  Serial2.IrqHandler();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(SERIAL_SPEED);
  delay(1000);    // Подождать вывод в SerialUSB
  //pinMode(BUZZER_PIN,OUTPUT);
  //digitalWrite(BUZZER_PIN,!BUZZER_LEVEL);

  // раскомментировать для отладочной информации !!!
  //while(!Serial);

  // ждём секунду вывода в SerialUSB
  uint32_t waitM = millis();
  while(!(millis() - waitM > 1000))
  {
    if(Serial)
      break;
  }
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Wire.begin();

  DBGLN(F("Init RTC..."));
  RealtimeClock.begin(RTC_WIRE_NUMBER);           // запускаем их на шине I2C 1 (SDA1, SCL1)
  // Отключить вывод импульсов 32 кгц на выходе
  RealtimeClock.enable32kHz(false);


  DBGLN(F("INIT settings..."));
  Settings.begin();
  Logger.begin();
  DBGLN(F("Settings inited."));

  // настраиваем железные кнопки
  Buttons.begin();

  Serial2.begin(SERIAL_SPEED); // Возможно для принтера 

  //Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);    // Настройка Serial2
  pinPeripheral(4, PIO_SERCOM_ALT);    // Настройка Serial2  
  delay(1000);

 // RealtimeClock.setTime(5,24,20,5,16,4,2021);

  DBGLN(F("INIT SD..."));
  SDInit::InitSD();
  DBGLN(F("SD inited."));
  
  DBGLN(F("Init screen..."));
  Screen.setup();
  
  DBGLN(F("Add main screen...")); 
  Screen.addScreen(MainScreen::create());           // первый экран покажется по умолчанию
  DBGLN(F("Main screen added.")); 

  DBGLN(F("Add menu screen #1...")); 
  Screen.addScreen(MenuScreen1::create());
  DBGLN(F("Menu screen #1 added.")); 

  DBGLN(F("Add menu screen #2..."));
  Screen.addScreen(MenuScreen2::create());
  DBGLN(F("Menu screen #2 added."));

  DBGLN(F("Add menu screen #3..."));
  Screen.addScreen(BordersScreen::create());
  DBGLN(F("Menu screen #3 added."));

  DBGLN(F("Add menu screen #4..."));
  Screen.addScreen(MenuScreen4::create());
  DBGLN(F("Menu screen #4 added."));

  DBGLN(F("Add screen4..."));
  // добавляем четвертый экран. Меню установки даты и времени
  Screen.addScreen(Screen4::create());
  
  DBGLN(F("Add screen5..."));
  // добавляем 5 экран. Установка времени
  Screen.addScreen(Screen5::create());

  DBGLN(F("Add screen6..."));
  // добавляем 6 экран. Установка даты
  Screen.addScreen(Screen6::create());

    // переключаемся на первый экран
  Screen.switchToScreen("Main");


  screenIdleTimer = millis();
  Screen.onAction(screenAction);


  DBGLN(F("Inited."));

  Serial.print(F("Gaz analizer "));
  Serial.println(SOFTWARE_VERSION);
  
 // Buzzer.begin();
 // Buzzer.buzz();

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  Settings.update();
  
  // обновляем кнопки
  Buttons.update();
  Screen.update();

   // проверяем, какой экран активен. Если активен главный экран - сбрасываем таймер ожидания. Иначе - проверяем, не истекло ли время ничегонеделанья.
  AbstractTFTScreen* activeScreen = Screen.getActiveScreen();
  if(activeScreen == mainScreen)
  {
    screenIdleTimer = millis();
  }
  else
  {
      if(millis() - screenIdleTimer > RESET_TO_MAIN_SCREEN_DELAY)
      {
        screenIdleTimer = millis();
        Screen.switchToScreen(mainScreen);
      }
  } // else

  // При питании от внутреннего источника, отключать подсветку дисплея через 5 минут при отсутствии активности на кнопках
  if(Settings.getPowerType() == batteryPower)
  {
    if(millis() - backlightTimer > BACKLIGHT_OFF_DELAY)
    {
      backlightTimer = millis();
      Settings.displayBacklight(false);
    }
  }


  
  // обрабатываем входящие команды
  CommandHandler.handleCommands();
  

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool nestedYield = false;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  
  if(nestedYield)
    return;
    
 nestedYield = true;
 
   // обновляем кнопки   
   Buttons.update();

  nestedYield = false;
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

