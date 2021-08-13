#include "Settings.h"
//--------------------------------------------------------------------------------------------------------------------------------
GlobalSettings Settings;
//--------------------------------------------------------------------------------------------------------------------------------
GlobalSettings::GlobalSettings()
{
}
//--------------------------------------------------------------------------------------------------------------------------------
String GlobalSettings::GetPassword()
{
  return MENU_PASSWORD;
}
//--------------------------------------------------------------------------------------------------------------------------------
void GlobalSettings::Set0_O2Value(uint16_t val)
{
  write("o20", "o20crc", val);
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t GlobalSettings::Get0_O2Value()
{
  uint16_t result;
 if(!read("o20", "o20crc", result))
  {
    result = 0; // нет калибровочных коэффициентов для 0% О2
  }

  return result;  
}
//--------------------------------------------------------------------------------------------------------------------------------
void GlobalSettings::Set20_9_O2Value(uint16_t val)
{
 write("o2209", "o2209crc", val); 
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t GlobalSettings::Get20_9_O2Value()
{
  uint16_t result;
 if(!read("o2209", "o2209crc", result))
  {
    result = 0; // нет калибровочных коэффициентов для 20.9% О2
  }

  return result;
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t GlobalSettings::GetCalibrationTime()
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
void GlobalSettings::SetCalibrationTime(uint16_t val)
{
  if(val < 1)
  {
    val = DEFAULT_CALIBRATION_TIME;
  }
  write("ctime", "ctimecrc", val);   
}
//--------------------------------------------------------------------------------------------------------------------------------
uint16_t GlobalSettings::GetMeasureTime()
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
void GlobalSettings::SetMeasureTime(uint16_t val)
{
  if(val < 1)
  {
    val = DEFAULT_MEASURE_TIME;
  }
  write("mtime", "mtimecrc", val); 
}
//--------------------------------------------------------------------------------------------------------------------------------
TFTCalibrationData GlobalSettings::GetTftCalibrationData()
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
void GlobalSettings::SetTftCalibrationData(TFTCalibrationData& data)
{
  data.isValid = true;
  write("cal1", "calcrc", data); 
}
//--------------------------------------------------------------------------------------------------------------------------------
void GlobalSettings::setup()
{
  prefs.begin("sett_storage7");
}
//--------------------------------------------------------------------------------------------------------------------------------

