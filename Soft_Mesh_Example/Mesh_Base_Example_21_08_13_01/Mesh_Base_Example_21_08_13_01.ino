/*
 
 ESP32 Dev Module


*/


#include "SPI.h"
#include <TFT_eSPI.h>
#include <SD.h>

#include "Configuration_ESP32.h"
#include "Settings.h"
#include "Sensor.h"

#ifdef USE_TFT_MODULE
#include "TFTModule.h"
#endif



#ifdef USE_TFT_MODULE
TFTModule tftModule;
#endif

//--------------------------------------------------------------------------------------------------------------------------------
bool canCallYield = false;
//--------------------------------------------------------------------------------------------------------------------------------

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
