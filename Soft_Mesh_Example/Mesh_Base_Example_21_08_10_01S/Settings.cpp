#include "Settings.h"
//--------------------------------------------------------------------------------------------------------------------------------
SettingsClass Settings;
//--------------------------------------------------------------------------------------------------------------------------------
SettingsClass::SettingsClass()
{
}
//--------------------------------------------------------------------------------------------------------------------------------
String SettingsClass::GetPassword()
{
  return MENU_PASSWORD;
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::Set0_O2Value(uint16_t val)
{
  write("o20", "o20crc", val);
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::Get0_O2Value()
{
  uint16_t result;
 if(!read("o20", "o20crc", result))
  {
    result = 0; // нет калибровочных коэффициентов для 0% О2
  }

  return result;  
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::Set20_9_O2Value(uint16_t val)
{
 write("o2209", "o2209crc", val); 
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::Get20_9_O2Value()
{
  uint16_t result;
 if(!read("o2209", "o2209crc", result))
  {
    result = 0; // нет калибровочных коэффициентов для 20.9% О2
  }

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::GetCalibrationTime()
{
  uint16_t result;
  if(!read("ctime", "ctimecrc", result))
  {
    result = DEFAULT_CALIBRATION_TIME; // по умолчанию 60 секунд на калибровку
  }

  if(result < 1)
  {
    result = DEFAULT_CALIBRATION_TIME;
  }

  return result;  
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::SetCalibrationTime(uint16_t val)
{
  if(val < 1)
  {
    val = DEFAULT_CALIBRATION_TIME;
  }
  write("ctime", "ctimecrc", val);   
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t SettingsClass::GetMeasureTime()
{
  uint16_t result;
  if(!read("mtime", "mtimecrc", result))
  {
    result = DEFAULT_MEASURE_TIME; // по умолчанию 30 секунд на изменение
  }

  if(result < 1)
  {
    result = DEFAULT_MEASURE_TIME;
  }

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::SetMeasureTime(uint16_t val)
{
  if(val < 1)
  {
    val = DEFAULT_MEASURE_TIME;
  }
  write("mtime", "mtimecrc", val); 
}
//--------------------------------------------------------------------------------------------------------------------------------
TFTCalibrationData SettingsClass::GetTftCalibrationData()
{
  TFTCalibrationData result;

  if(!read("cal1", "calcrc", result))
  {
    result.isValid = false;
    return result;
  }
  
  result.isValid = true;
  return result;  
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::SetTftCalibrationData(TFTCalibrationData& data)
{
  data.isValid = true;
  write("cal1", "calcrc", data); 
}
//--------------------------------------------------------------------------------------------------------------------------------
void SettingsClass::setup()
{
  prefs.begin("sett_storage7");
}
//--------------------------------------------------------------------------------------------------------------------------------

