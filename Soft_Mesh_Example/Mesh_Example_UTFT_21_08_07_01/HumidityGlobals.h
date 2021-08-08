#ifndef _HUMIDITY_GLOBALS_H
#define _HUMIDITY_GLOBALS_H
//--------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool IsOK; // данные получены?
  int8_t Humidity; // целое значение влажности
  uint8_t HumidityDecimal; // значение влажности после запятой
  int8_t Temperature; // целое значение температуры
  uint8_t TemperatureDecimal; // значение температуры после запятой
  
} HumidityAnswer; // ответ от датчика
#pragma pack(pop)
//--------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  NO_HUMIDITY_SENSOR = 0,
  DHT11, // сенсор DHT11
  DHT2x, // сенсор DHT21 и старше
  SI7021, // цифровой сенсор Si7021 
  SHT10, // датчик SHT10
  MISOL, // датчики с метеостанции типа MISOL
  
} HumiditySensorType; // какие сенсоры поддерживаем
//--------------------------------------------------------------------------------------------------------------------------------------
#endif
