#pragma once
//--------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <Preferences.h>
#include "Utils.h"
#include "Globals.h"
//--------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool isValid;
  uint16_t points[5];
  
} TFTCalibrationData;
#pragma pack(pop)
//--------------------------------------------------------------------------------------------------------------------------------
class SettingsClass
{
  public:
    SettingsClass();

    void setup();
/*
    int GetControllerID();
    void SetControllerID(int value);
*/

  TFTCalibrationData GetCalibrationData();
  void SetCalibrationData(TFTCalibrationData& data);

  uint16_t GetMeasureTime();
  void SetMeasureTime(uint16_t val);

  void Set20_9_O2Value(uint16_t val);
  uint16_t Get20_9_O2Value();

 private:

  uint16_t adc20_9_O2Value;

  Preferences prefs;
   
template< typename T >
    bool read(const char* sectionName, const char* sectionCRCName, T& result)
    {
      uint8_t crc = prefs.getUChar(sectionCRCName);
      size_t readed = prefs.getBytes(sectionName,&result,sizeof(result));

      if(readed != sizeof(result))
        return false;

      // читаем контрольную сумму
      return crc == crc8((const uint8_t*)&result, sizeof(result));
    }

    template< typename T >
    void write(const char* sectionName, const char* sectionCRCName, T& val)
    {
        prefs.putBytes(sectionName,&val,sizeof(val));        

       // пишем контрольную сумму
        uint8_t crc = crc8((const uint8_t*)&val, sizeof(val));
        
        prefs.putUChar(sectionCRCName,crc);        

    }  
};
//--------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
//--------------------------------------------------------------------------------------------------------------------------------

