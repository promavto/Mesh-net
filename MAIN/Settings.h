#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "AT24CX.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SettingsClass
{
  public:

    SettingsClass();

    void begin();

    // возвращает настройку кол-ва импульсов на канал
    uint16_t getChannelPulses(uint8_t channelNum);

    // сохраняет настройку кол-ва импульсов на канал
    void setChannelPulses(uint8_t channelNum, uint16_t val);

  private:

    AT24C64* eeprom;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SettingsClass Settings;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------