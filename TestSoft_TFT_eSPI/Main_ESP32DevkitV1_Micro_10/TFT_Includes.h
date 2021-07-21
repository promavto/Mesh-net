#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
//#include "Globals.h"
#include "TFT_eSPI.h"
#include <SPI.h>


#define CS_PIN 14
#define IRQ_PIN 13
#define MISO 19
#define MOSI 23
#define SCK 18

#define TFT_W 320
#define TFT_H 240


//--------------------------------------------------------------------------------------------------------------------------------------

#include "Fonts/GFXFF/gfxfont.h"
//#include "RusFont.h"
#include "BigRusFont.h"
#include "SmallRusFont.h"
#include "IconsFont.h"
#include "SensorFont.h"
#include "SensorFont2.h"
#include "SevenSegNumFontMDS.h"
#include "SevenSegNumFontPlus.h"
#include "Various_Symbols_32x32.h"

#define FONTTYPE const GFXfont*
//--------------------------------------------------------------------------------------------------------------------------------------
#define TFT_Class TFT_eSPI              // класс поддержки TFT
#define TOUCH_Class URTouch             // класс поддержки тача

//#define TFT_FONT (&keyrus)            // какой шрифт юзаем
#define TFT_FONT (&BigRusFont)          // какой шрифт юзаем
#define TFT_SMALL_FONT (&SmallRusFont)  // какой шрифт юзаем
#define SENSOR_FONT (&SensorFont)
#define SENSOR_FONT2 (&SensorFont2)
#define SEVEN_SEG_NUM_FONT_MDS (&SevenSegNumFontMDS)
#define SEVEN_SEG_NUM_FONT_PLUS (&SevenSegNumFontPlus)
#define VARIOUS_SYMBOLS_32x32 (&Various_Symbols_32x32)
#define ICONS_FONT (&IconsFont)

