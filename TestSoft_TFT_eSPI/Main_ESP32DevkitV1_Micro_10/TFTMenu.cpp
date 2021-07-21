#include "TFTMenu.h"
//#include "AbstractModule.h"
//#include "URTouch_ILI9341.h"
//#include <SPI.h>


#ifdef USE_TFT_MODULE


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint8_t utf8_rus_charmap[] PROGMEM = { 'A',128,'B',129,130,'E',131,132,133,134,135,'K',136,'M','H','O',137,'P','C','T',138,139,'X',140,141,
142,143,144,145,146,147,148,149,'a',150,151,152,153,'e',154,155,156,157,158,159,160,161,162,'o',163,'p','c',164,'y',165,'x',166,167,168,169,170,
171,172,173,174,175 };
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void buttonPressed(int btn) // вызывается по нажатию на кнопку - тут можно пищать баззером, например)
{
	if (btn != -1)
	{
		// BUZZ!!!!
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void buttonDrawed() // вызывается после отрисовки каждой кнопки
{
	yield();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int rusPrintFunc(const char* st, int x, int y, int deg, bool computeStringLengthOnly)
{
	//return Screen.print(st, x, y, deg, computeStringLengthOnly);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen::AbstractTFTScreen(const char* name)
{
	screenName = name;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen::~AbstractTFTScreen()
{

	delete screenButtons;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::setup(TFTMenu* menu)
{

	TFT_Class* dc = menu->getDC();
	
	if (!dc)
	{
	return;
	}
	screenButtons = new TFT_Buttons_Rus(dc, menu->getTouch(), menu->getRusPrinter());
	screenButtons->setTextFont(TFT_FONT);
	screenButtons->setSymbolFont(SENSOR_FONT);
	screenButtons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);

	// тут общие для всех классов настройки
	doSetup(menu);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::notifyButtonPressed(int pressedButton)
{
	Screen.notifyAction(this);
	onButtonPressed(&Screen, pressedButton);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::update(TFTMenu* menu)
{
	if (isActive())
	{
#if DISPLAY_USED == DISPLAY_ILI9341 
		int pressedButton = screenButtons->checkButtons(buttonPressed);

		if (pressedButton != -1)
		{
			notifyButtonPressed(pressedButton);
			//menu->notifyAction(this);
			//onButtonPressed(menu, pressedButton);
		}
#endif

		if (isActive())
			doUpdate(menu);
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::draw(TFTMenu* menu)
{
	if (isActive())
	{
		doDraw(menu);

		if (isActive())
		{
			screenButtons->drawButtons(buttonDrawed);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// TFTMenu
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TFTMenu::TFTMenu()
{
	currentScreenIndex = -1;
	requestedToActiveScreen = NULL;
	requestedToActiveScreenIndex = -1;
	on_action = NULL;
	halDCDescriptor = NULL;
	//curFont = NULL;
	halTouch = NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::notifyAction(AbstractTFTScreen* screen)
{
	if (on_action)
		on_action(screen);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::addScreen(AbstractTFTScreen* screen)
{
	screen->setup(this);
	screens.push_back(screen);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::initHAL()
{
	//Тут инициализация/переинициализация дисплея
	halDCDescriptor->init();
	halDCDescriptor->setRotation(1);
	halDCDescriptor->fillScreen(TFT_BACK_COLOR);

	halDCDescriptor->setFreeFont(TFT_FONT);

	halDCDescriptor->setTextColor(TFT_RED, TFT_BACK_COLOR);

	halDCDescriptor->drawString("Test TFT", 40, 40);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::setup()
{
	//создание библиотеки для экрана


	halDCDescriptor = new TFT_eSPI();
	halTouch = new TOUCH_Class(CS_PIN, IRQ_PIN);

// инициализируем дисплей
	initHAL();

	//// добавляем экран мессадж-бокса
	//addScreen(MessageBoxScreen::create());

	//// добавляем экран экранной клавиатуры
	//addScreen(KeyboardScreen::create());


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::update()
{

	if (requestedToActiveScreen != NULL)
	{
		// попросили сделать экран активным
		AbstractTFTScreen* screen = requestedToActiveScreen;
		currentScreenIndex = requestedToActiveScreenIndex;

		requestedToActiveScreen = NULL;

		screen->setActive(true);
		screen->onActivate();

		//Тут очистка экрана
		clearScreen();

		screen->update(this);
		screen->draw(this);

		return;

	} // if(requestedToActiveScreen != NULL)

	if (currentScreenIndex == -1) // ни разу не рисовали ещё ничего, исправляемся
	{
		if (screens.size())
			switchToScreen((unsigned int)0); // переключаемся на первый экран, если ещё ни разу не показали ничего     
	}

	if (currentScreenIndex == -1)
		return;

	// обновляем текущий экран
	AbstractTFTScreen* currentScreen = screens[currentScreenIndex];
	currentScreen->update(this);


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen* TFTMenu::getActiveScreen()
{
	if (currentScreenIndex < 0 || !screens.size())
		return NULL;

	return screens[currentScreenIndex];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::switchToScreen(AbstractTFTScreen* screen)
{

	if (requestedToActiveScreen != NULL) // ждём переключения на новый экран
		return;

	if (currentScreenIndex > -1 && screens.size())
	{
		AbstractTFTScreen* si = screens[currentScreenIndex];
		si->setActive(false);
		si->onDeactivate();
	}

	for (size_t i = 0; i < screens.size(); i++)
	{
		if (screens[i] == screen)
		{
			requestedToActiveScreen = screen;
			requestedToActiveScreenIndex = i;

			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::switchToScreen(unsigned int screenIndex)
{
	if (screenIndex < screens.size())
		switchToScreen(screens[screenIndex]);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::switchToScreen(const char* screenName)
{

	// переключаемся на запрошенный экран
	for (size_t i = 0; i < screens.size(); i++)
	{
		AbstractTFTScreen* si = screens[i];
		if (!strcmp(si->getName(), screenName))
		{
			switchToScreen(si);
			break;
		}
	}

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///*
//String TFTMenu::utf8rus(const char* source)
//{
//  int i,k;
//  String target;
//  unsigned char n;
//  char m[2] = { '0', '\0' };
//
//  k = strlen(source); i = 0;
//
//  while (i < k) {
//	n = source[i]; i++;
//
//	if (n >= 0xC0) {
//	  switch (n) {
//		case 0xD0: {
//		  n = source[i]; i++;
//		  if (n == 0x81) { n = 0xA8; break; }
//		  if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
//		  break;
//		}
//		case 0xD1: {
//		  n = source[i]; i++;
//		  if (n == 0x91) { n = 0xB8; break; }
//		  if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
//		  break;
//		}
//	  }
//	}
//	m[0] = n; target = target + String(m);
//  }
//return target;
//
//}
//*/
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void TFTMenu::setFont(FONT_TYPE* font)
//{
//	curFont = font;
//
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->setFont(font);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//FONT_TYPE* TFTMenu::getFont()
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return halDCDescriptor->getFont();
//#else
//#error "Unsupported display!"
//#endif    
//}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::clearScreen()
{
	halDCDescriptor->fillScreen(TFT_BACK_COLOR);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void TFTMenu::fillScreen(COLORTYPE color)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->fillScr(color);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void TFTMenu::setBackColor(COLORTYPE color)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->setBackColor(color);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//COLORTYPE  TFTMenu::getBackColor()
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return halDCDescriptor->getBackColor();
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void TFTMenu::setColor(COLORTYPE color)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->setColor(color);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//COLORTYPE  TFTMenu::getColor()
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return halDCDescriptor->getColor();
//#else
//#error "Unsupported display!"
//#endif  
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void  TFTMenu::drawRect(int x1, int y1, int x2, int y2)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->drawRect(x1, y1, x2, y2);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void  TFTMenu::drawRoundRect(int x1, int y1, int x2, int y2)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->drawRoundRect(x1, y1, x2, y2);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void  TFTMenu::fillRect(int x1, int y1, int x2, int y2)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->fillRect(x1, y1, x2, y2);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void  TFTMenu::fillRoundRect(int x1, int y1, int x2, int y2)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	halDCDescriptor->fillRoundRect(x1, y1, x2, y2);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void  TFTMenu::clrRoundRect(int x1, int y1, int x2, int y2)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	//halDCDescriptor->fillRoundRect(x1,y1,x2,y2);
//#else
//#error "Unsupported display!"
//#endif    
//}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint16_t TFTMenu::getFontWidth(FONT_TYPE* font)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341    
//	return READ_FONT_BYTE(0);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint16_t TFTMenu::getFontHeight(FONT_TYPE* font)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return READ_FONT_BYTE(1);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint16_t TFTMenu::getScreenWidth()
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return halDCDescriptor->getDisplayXSize();
//#else
//#error "Unsupported display!"
//#endif     
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//uint16_t TFTMenu::getScreenHeight()
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return halDCDescriptor->getDisplayYSize();
//#else
//#error "Unsupported display!"
//#endif     
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
//int TFTMenu::print(const char* st, int x, int y, int deg, bool computeStringLengthOnly)
//{
//#if DISPLAY_USED == DISPLAY_ILI9341
//	return printILI(st, x, y, deg, computeStringLengthOnly);
//#else
//#error "Unsupported display!"
//#endif    
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#if DISPLAY_USED == DISPLAY_ILI9341
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//int TFTMenu::printILI(const char* st, int x, int y, int deg, bool computeStringLengthOnly)
//{
//
//	int stl, i;
//	stl = strlen(st);
//
//	if (halDCDescriptor->orient == PORTRAIT)
//	{
//		if (x == RIGHT)
//			x = (halDCDescriptor->disp_x_size + 1) - (stl*halDCDescriptor->cfont.x_size);
//
//		if (x == CENTER)
//			x = ((halDCDescriptor->disp_x_size + 1) - (stl*halDCDescriptor->cfont.x_size)) / 2;
//	}
//	else
//	{
//		if (x == RIGHT)
//			x = (halDCDescriptor->disp_y_size + 1) - (stl*halDCDescriptor->cfont.x_size);
//
//		if (x == CENTER)
//			x = ((halDCDescriptor->disp_y_size + 1) - (stl*halDCDescriptor->cfont.x_size)) / 2;
//	}
//
//	uint8_t utf_high_byte = 0;
//	uint8_t ch, ch_pos = 0;
//
//	for (i = 0; i < stl; i++)
//	{
//		ch = st[i];
//
//		if (ch >= 128)
//		{
//			if (utf_high_byte == 0 && (ch == 0xD0 || ch == 0xD1))
//			{
//				utf_high_byte = ch;
//				continue;
//			}
//			else
//			{
//				if (utf_high_byte == 0xD0)
//				{
//					if (ch == 0x81)
//					{ //Ё
//						ch = 6;
//					}
//					else
//					{
//						if (ch <= 0x95)
//						{
//							ch -= 0x90;
//						}
//						else if (ch < 0xB6)
//						{
//							ch -= (0x90 - 1);
//						}
//						else
//						{
//							ch -= (0x90 - 2);
//						}
//					}
//
//					ch = pgm_read_byte((utf8_rus_charmap + ch));
//
//				}
//				else if (utf_high_byte == 0xD1)
//				{
//					if (ch == 0x91)
//					{//ё
//						ch = 39;
//					}
//					else
//					{
//						ch -= 0x80;
//						ch += 50;
//					}
//
//					ch = pgm_read_byte((utf8_rus_charmap + ch));
//				}
//
//				utf_high_byte = 0;
//			}
//		}
//		else
//		{
//			utf_high_byte = 0;
//		}
//
//
//		if (deg == 0)
//		{
//			if (!computeStringLengthOnly)
//				halDCDescriptor->printChar(ch, x + (ch_pos * (halDCDescriptor->cfont.x_size)), y);
//		}
//		else
//		{
//			if (!computeStringLengthOnly)
//				halDCDescriptor->rotateChar(ch, x, y, ch_pos, deg);
//		}
//		++ch_pos;
//	} // for  
//
//	return ch_pos;
//}
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#endif // DISPLAY_USED == DISPLAY_ILI9341
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TFTMenu Screen;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
////==============================================================================================================================
////
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////#define FONT_HEIGHT(dc) dc->fontHeight(1)
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////static const char* WEEKDAYS[] = {
////
////  "ПН",
////  "ВТ",
////  "СР",
////  "ЧТ",
////  "ПТ",
////  "СБ",
////  "ВС"
////
////};
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////unsigned int utf8GetCharSize(unsigned char bt) 
////{ 
////  if (bt < 128) 
////  return 1; 
////  else if ((bt & 0xE0) == 0xC0) 
////  return 2; 
////  else if ((bt & 0xF0) == 0xE0) 
////  return 3; 
////  else if ((bt & 0xF8) == 0xF0) 
////  return 4; 
////  else if ((bt & 0xFC) == 0xF8) 
////  return 5; 
////  else if ((bt & 0xFE) == 0xFC) 
////  return 6; 
////
//// 
////  return 1; 
////} 
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void ButtonPressed(int btn)
////{
////  if(btn != -1)
////  {
////    #ifdef USE_BUZZER
////    Buzzer.buzz();
////    #endif
////  }
////
////  TFTScreen->onButtonPressed(btn);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void ButtonReleased(int btn)
////{
////  TFTScreen->onButtonReleased(btn);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawButtonsYield() // вызывается после отрисовки каждой кнопки
////{
////  yield();
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawScreenCaption(TFTMenu* hal, const String& str) // рисуем заголовок экрана
////{
////
////  TFT_Class* dc = hal->getDC();
////  
////  if(!dc)
////  {
////    return;
////  }
////  
////  TFTRus* rusPrinter = hal->getRusPrinter();
////  
////  int screenWidth = dc->width();
////  
////  dc->setFreeFont(TFT_FONT);
////  
////  int fontHeight = FONT_HEIGHT(dc);
////  int top = 10;
////
////  // подложка под заголовок
////  dc->fillRect(0, 0, screenWidth, top*2 + fontHeight, TFT_NAVY);
////   
////  int left = (screenWidth - rusPrinter->textWidth(str.c_str()))/2;
////
////  rusPrinter->print(str.c_str(),left,top, TFT_NAVY, TFT_WHITE);    
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawTimeInBox(TFTInfoBox* box, uint32_t val)
////{
////  TFT_Class* dc = TFTScreen->getDC();
////  
////  if(!dc)
////  {
////    return;
////  }
////  
////  TFTRus* rusPrinter = TFTScreen->getRusPrinter();
////  
////  TFTInfoBoxContentRect rc =  box->getContentRect(TFTScreen);
////  dc->fillRect(rc.x,rc.y,rc.w,rc.h, INFO_BOX_BACK_COLOR);
////  yield();
////
////  dc->setFreeFont(SEVEN_SEG_NUM_FONT_PLUS);
////
////  uint8_t hours = val/60;
////  uint8_t mins = val%60;
////
////  String strVal;
////  if(hours < 10)
////    strVal += '0';
////
////  strVal += hours;
////  strVal += ':';
////
////  if(mins < 10)
////    strVal += '0';
////
////  strVal += mins;
////
////  
////  int fontHeight = FONT_HEIGHT(dc);
////  int strLen = rusPrinter->textWidth(strVal.c_str());
////
////  int leftPos = rc.x + (rc.w - strLen)/2;
////  int topPos = rc.y + (rc.h - fontHeight)/2;
////  rusPrinter->print(strVal.c_str(),leftPos,topPos,INFO_BOX_BACK_COLOR,SENSOR_BOX_FONT_COLOR);
////  yield();
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, const String& strVal, FONTTYPE font)
////{
////  TFT_Class* dc = TFTScreen->getDC();
////  
////  if(!dc)
////  {
////    return;
////  }
////  
////  TFTRus* rusPrinter = TFTScreen->getRusPrinter();
////  
////  TFTInfoBoxContentRect rc =  box->getContentRect(TFTScreen);
////  dc->fillRect(rc.x,rc.y,rc.w,rc.h, INFO_BOX_BACK_COLOR);
////  yield();
////
////  dc->setFreeFont(font);
////
////  
////  int fontHeight = FONT_HEIGHT(dc);
////  int strLen = rusPrinter->textWidth(strVal.c_str());
////
////  int leftPos = rc.x + (rc.w - strLen)/2;
////  int topPos = rc.y + (rc.h - fontHeight)/2;
////  rusPrinter->print(strVal.c_str(),leftPos,topPos,INFO_BOX_BACK_COLOR,SENSOR_BOX_FONT_COLOR);
////  yield();
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, int val)
////{
////  return drawValueInBox(box,String(val));
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, int16_t val)
////{
////  return drawValueInBox(box,String(val));
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, uint16_t val)
////{
////  return drawValueInBox(box,String(val));
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, int8_t val)
////{
////  return drawValueInBox(box,String(val));
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, uint8_t val)
////{
////  return drawValueInBox(box,String(val));
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//////void drawValueInBox(TFTInfoBox* box, int32_t val)
//////{
//////  return drawValueInBox(box,String(val));
//////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void drawValueInBox(TFTInfoBox* box, uint32_t val)
////{
////  return drawValueInBox(box,String(val));
////}
////
////
////
////
////
////
////
////
////
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTInfoBox
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TFTInfoBox::TFTInfoBox(const char* caption, int width, int height, int x, int y, int cxo)
////{
////  boxCaption = caption;
////  boxWidth = width;
////  boxHeight = height;
////  posX = x;
////  posY = y;
////  captionXOffset = cxo;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TFTInfoBox::~TFTInfoBox()
////{
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTInfoBox::drawCaption(TFTMenu* menuManager, const char* caption)
////{
////  TFT_Class* dc = menuManager->getDC();
////  if(!dc)
////  {
////    return;
////  }  
////  
////  dc->setFreeFont(TFT_FONT);
////  
//////  caption = "TEST CAPTION";
////  menuManager->getRusPrinter()->print(caption,posX+captionXOffset,posY,TFT_BACK_COLOR,INFO_BOX_CAPTION_COLOR);  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTInfoBox::draw(TFTMenu* menuManager)
////{
////  drawCaption(menuManager,boxCaption);
////  
////  int curTop = posY;
////
////  TFT_Class* dc = menuManager->getDC();
////  if(!dc)
////  {
////    return;
////  }
////
////  dc->setFreeFont(TFT_FONT);
////    
////  int fontHeight = FONT_HEIGHT(dc);
//////  Serial.print("Font height: "); Serial.println(fontHeight);
////  
////  curTop += fontHeight + INFO_BOX_CONTENT_PADDING;
////
////  dc->fillRoundRect(posX, curTop, boxWidth, (boxHeight - fontHeight - INFO_BOX_CONTENT_PADDING),2,INFO_BOX_BACK_COLOR);
////
////  yield();
////
////  dc->drawRoundRect(posX, curTop, boxWidth, (boxHeight - fontHeight - INFO_BOX_CONTENT_PADDING),2,INFO_BOX_BORDER_COLOR);
////
////  yield();
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TFTInfoBoxContentRect TFTInfoBox::getContentRect(TFTMenu* menuManager)
////{
////    TFTInfoBoxContentRect result;
////    TFT_Class* dc = menuManager->getDC();
////	
////	if(!dc)
////	{
////		return result;
////	}	
////
////    dc->setFreeFont(TFT_FONT);
////    
////    int fontHeight = FONT_HEIGHT(dc);
////
////    result.x = posX + INFO_BOX_CONTENT_PADDING;
////    result.y = posY + fontHeight + INFO_BOX_CONTENT_PADDING*2;
////
////    result.w = boxWidth - INFO_BOX_CONTENT_PADDING*2;
////    result.h = boxHeight - (fontHeight + INFO_BOX_CONTENT_PADDING*3);
////
////    return result;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////AbstractTFTScreen::AbstractTFTScreen()
////{
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////AbstractTFTScreen::~AbstractTFTScreen()
////{
////  
////}
////
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTMenu
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TFTMenu* TFTScreen = NULL;
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TFTMenu::TFTMenu()
////{
////  TFTScreen = this;
////  currentScreenIndex = -1;
////  flags.isLCDOn = true;
////  switchTo = NULL;
////  switchToIndex = -1;
////  tftDC = NULL;
////  tftTouch = NULL;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::setup()
////{
////  //TFTBinding bnd = HardwareBinding->GetTFTBinding();
////  //if(!(bnd.DisplayType == 1 || bnd.DisplayType == 2))
////  //{
////  //  return; // нет привязки
////  //}	
////	
////  lcdOn(); // включаем подсветку
////
////  int rot = 1;
////  int dRot = 1;
////
////  
////    tftDC = new TFT_eSPI();
////
////	tftDC->init();
////	tftDC->setRotation(dRot);
////	tftDC->fillScreen(TFT_BACK_COLOR);
////
////	tftDC->setFreeFont(TFT_FONT);
////
////	tftDC->setTextColor(TFT_RED, TFT_BACK_COLOR);
////
////	tftDC->drawString("Test TFT", 40, 40);
////
////
////	tftTouch = new TOUCH_Class(CS_PIN, IRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
////
////	tftTouch->setRotation(rot);
////	tftTouch->begin();
////
////
////  rusPrint.init(tftDC);
//// // TFTRus* rusPrinter = new getRusPrinter();
////
////  rusPrint.print("Проба", 40, 80, TFT_BACK_COLOR, TFT_FONT_COLOR);
////
////  resetIdleTimer();
////
////  // добавляем служебные экраны
////
////  // окно сообщения
////  TFTScreenInfo mbscrif;
////  mbscrif.screen = MessageBoxScreen::create();
////  mbscrif.screen->setup(this);
////  mbscrif.screenName = "MB";
////  screens.push_back(mbscrif);
////
////  // клавиатура
////  mbscrif.screen = KeyboardScreen::create();
////  mbscrif.screen->setup(this);
////  mbscrif.screenName = "KBD";
////  screens.push_back(mbscrif);
////
////  AbstractTFTScreen* wmScreen = new TFTWMScreen(0);
////  wmScreen->setup(this);
////  TFTScreenInfo wmss;
////  wmss.screenName = "WMS1";
////  wmss.screen = wmScreen;
////  screens.push_back(wmss);
////
////  //wmScreen = new TFTWMenuScreen();
////  //wmScreen->setup(this);
////  //wmss.screenName = "WMS";
////  //wmss.screen = wmScreen;
////  //screens.push_back(wmss);
////
////  //TFTMenuScreen
////  AbstractTFTScreen* MScreen = new TFTMenuScreen(0);
////  MScreen->setup(this);
////  TFTScreenInfo mmss;
////  mmss.screenName = "MENU";
////  mmss.screen = MScreen;
////  screens.push_back(mmss);
////
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::switchBacklight(uint8_t level)
////{
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::lcdOn()
////{
////    switchBacklight(1);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::lcdOff()
////{
////    switchBacklight(0);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::onButtonPressed(int button)
////{
////  if(currentScreenIndex == -1)
////    return;
////
////  TFTScreenInfo* currentScreenInfo = &(screens[currentScreenIndex]);
////  currentScreenInfo->screen->onButtonPressed(this, button);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::onButtonReleased(int button)
////{
////  if(currentScreenIndex == -1)
////    return;
////
////  TFTScreenInfo* currentScreenInfo = &(screens[currentScreenIndex]);
////  currentScreenInfo->screen->onButtonReleased(this, button);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::update()
////{
////  if(!tftDC)
////  {
////    return;
////  }
////	
////	
////  //TFTBinding bnd = HardwareBinding->GetTFTBinding();
////  //if(!(bnd.DisplayType == 1 || bnd.DisplayType == 2))
////  //{
////  //  return; // нет привязки
////  //}  
////	
////	
////  if(currentScreenIndex == -1 && !switchTo) // ни разу не рисовали ещё ничего, исправляемся
////  {
////     switchToScreen("MENU"); // переключаемся на стартовый экран, если ещё ни разу не показали ничего     
////  }
////
////  if(switchTo != NULL)
////  {
////      tftDC->fillScreen(TFT_BACK_COLOR); // clear screen first      
////      yield();
////      currentScreenIndex = switchToIndex;
////      switchTo->onActivate(this);
////      switchTo->update(this);
////      yield();
////      switchTo->draw(this);
////      yield();
////      resetIdleTimer(); // сбрасываем таймер ничегонеделанья
////
////      switchTo = NULL;
////      switchToIndex = -1;
////    return;
////  }
////
////
////  if(flags.isLCDOn)
////  {
////	//TFTBinding bnd = HardwareBinding->GetTFTBinding();
//// //   if(millis() - idleTimer > bnd.OffDelay)
////	//{
//// //     flags.isLCDOn = false;
//// //     lcdOff();
//// //   }
////  }
////  else
////  {
////    // LCD currently off, check the touch on screen
////    if(tftTouch->dataAvailable())
////    {
////      tftTouch->getPoint();
////
////      while(tftTouch->dataAvailable())
////      {
////        yield();
////      }
////      
////      lcdOn();
////      resetIdleTimer();
////      flags.isLCDOn = true;
////    }
////  }
////
////  // обновляем текущий экран
////  TFTScreenInfo* currentScreenInfo = &(screens[currentScreenIndex]);
////  currentScreenInfo->screen->update(this);
////  yield();
////  
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::switchToScreen(AbstractTFTScreen* to)
////{
////	if(!tftDC)
////	{
////		return;
////	}
////   // переключаемся на запрошенный экран
////  for(size_t i=0;i<screens.size();i++)
////  {
////    TFTScreenInfo* si = &(screens[i]);
////    if(si->screen == to)
////    {
////      switchTo = si->screen;
////      switchToIndex = i;
////      break;
////
////    }
////  } 
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TFTMenu::switchToScreen(const char* screenName)
////{
////	if(!tftDC)
////	{
////		return;
////	}
////  
////  // переключаемся на запрошенный экран
////  for(size_t i=0;i<screens.size();i++)
////  {
////    TFTScreenInfo* si = &(screens[i]);
////    if(!strcmp(si->screenName,screenName))
////    {
////      switchTo = si->screen;
////      switchToIndex = i;
////      break;
////
////    }
////  }
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenu::resetIdleTimer()
////{
////  idleTimer = millis();
////}
////
//// //========================================================================================================================================================================
////
////
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTMenuScreen
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// TFTMenuScreen::TFTMenuScreen(uint8_t _channel)
//// {
////	
////
////	 channel = _channel;
////
////	 tickerButton = -1;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// TFTMenuScreen::~TFTMenuScreen()
//// {
////	 delete screenButtons;
////	
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenuScreen::onActivate(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// 
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenuScreen::setup(TFTMenu* menuManager)
//// {
////
////
////	 TFT_Class* dc = menuManager->getDC();
////
////	 if (!dc)
////	 {
////		 return;
////	 }
////
////	 screenButtons = new TFT_Buttons_Rus(dc, menuManager->getTouch(), menuManager->getRusPrinter());
////	 screenButtons->setTextFont(TFT_FONT);
////	 screenButtons->setSymbolFont(SENSOR_FONT);
////
////	 screenButtons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);
////
////	 TFTRus* rusPrinter = menuManager->getRusPrinter();
////
////	 int screenWidth = dc->width();
////	 int screenHeight = dc->height();
////
////	 dc->setFreeFont(TFT_FONT);
////	 int textFontHeight = FONT_HEIGHT(dc);
////	 int textFontWidth = dc->textWidth("w", 1);
////
////	 //// вычисляем ширину всего занятого пространства
////	 //int widthOccupied = TFT_TEXT_INPUT_WIDTH * 2 + TFT_ARROW_BUTTON_WIDTH * 4 + INFO_BOX_V_SPACING * 6;
////
////	 //// теперь вычисляем левую границу для начала рисования
////	 //int leftPos = (screenWidth - widthOccupied) / 2;
////	 //int initialLeftPos = leftPos;
////
////	 //// теперь вычисляем верхнюю границу для отрисовки кнопок
////	 //int topPos = INFO_BOX_V_SPACING * 2;
////	 //int secondRowTopPos = topPos + TFT_ARROW_BUTTON_HEIGHT + INFO_BOX_V_SPACING * 2;
////	 //int thirdRowTopPos = secondRowTopPos + TFT_ARROW_BUTTON_HEIGHT + INFO_BOX_V_SPACING * 2;
////
////	 //const int spacing = 10;
////
////	 //int buttonHeight = TFT_ARROW_BUTTON_HEIGHT;
////
////	 //int controlsButtonsWidth = (screenWidth - spacing * 2 - initialLeftPos * 2) / 3;
////	 //int controlsButtonsTop = screenHeight - buttonHeight - spacing;
////	 //// первая - кнопка назад
////	 backButton = screenButtons->addButton(5, 10, 200, 30, WM_BACK_CAPTION);
////	 //saveButton = screenButtons->addButton(initialLeftPos + spacing + controlsButtonsWidth, controlsButtonsTop, controlsButtonsWidth, buttonHeight, WM_SAVE_CAPTION);
////	 //onOffButton = screenButtons->addButton(initialLeftPos + spacing * 2 + controlsButtonsWidth * 2, controlsButtonsTop, controlsButtonsWidth, buttonHeight, WM_ON_CAPTION);
////
////	 //screenButtons->setButtonFontColor(onOffButton, CHANNELS_BUTTONS_TEXT_COLOR);
////	 //screenButtons->disableButton(saveButton);
////
////
////	 //static char leftArrowCaption[2] = { 0 };
////	 //static char rightArrowCaption[2] = { 0 };
////
////	 //leftArrowCaption[0] = rusPrinter->mapChar(charLeftArrow);
////	 //rightArrowCaption[0] = rusPrinter->mapChar(charRightArrow);
////
////	 //int textBoxHeightWithCaption = TFT_TEXT_INPUT_HEIGHT + textFontHeight + INFO_BOX_CONTENT_PADDING;
////	 //int textBoxTopPos = topPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////
////	 //// теперь добавляем наши кнопки
////
////
////	 //// вторая строка
////	 //textBoxTopPos = secondRowTopPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////	 //topPos = secondRowTopPos;
////	 //leftPos = initialLeftPos;
////
////
////	 //// третья строка
////	 //textBoxTopPos = thirdRowTopPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////	 //topPos = thirdRowTopPos;
////	 //leftPos = initialLeftPos;
////
////	 //Serial.println("TFTMenuScreen SETUP DONE!"); Serial.flush();
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////  void TFTMenuScreen::onButtonPressed(TFTMenu* menuManager, int buttonID)
//// {
////	 tickerButton = -1;
////	 if (buttonID == backButton)
////	 {
////		 Serial.println(" Test button");
////	 }
////
////	 //if (buttonID == dec25PercentsButton || buttonID == inc25PercentsButton || buttonID == dec50PercentsButton
////		// || buttonID == inc50PercentsButton || buttonID == dec75PercentsButton || buttonID == inc75PercentsButton
////		// || buttonID == dec100PercentsButton || buttonID == inc100PercentsButton)
////	 //{
////		// tickerButton = buttonID;
////		// Ticker.start(this);
////	 //}
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenuScreen::onButtonReleased(TFTMenu* menuManager, int buttonID)
//// {
////	 Ticker.stop();
////	 tickerButton = -1;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenuScreen::onTick()
//// {
////	/* if (tickerButton == dec25PercentsButton)
////		 inc25Temp(-3);
////	 else
////		 if (tickerButton == inc25PercentsButton)
////			 inc25Temp(3);
////		 else
////			 if (tickerButton == dec50PercentsButton)
////				 inc50Temp(-3);
////			 else
////				 if (tickerButton == inc50PercentsButton)
////					 inc50Temp(3);
////				 else
////					 if (tickerButton == dec75PercentsButton)
////						 inc75Temp(-3);
////					 else
////						 if (tickerButton == inc75PercentsButton)
////							 inc75Temp(3);
////						 else
////							 if (tickerButton == dec100PercentsButton)
////								 inc100Temp(-3);
////							 else
////								 if (tickerButton == inc100PercentsButton)
////									 inc100Temp(3);*/
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////  void TFTMenuScreen::update(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTMenuScreen::draw(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////	 if (screenButtons)
////	 {
////		 screenButtons->drawButtons(drawButtonsYield);
////	 }
////
////
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////
//////
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTWMScreen
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// TFTWMScreen::TFTWMScreen(uint8_t _channel)
//// {
////	 percents25Box = NULL;
////	 percents50Box = NULL;
////	 percents75Box = NULL;
////	 percents100Box = NULL;
////	 histeresisBox = NULL;
////	 sensorBox = NULL;
////
////	 percents25Temp = WM_25PERCENTS_OPEN_TEMP;
////	 percents50Temp = WM_50PERCENTS_OPEN_TEMP;
////	 percents75Temp = WM_75PERCENTS_OPEN_TEMP;
////	 percents100Temp = WM_100PERCENTS_OPEN_TEMP;
////	 histeresis = WM_DEFAULT_HISTERESIS;
////	 sensor = 0;
////	 isActive = WM_ACTIVE_FLAG;
////
////	 channel = _channel;
////
////	 sensorDataLeft = sensorDataTop = 0;
////	 tickerButton = -1;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// TFTWMScreen::~TFTWMScreen()
//// {
////	 delete screenButtons;
////	 delete percents25Box;
////	 delete percents50Box;
////	 delete percents75Box;
////	 delete percents100Box;
////	 delete histeresisBox;
////	 delete sensorBox;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::onActivate(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////	 //GlobalSettings* settings = MainController->GetSettings();
////
////	 //percents25Temp = settings->Get25PercentsOpenTemp(channel);
////	 //percents50Temp = settings->Get50PercentsOpenTemp(channel);
////	 //percents75Temp = settings->Get75PercentsOpenTemp(channel);
////	 //percents100Temp = settings->Get100PercentsOpenTemp(channel);
////	 //histeresis = settings->GetWMHisteresis(channel);
////	 //sensor = settings->GetWMSensor(channel);
////	 //isActive = settings->GetWMActive(channel);
////
////	 //if (isActive)
////	 //{
////		// screenButtons->setButtonBackColor(onOffButton, MODE_ON_COLOR);
////		// screenButtons->relabelButton(onOffButton, WM_ON_CAPTION);
////	 //}
////	 //else
////	 //{
////		// screenButtons->setButtonBackColor(onOffButton, MODE_OFF_COLOR);
////		// screenButtons->relabelButton(onOffButton, WM_OFF_CAPTION);
////	 //}
////
////	 //screenButtons->disableButton(saveButton);
////
////	 //blinkActive = false;
////	 //screenButtons->setButtonFontColor(saveButton, WM_BLINK_OFF_TEXT_COLOR);
////	 //screenButtons->setButtonBackColor(saveButton, WM_OFF_BLINK_BGCOLOR);
////
////	 //AbstractModule* module = MainController->GetModuleByID("STATE");
////	 //if (module)
////		// tempSensorsCount = module->State.GetStateCount(StateTemperature);
////	 //else
////		// tempSensorsCount = 0;
////
////	 //module = MainController->GetModuleByID("HUMIDITY");
////	 //if (module)
////		// humiditySensorsCount = module->State.GetStateCount(StateTemperature);
////	 //else
////		// humiditySensorsCount = 0;
////
////
////	 //getSensorData();
////
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::getSensorData()
//// {
////	 sensorDataString = F("^ - ");
////	 sensorDataString += char('~' + 1);
////	 sensorDataString += F("C    ");
////
////	 const char* moduleName = "STATE";
////	 uint16_t sensorIndex = sensor;
////
////	 if (sensorIndex >= tempSensorsCount)
////	 {
////		 sensorIndex -= tempSensorsCount;
////		 moduleName = "HUMIDITY";
////	 }
////
////	/* AbstractModule* module = MainController->GetModuleByID(moduleName);
////	 if (!module)
////		 return;*/
////
////	 //OneState* sensorState = module->State.GetState(StateTemperature, sensorIndex);
////	 //if (!sensorState)
////		// return;
////
////	 //if (sensorState->HasData())
////	 //{
////		// TemperaturePair tmp = *sensorState;
////		// sensorDataString = F("^");
////
////		// sensorDataString += tmp.Current;
////
////		// sensorDataString += char('~' + 1);
////		// sensorDataString += 'C';
////
////	 //}
////
////	 //while (sensorDataString.length() < 10)
////		// sensorDataString += ' ';
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::drawSensorData(TFTMenu* menuManager)
//// {
////	 TFT_Class* dc = menuManager->getDC();
////	 dc->setFreeFont(TFT_FONT);
////
////	 dc->setTextColor(TFT_RED, TFT_BACK_COLOR);
////	 dc->drawString(sensorDataString.c_str(), sensorDataLeft, sensorDataTop, 1);
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::setup(TFTMenu* menuManager)
//// {
////
////
////	 TFT_Class* dc = menuManager->getDC();
////
////	 if (!dc)
////	 {
////		 return;
////	 }
////
////	 screenButtons = new TFT_Buttons_Rus(dc, menuManager->getTouch(), menuManager->getRusPrinter());
////	 screenButtons->setTextFont(TFT_FONT);
////	 screenButtons->setSymbolFont(SENSOR_FONT);
////
////	 screenButtons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);
////
////	 TFTRus* rusPrinter = menuManager->getRusPrinter();
////
////	 int screenWidth = dc->width();
////	 int screenHeight = dc->height();
////
////	 dc->setFreeFont(TFT_FONT);
////	 int textFontHeight = FONT_HEIGHT(dc);
////	 int textFontWidth = dc->textWidth("w", 1);
////
////	 // вычисляем ширину всего занятого пространства
////	 int widthOccupied = TFT_TEXT_INPUT_WIDTH * 2 + TFT_ARROW_BUTTON_WIDTH * 4 + INFO_BOX_V_SPACING * 6;
////
////	 // теперь вычисляем левую границу для начала рисования
////	 int leftPos = (screenWidth - widthOccupied) / 2;
////	 int initialLeftPos = leftPos;
////
////	 // теперь вычисляем верхнюю границу для отрисовки кнопок
////	 int topPos = INFO_BOX_V_SPACING * 2;
////	 int secondRowTopPos = topPos + TFT_ARROW_BUTTON_HEIGHT + INFO_BOX_V_SPACING * 2;
////	 int thirdRowTopPos = secondRowTopPos + TFT_ARROW_BUTTON_HEIGHT + INFO_BOX_V_SPACING * 2;
////
////	 const int spacing = 10;
////
////	 int buttonHeight = TFT_ARROW_BUTTON_HEIGHT;
////
////	 int controlsButtonsWidth = (screenWidth - spacing * 2 - initialLeftPos * 2) / 3;
////	 int controlsButtonsTop = screenHeight - buttonHeight - spacing;
////	 // первая - кнопка назад
////	 backButton = screenButtons->addButton(initialLeftPos, controlsButtonsTop, controlsButtonsWidth, buttonHeight, WM_BACK_CAPTION);
////	 saveButton = screenButtons->addButton(initialLeftPos + spacing + controlsButtonsWidth, controlsButtonsTop, controlsButtonsWidth, buttonHeight, WM_SAVE_CAPTION);
////	 onOffButton = screenButtons->addButton(initialLeftPos + spacing * 2 + controlsButtonsWidth * 2, controlsButtonsTop, controlsButtonsWidth, buttonHeight, WM_ON_CAPTION);
////
////	 screenButtons->setButtonFontColor(onOffButton, CHANNELS_BUTTONS_TEXT_COLOR);
////	 screenButtons->disableButton(saveButton);
////
////
////	 static char leftArrowCaption[2] = { 0 };
////	 static char rightArrowCaption[2] = { 0 };
////
////	 leftArrowCaption[0] = rusPrinter->mapChar(charLeftArrow);
////	 rightArrowCaption[0] = rusPrinter->mapChar(charRightArrow);
////
////	 int textBoxHeightWithCaption = TFT_TEXT_INPUT_HEIGHT + textFontHeight + INFO_BOX_CONTENT_PADDING;
////	 int textBoxTopPos = topPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////
////	 // теперь добавляем наши кнопки
////	 dec25PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 percents25Box = new TFTInfoBox(WM_T25_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 inc25PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////	 leftPos += INFO_BOX_V_SPACING * 2 + TFT_ARROW_BUTTON_WIDTH;
////
////	 dec50PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 percents50Box = new TFTInfoBox(WM_T50_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 inc50PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////
////	 // вторая строка
////	 textBoxTopPos = secondRowTopPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////	 topPos = secondRowTopPos;
////	 leftPos = initialLeftPos;
////
////	 dec75PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 percents75Box = new TFTInfoBox(WM_T75_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 inc75PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////	 leftPos += INFO_BOX_V_SPACING * 2 + TFT_ARROW_BUTTON_WIDTH;
////
////	 dec100PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 percents100Box = new TFTInfoBox(WM_T100_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 inc100PercentsButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////	 // третья строка
////	 textBoxTopPos = thirdRowTopPos - textFontHeight - INFO_BOX_CONTENT_PADDING;
////	 topPos = thirdRowTopPos;
////	 leftPos = initialLeftPos;
////
////	 decHisteresisButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 histeresisBox = new TFTInfoBox(WM_HISTERESIS_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 incHisteresisButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////	 leftPos += INFO_BOX_V_SPACING * 2 + TFT_ARROW_BUTTON_WIDTH;
////
////	 decSensorButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, leftArrowCaption, BUTTON_SYMBOL);
////	 leftPos += INFO_BOX_V_SPACING + TFT_ARROW_BUTTON_WIDTH;
////
////	 sensorBox = new TFTInfoBox(WM_SENSOR_CAPTION, TFT_TEXT_INPUT_WIDTH, textBoxHeightWithCaption, leftPos, textBoxTopPos, -(TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING));
////	 sensorDataTop = textBoxTopPos;
////	 sensorDataLeft = leftPos + rusPrinter->textWidth(WM_SENSOR_CAPTION) + textFontWidth * 3 - (TFT_ARROW_BUTTON_WIDTH + INFO_BOX_V_SPACING);
////	 leftPos += INFO_BOX_V_SPACING + TFT_TEXT_INPUT_WIDTH;
////
////	 incSensorButton = screenButtons->addButton(leftPos, topPos, TFT_ARROW_BUTTON_WIDTH, TFT_ARROW_BUTTON_HEIGHT, rightArrowCaption, BUTTON_SYMBOL);
////
////	 //Serial.println("TFTWMScreen SETUP DONE!"); Serial.flush();
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::saveSettings()
//// {
////	 //GlobalSettings* settings = MainController->GetSettings();
////
////	 //settings->Set25PercentsOpenTemp(channel, percents25Temp);
////	 //settings->Set50PercentsOpenTemp(channel, percents50Temp);
////	 //settings->Set75PercentsOpenTemp(channel, percents75Temp);
////	 //settings->Set100PercentsOpenTemp(channel, percents100Temp);
////	 //settings->SetWMHisteresis(channel, histeresis);
////	 //settings->SetWMSensor(channel, sensor);
////	 //settings->SetWMActive(channel, isActive);
////
////	 ////УВЕДОМЛЯЕМ МОДУЛЬ КОНТРОЛЯ, ЧТО ЕМУ НЕОБХОДИМО ПЕРЕЗАГРУЗИТЬ НАСТРОЙКИ
////	 //LogicManageModule->ReloadWindowsSettings();
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::blinkSaveSettingsButton(bool bOn)
//// {
////
////	 if (bOn)
////	 {
////		 if (blinkActive != bOn)
////		 {
////			 blinkActive = bOn;
////			 blinkOn = true;
////			 blinkTimer = millis();
////			 screenButtons->setButtonFontColor(saveButton, WM_BLINK_ON_TEXT_COLOR);
////			 screenButtons->setButtonBackColor(saveButton, WM_ON_BLINK_BGCOLOR);
////			 screenButtons->drawButton(saveButton);
////		 }
////	 }
////	 else
////	 {
////		 blinkOn = false;
////		 blinkActive = false;
////		 screenButtons->setButtonFontColor(saveButton, WM_BLINK_OFF_TEXT_COLOR);
////		 screenButtons->setButtonBackColor(saveButton, WM_OFF_BLINK_BGCOLOR);
////		 screenButtons->drawButton(saveButton);
////	 }
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::onButtonPressed(TFTMenu* menuManager, int buttonID)
//// {
////	 tickerButton = -1;
////	 if (buttonID == dec25PercentsButton || buttonID == inc25PercentsButton || buttonID == dec50PercentsButton
////		 || buttonID == inc50PercentsButton || buttonID == dec75PercentsButton || buttonID == inc75PercentsButton
////		 || buttonID == dec100PercentsButton || buttonID == inc100PercentsButton)
////	 {
////		 tickerButton = buttonID;
////		 Ticker.start(this);
////	 }
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::onButtonReleased(TFTMenu* menuManager, int buttonID)
//// {
////	 Ticker.stop();
////	 tickerButton = -1;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::onTick()
//// {
////	 if (tickerButton == dec25PercentsButton)
////		 inc25Temp(-3);
////	 else
////		 if (tickerButton == inc25PercentsButton)
////			 inc25Temp(3);
////		 else
////			 if (tickerButton == dec50PercentsButton)
////				 inc50Temp(-3);
////			 else
////				 if (tickerButton == inc50PercentsButton)
////					 inc50Temp(3);
////				 else
////					 if (tickerButton == dec75PercentsButton)
////						 inc75Temp(-3);
////					 else
////						 if (tickerButton == inc75PercentsButton)
////							 inc75Temp(3);
////						 else
////							 if (tickerButton == dec100PercentsButton)
////								 inc100Temp(-3);
////							 else
////								 if (tickerButton == inc100PercentsButton)
////									 inc100Temp(3);
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::inc25Temp(int val)
//// {
////	 int16_t old = percents25Temp;
////
////	 percents25Temp += val;
////
////	 if (percents25Temp < 0)
////		 percents25Temp = 0;
////
////	 if (percents25Temp > 99)
////		 percents25Temp = 99;
////
////	 if (percents25Temp != old)
////		 drawValueInBox(percents25Box, percents25Temp);
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::inc50Temp(int val)
//// {
////	 int16_t old = percents50Temp;
////
////	 percents50Temp += val;
////
////	 if (percents50Temp < 0)
////		 percents50Temp = 0;
////
////	 if (percents50Temp > 99)
////		 percents50Temp = 99;
////
////	 if (percents50Temp != old)
////		 drawValueInBox(percents50Box, percents50Temp);
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::inc75Temp(int val)
//// {
////	 int16_t old = percents75Temp;
////
////	 percents75Temp += val;
////
////	 if (percents75Temp < 0)
////		 percents75Temp = 0;
////
////	 if (percents75Temp > 99)
////		 percents75Temp = 99;
////
////	 if (percents75Temp != old)
////		 drawValueInBox(percents75Box, percents75Temp);
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::inc100Temp(int val)
//// {
////	 int16_t old = percents100Temp;
////
////	 percents100Temp += val;
////
////	 if (percents100Temp < 0)
////		 percents100Temp = 0;
////
////	 if (percents100Temp > 99)
////		 percents100Temp = 99;
////
////	 if (percents100Temp != old)
////		 drawValueInBox(percents100Box, percents100Temp);
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::update(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////
////	 static uint32_t sensorUpdateTimer = millis();
////	 if (millis() - sensorUpdateTimer > 2000)
////	 {
////		 String old1 = sensorDataString;
////
////		 getSensorData();
////
////		 if (sensorDataString != old1)
////		 {
////			 drawSensorData(menuManager);
////		 }
////
////		 sensorUpdateTimer = millis();
////	 }
////
////	 if (blinkActive && screenButtons)
////	 {
////		 if (millis() - blinkTimer > 500)
////		 {
////			 blinkOn = !blinkOn;
////
////			 if (blinkOn)
////			 {
////				 screenButtons->setButtonFontColor(saveButton, WM_BLINK_ON_TEXT_COLOR);
////				 screenButtons->setButtonBackColor(saveButton, WM_ON_BLINK_BGCOLOR);
////				 screenButtons->drawButton(saveButton);
////			 }
////			 else
////			 {
////				 screenButtons->setButtonFontColor(saveButton, WM_BLINK_OFF_TEXT_COLOR);
////				 screenButtons->setButtonBackColor(saveButton, WM_OFF_BLINK_BGCOLOR);
////				 screenButtons->drawButton(saveButton);
////			 }
////
////			 blinkTimer = millis();
////		 }
////	 } // if(blinkActive)
////
////	 if (screenButtons)
////	 {
////		 int pressed_button = screenButtons->checkButtons(ButtonPressed, ButtonReleased);
////
////		 if (pressed_button != -1)
////		 {
////			 menuManager->resetIdleTimer();
////
////
////			 if (pressed_button == backButton)
////			 {
////				 menuManager->switchToScreen("WMS");
////				 return;
////			 }
////			 else
////				 if (pressed_button == saveButton)
////				 {
////					 saveSettings();
////					 blinkSaveSettingsButton(false);
////					 screenButtons->disableButton(saveButton, true);
////					 return;
////				 }
////				 else
////					 if (pressed_button == onOffButton)
////					 {
////
////						 if (isActive)
////							 isActive = false;
////						 else
////							 isActive = true;
////
////						 if (isActive)
////						 {
////							 screenButtons->setButtonBackColor(onOffButton, MODE_ON_COLOR);
////							 screenButtons->relabelButton(onOffButton, WM_ON_CAPTION, true);
////						 }
////						 else
////						 {
////							 screenButtons->setButtonBackColor(onOffButton, MODE_OFF_COLOR);
////							 screenButtons->relabelButton(onOffButton, WM_OFF_CAPTION, true);
////						 }
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == dec25PercentsButton)
////					 {
////						 inc25Temp(-1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == inc25PercentsButton)
////					 {
////						 inc25Temp(1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == dec50PercentsButton)
////					 {
////						 inc50Temp(-1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == inc50PercentsButton)
////					 {
////						 inc50Temp(1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == dec75PercentsButton)
////					 {
////						 inc75Temp(-1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == inc75PercentsButton)
////					 {
////						 inc75Temp(1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == dec100PercentsButton)
////					 {
////						 inc100Temp(-1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == inc100PercentsButton)
////					 {
////						 inc100Temp(1);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == decHisteresisButton)
////					 {
////						 histeresis -= 5;
////						 if (histeresis < 0)
////							 histeresis = 0;
////
////						 drawValueInBox(histeresisBox, formatHisteresis());
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == incHisteresisButton)
////					 {
////						 histeresis += 5;
////						 if (histeresis > 50)
////							 histeresis = 50;
////
////						 drawValueInBox(histeresisBox, formatHisteresis());
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////					 }
////					 else if (pressed_button == decSensorButton)
////					 {
////						 sensor--;
////						 if (sensor < 0)
////							 sensor = 0;
////
////						 drawValueInBox(sensorBox, sensor);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////
////						 getSensorData();
////						 drawSensorData(menuManager);
////					 }
////					 else if (pressed_button == incSensorButton)
////					 {
////						 sensor++;
////						 if (sensor >= (humiditySensorsCount + tempSensorsCount))
////							 sensor = (humiditySensorsCount + tempSensorsCount) - 1;
////
////						 if (sensor < 0)
////							 sensor = 0;
////
////						 drawValueInBox(sensorBox, sensor);
////						 screenButtons->enableButton(saveButton, !screenButtons->buttonEnabled(saveButton));
////						 blinkSaveSettingsButton(true);
////						 getSensorData();
////						 drawSensorData(menuManager);
////					 }
////
////		 } // if(pressed_button != -1)
////
////	 } // if(screenButtons)
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// void TFTWMScreen::draw(TFTMenu* menuManager)
//// {
////	 if (!menuManager->getDC())
////	 {
////		 return;
////	 }
////
////	 if (screenButtons)
////	 {
////		 screenButtons->drawButtons(drawButtonsYield);
////	 }
////
////	 percents25Box->draw(menuManager);
////	 drawValueInBox(percents25Box, percents25Temp);
////
////	 percents50Box->draw(menuManager);
////	 drawValueInBox(percents50Box, percents50Temp);
////
////	 percents75Box->draw(menuManager);
////	 drawValueInBox(percents75Box, percents75Temp);
////
////	 percents100Box->draw(menuManager);
////	 drawValueInBox(percents100Box, percents100Temp);
////
////	 histeresisBox->draw(menuManager);
////	 drawValueInBox(histeresisBox, formatHisteresis());
////
////	 sensorBox->draw(menuManager);
////	 drawValueInBox(sensorBox, sensor);
////
////	 drawSensorData(menuManager);
////
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// String TFTWMScreen::formatHisteresis()
//// {
////	 int16_t val = histeresis / 10;
////	 int16_t fract = histeresis % 10;
////
////	 String result;
////	 result += val;
////	 result += '.';
////	 result += fract;
////
////	 return result;
//// }
//// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//////// TFTWMenuScreen
////////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTWMenuScreen::TFTWMenuScreen()
////// {
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TFTWMenuScreen::~TFTWMenuScreen()
////// {
//////	 delete screenButtons;
//////
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// void TFTWMenuScreen::onActivate(TFTMenu* menuManager)
////// {
//////	 if (!menuManager->getDC())
//////	 {
//////		 return;
//////	 }
//////
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// void TFTWMenuScreen::setup(TFTMenu* menuManager)
////// {
//////
//////
//////	 TFT_Class* dc = menuManager->getDC();
//////
//////	 if (!dc)
//////	 {
//////		 return;
//////	 }
//////
//////
//////	 screenButtons = new TFT_Buttons_Rus(dc, menuManager->getTouch(), menuManager->getRusPrinter());
//////	 screenButtons->setTextFont(TFT_FONT);
//////	 screenButtons->setSymbolFont(SENSOR_FONT);
//////
//////	 screenButtons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);
//////
//////
//////	 int screenWidth = dc->width();
//////	 int screenHeight = dc->height();
//////
//////
//////	 // вычисляем ширину всего занятого пространства
//////	 int widthOccupied = TFT_TEXT_INPUT_WIDTH * 2 + TFT_ARROW_BUTTON_WIDTH * 4 + INFO_BOX_V_SPACING * 6;
//////
//////	 // теперь вычисляем левую границу для начала рисования
//////	 int leftPos = (screenWidth - widthOccupied) / 2;
//////
//////	 const int spacing = 10;
//////
//////	 int controlButtonHeight = TFT_ARROW_BUTTON_HEIGHT;
//////
//////	 int controlsButtonsWidth = (screenWidth - spacing * 2 - leftPos * 2) / 3;
//////	 int controlsButtonsTop = screenHeight - controlButtonHeight - spacing;
//////
//////	 int channelsButtonsWidth = (screenWidth - leftPos * 2 - spacing) / 2;
//////	 int channelsButtonsTop = 20;
//////	 int channelButtonHeight = TFT_ARROW_BUTTON_HEIGHT;
//////
//////	 // первая - кнопка назад
//////	 backButton = screenButtons->addButton(leftPos, controlsButtonsTop, controlsButtonsWidth, controlButtonHeight, WM_BACK_CAPTION);
//////
//////
//////	 // первая строка
//////	 temp1Button = screenButtons->addButton(leftPos, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_TSECTION_1_CAPTION);
//////	 screenButtons->setButtonBackColor(temp1Button, WM_SECTION_1_BGCOLOR);
//////	 screenButtons->setButtonFontColor(temp1Button, WM_SECTION_1_FONT_COLOR);
//////
//////	 windowsButton = screenButtons->addButton(leftPos + channelsButtonsWidth + spacing, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_WLIST_CAPTION);
//////
//////	 channelsButtonsTop += channelButtonHeight + spacing;
//////
//////	 // вторая строка
//////	 temp2Button = screenButtons->addButton(leftPos, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_TSECTION_2_CAPTION);
//////	 screenButtons->setButtonBackColor(temp2Button, WM_SECTION_2_BGCOLOR);
//////	 screenButtons->setButtonFontColor(temp2Button, WM_SECTION_2_FONT_COLOR);
//////
//////	 orientationButton = screenButtons->addButton(leftPos + channelsButtonsWidth + spacing, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_ORIENTATION_CAPTION);
//////
//////	 channelsButtonsTop += channelButtonHeight + spacing;
//////
//////	 // третья строка
//////	 temp3Button = screenButtons->addButton(leftPos, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_TSECTION_3_CAPTION);
//////	 screenButtons->setButtonBackColor(temp3Button, WM_SECTION_3_BGCOLOR);
//////	 screenButtons->setButtonFontColor(temp3Button, WM_SECTION_3_FONT_COLOR);
//////
//////	 rainButton = screenButtons->addButton(leftPos + channelsButtonsWidth + spacing, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_RAIN_CAPTION);
//////
//////	 channelsButtonsTop += channelButtonHeight + spacing;
//////
//////	 // четвертая строка
//////	 temp4Button = screenButtons->addButton(leftPos, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_TSECTION_4_CAPTION);
//////	 screenButtons->setButtonBackColor(temp4Button, WM_SECTION_4_BGCOLOR);
//////	 screenButtons->setButtonFontColor(temp4Button, WM_SECTION_4_FONT_COLOR);
//////
//////	 windButton = screenButtons->addButton(leftPos + channelsButtonsWidth + spacing, channelsButtonsTop, channelsButtonsWidth, channelButtonHeight, WM_WIND_CAPTION);
//////
//////	 channelsButtonsTop += channelButtonHeight + spacing;
//////
//////
//////	 //Serial.println("TFTWMenuScreen SETUP DONE!"); Serial.flush();
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// void TFTWMenuScreen::update(TFTMenu* menuManager)
////// {
//////
//////	 if (!menuManager->getDC())
//////	 {
//////		 return;
//////	 }
//////
//////	 if (screenButtons)
//////	 {
//////		 int pressed_button = screenButtons->checkButtons(ButtonPressed, ButtonReleased);
//////
//////		 if (pressed_button != -1)
//////		 {
//////			 menuManager->resetIdleTimer();
//////
//////			 if (pressed_button == backButton)
//////			 {
//////				 menuManager->switchToScreen("SENSORS");
//////				 return;
//////			 }
//////			 else
//////				 if (pressed_button == temp1Button)
//////				 {
//////					 menuManager->switchToScreen("WMS1");
//////					 return;
//////				 }
//////				 else
//////					 if (pressed_button == temp2Button)
//////					 {
//////						 menuManager->switchToScreen("WMS2");
//////						 return;
//////					 }
//////					 else
//////						 if (pressed_button == temp3Button)
//////						 {
//////							 menuManager->switchToScreen("WMS3");
//////							 return;
//////						 }
//////						 else
//////							 if (pressed_button == temp4Button)
//////							 {
//////								 menuManager->switchToScreen("WMS4");
//////								 return;
//////							 }
//////							 else
//////								 if (pressed_button == windowsButton)
//////								 {
//////									 menuManager->switchToScreen("WMW");
//////									 return;
//////								 }
//////								 else
//////									 if (pressed_button == rainButton)
//////									 {
//////										 menuManager->switchToScreen("RAIN");
//////										 return;
//////									 }
//////									 else
//////										 if (pressed_button == orientationButton)
//////										 {
//////											 menuManager->switchToScreen("ORIENT");
//////											 return;
//////										 }
//////										 else
//////											 if (pressed_button == windButton)
//////											 {
//////												 menuManager->switchToScreen("WIND");
//////												 return;
//////											 }
//////
//////
//////
//////		 } // if(pressed_button != -1)
//////	 } // if(screenButtons)
//////
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// void TFTWMenuScreen::draw(TFTMenu* menuManager)
////// {
//////	 if (!menuManager->getDC())
//////	 {
//////		 return;
//////	 }
//////
//////	 if (screenButtons)
//////	 {
//////		 screenButtons->drawButtons(drawButtonsYield);
//////	 }
//////
////// }
////// //------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//////
//////
//////
////
////
////
////
//////=======================================================================================================================================================================
////
////
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////KeyboardScreen* Keyboard;
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////KeyboardScreen::KeyboardScreen() : AbstractTFTScreen()
////{
////  inputTarget = NULL;
////  maxLen = 20;
////  isRusInput = true;
////  
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////  
////
////  buttons = new TFT_Buttons_Rus(TFTScreen->getDC(), TFTScreen->getTouch(),TFTScreen->getRusPrinter(),60);
////  
////  buttons->setTextFont(TFT_FONT);
////  buttons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);
////  //buttons->setSymbolFont(Various_Symbols_32x32);
////  buttons->setSymbolFont(VARIOUS_SYMBOLS_32x32);
////
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////KeyboardScreen::~KeyboardScreen()
////{
////  for(size_t i=0;i<captions.size();i++)
////  {
////    delete captions[i];
////  }
////  for(size_t i=0;i<altCaptions.size();i++)
////  {
////    delete altCaptions[i];
////  }
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::switchInput(bool redraw)
////{
////  isRusInput = !isRusInput;
////  Vector<String*>* pVec = isRusInput ? &captions : &altCaptions;
////
////  // у нас кнопки изменяемой клавиатуры начинаются с индекса 10
////  size_t startIdx = 10;
////
////  for(size_t i=startIdx;i<pVec->size();i++)
////  {
////    buttons->relabelButton(i,(*pVec)[i]->c_str(),redraw);
////  }
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::setup(TFTMenu* dc)
////{
////  if(!dc->getDC())
////  {
////    return;
////  }
////	
////  createKeyboard(dc);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::update(TFTMenu* menu)
////{
////  if(!menu->getDC())
////  {
////    return;
////  }
////	
////    // тут обновляем внутреннее состояние
////    // раз нас вызвали, то пока не нажмут кнопки - мы не выйдем, поэтому всегда сообщаем, что на экране что-то происходит
////    menu->resetIdleTimer();
////
////    // мигаем курсором
////    static uint32_t cursorTimer = millis();
////    if(millis() - cursorTimer > 500)
////    {
////      static bool cursorVisible = true;
////      cursorVisible = !cursorVisible;
////
////      redrawCursor(menu,cursorVisible);
////
////      cursorTimer = millis();
////    }
////    
////    // проверяем на перемещение курсора
////    TOUCH_Class* touch = menu->getTouch();
////    
////    if(touch->dataAvailable())
////    {
////       TS_Point p = touch->getPoint();
////      int   touch_x = p.x;
////      int   touch_y = p.y;
////
////      // проверяем на попадание в прямоугольную область ввода текста
////      TFT_Class* dc = menu->getDC();
////      dc->setFreeFont(TFT_FONT);
////      
////      int screenWidth = dc->width();
////      const int fontWidth = 8;
////      
////      if(touch_x >= KBD_SPACING && touch_x <= (screenWidth - KBD_SPACING) && touch_y >= KBD_SPACING && touch_y <= (KBD_SPACING + KBD_BUTTON_HEIGHT))
////      {
////        #ifdef USE_BUZZER
////          Buzzer.buzz();
////        #endif
////        // кликнули на области ввода, ждём отпускания тача
////        while (touch->dataAvailable()) { yield(); }
////        
////
////        // вычисляем, на какой символ приходится клик тачем
////        int symbolNum = touch_x/fontWidth - 1;
////        
////        if(symbolNum < 0)
////          symbolNum = 0;
////          
////        int valLen = menu->getRusPrinter()->getStringLength(inputVal.c_str());
////
////        if(symbolNum > valLen)
////          symbolNum = valLen;
////
////        redrawCursor(menu,true);
////        cursorPos = symbolNum;
////        redrawCursor(menu,false);
////      }
////    } // if (touch->dataAvailable())
////  
////    int pressed_button = buttons->checkButtons(ButtonPressed, ButtonReleased);
////    if(pressed_button != -1)
////    {
////      
////       if(pressed_button == backspaceButton)
////       {
////        // удалить последний введённый символ
////        drawValue(menu,true);
////       }
////       else
////       if(pressed_button == okButton)
////       {
////          // закрыть всё нафик
////          if(inputTarget)
////          {
////            inputTarget->onKeyboardInputResult(inputVal,true);
////            inputVal = "";
////          }
////       }
////        else
////       if(pressed_button == switchButton)
////       {
////          // переключить раскладку
////          switchInput(true);
////       }
////       else
////       if(pressed_button == cancelButton)
////       {
////          // закрыть всё нафик
////          if(inputTarget)
////          {
////            inputTarget->onKeyboardInputResult(inputVal,false);
////            inputVal = "";
////          }
////       }
////       else
////       {
////         // одна из кнопок клавиатуры, добавляем её текст к буферу, но - в позицию курсора!!!
////         int oldLen = menu->getRusPrinter()->getStringLength(inputVal.c_str());
////         const char* lbl = buttons->getLabel(pressed_button);
////         
////         if(!oldLen) // пустая строка
////         {
////          inputVal = lbl;
////         }
////         else
////         if(oldLen < maxLen)
////         {
////            
////            String buff;            
////            const char* ptr = inputVal.c_str();
////            
////            for(int i=0;i<oldLen;i++)
////            {
////              unsigned char curChar = (unsigned char) *ptr;
////              unsigned int charSz = utf8GetCharSize(curChar);
////              for(byte k=0;k<charSz;k++) 
////              {
////                utf8Bytes[k] = *ptr++;
////              }
////              utf8Bytes[charSz] = '\0'; // добавляем завершающий 0
////              
////              if(i == cursorPos)
////              {
////                buff += lbl;
////              }
////              
////              buff += utf8Bytes;
////              
////            } // for
////
////            if(cursorPos >= oldLen)
////              buff += lbl;
////
////          inputVal = buff;
////          
////         } // if(oldLen < maxLen)
////         
////
////          int newLen = menu->getRusPrinter()->getStringLength(inputVal.c_str());
////
////          if(newLen <= maxLen)
////          {
////            drawValue(menu);
////                     
////            if(newLen != oldLen)
////            {
////              redrawCursor(menu,true);
////              cursorPos++;
////              redrawCursor(menu,false);
////            }
////            
////          }
////          
////
////         
////       } // else одна из кнопок клавиатуры
////    
////    } // if(pressed_button != -1)
////    
////    
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::draw(TFTMenu* menu)
////{
////  if(!menu->getDC())
////  {
////    return;
////  }
////	
////
////  buttons->drawButtons(drawButtonsYield);
////
////  TFT_Class* dc = menu->getDC();
////  int screenWidth = dc->width();
////  dc->drawRoundRect(KBD_SPACING, KBD_SPACING, screenWidth-KBD_SPACING*2, KBD_BUTTON_HEIGHT,2, TFT_LIGHTGREY);
////
////  drawValue(menu);
////  redrawCursor(menu,false);
////}
//////--------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::redrawCursor(TFTMenu* menu, bool erase)
////{
////  TFT_Class* dc = menu->getDC();
////  TFTRus* rus = menu->getRusPrinter();
////
////  dc->setFreeFont(TFT_FONT);
////  uint8_t fontHeight = FONT_HEIGHT(dc);
////
////  int top = KBD_SPACING + (KBD_BUTTON_HEIGHT - fontHeight)/2;
////  
////  String tmp = inputVal.substring(0,cursorPos);
////  
////  int left = KBD_SPACING*2 + rus->textWidth(tmp.c_str());
////
////  uint16_t fgColor = TFT_BACK_COLOR;
////
////  if(erase)
////    fgColor = TFT_BACK_COLOR;
////  else
////    fgColor = TFT_FONT_COLOR;
////  
////  dc->fillRect(left,top,1,fontHeight,fgColor);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::drawValue(TFTMenu* menu, bool deleteCharAtCursor)
////{
////  if(!inputVal.length())
////    return;
////
////
////   TFT_Class* dc = menu->getDC();
////
////  if(deleteCharAtCursor)
////  {
////    // надо удалить символ слева от позиции курсора.
////
////    String buff;
////    int len = menu->getRusPrinter()->getStringLength(inputVal.c_str());
////    const char* ptr = inputVal.c_str();
////    
////    for(int i=0;i<len;i++)
////    {
////      unsigned char curChar = (unsigned char) *ptr;
////      unsigned int charSz = utf8GetCharSize(curChar);
////      for(byte k=0;k<charSz;k++) 
////      {
////        utf8Bytes[k] = *ptr++;
////      }
////      utf8Bytes[charSz] = '\0'; // добавляем завершающий 0
////      
////      if(i != (cursorPos-1)) // игнорируем удаляемый символ
////      {
////        buff += utf8Bytes;
////      }
////      
////    } // for
////    
////    buff += ' '; // маскируем последний символ для корректной перерисовки на экране
////    inputVal = buff;
////
////  }
////
////  dc->setFreeFont(TFT_FONT);
////  
////  uint8_t fontHeight = FONT_HEIGHT(dc);
////
////
////  int top = KBD_SPACING + (KBD_BUTTON_HEIGHT - fontHeight)/2;
////  int left = KBD_SPACING*2;
////
////  menu->getRusPrinter()->print(inputVal.c_str(),left,top,TFT_BACK_COLOR,TFT_FONT_COLOR);
////
////  if(deleteCharAtCursor)
////  {
////    // если надо удалить символ слева от позиции курсора, то в этом случае у нас последний символ - пробел, и мы его удаляем
////    inputVal.remove(inputVal.length()-1,1);
////
////    redrawCursor(menu,true);
////
////    cursorPos--;
////    if(cursorPos < 0)
////      cursorPos = 0;
////
////    redrawCursor(menu,false);
////  }
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::createKeyboard(TFTMenu* menu)
////{
////  buttons->deleteAllButtons();
////
////  TFT_Class* dc = menu->getDC();
////  int screenWidth = dc->width();
////  int screenHeight = dc->height();  
////
////  // создаём клавиатуру
////
////  int colCounter = 0;
////  int left = KBD_SPACING;
////  int top = KBD_SPACING*2 + KBD_BUTTON_HEIGHT;
////
////  // сперва у нас кнопки 0-9
////  for(uint8_t i=0;i<10;i++)
////  {
////    char c = '0' + i;
////    String* s = new String(c);
////    captions.push_back(s);
////
////    String* altS = new String(c);
////    altCaptions.push_back(altS);    
////
////    /*int addedBtn = */buttons->addButton(left, top, KBD_BUTTON_WIDTH, KBD_BUTTON_HEIGHT, s->c_str());
////   // buttons->setButtonBackColor(addedBtn, VGA_GRAY);
////   // buttons->setButtonFontColor(addedBtn, VGA_BLACK);
////    
////    left += KBD_BUTTON_WIDTH + KBD_SPACING;
////    colCounter++;
////    if(colCounter >= KBD_BUTTONS_IN_ROW)
////    {
////      colCounter = 0;
////      left = KBD_SPACING;
////      top += KBD_SPACING + KBD_BUTTON_HEIGHT;
////    }
////  }
////  // затем - А-Я
////  const char* letters[] = {
////    "А", "Б", "В", "Г", "Д", "Е",
////    "Ж", "З", "И", "Й", "К", "Л",
////    "М", "Н", "О", "П", "Р", "С",
////    "Т", "У", "Ф", "Х", "Ц", "Ч",
////    "Ш", "Щ", "Ъ", "Ы", "Ь", "Э",
////    "Ю", "Я", NULL
////  };
////
////  const char* altLetters[] = {
////    "A", "B", "C", "D", "E", "F",
////    "G", "H", "I", "J", "K", "L",
////    "M", "N", "O", "P", "Q", "R",
////    "S", "T", "U", "V", "W", "X",
////    "Y", "Z", ".", ",", ":", ";",
////    "!", "?", NULL
////  };  
////
////  int lettersIterator = 0;
////  while(letters[lettersIterator])
////  {
////    String* s = new String(letters[lettersIterator]);
////    captions.push_back(s);
////
////    String* altS = new String(altLetters[lettersIterator]);
////    altCaptions.push_back(altS);
////
////    buttons->addButton(left, top, KBD_BUTTON_WIDTH, KBD_BUTTON_HEIGHT, s->c_str());
////    left += KBD_BUTTON_WIDTH + KBD_SPACING;
////    colCounter++;
////    if(colCounter >= KBD_BUTTONS_IN_ROW)
////    {
////      colCounter = 0;
////      left = KBD_SPACING;
////      top += KBD_SPACING + KBD_BUTTON_HEIGHT;
////    } 
////
////    lettersIterator++;
////  }
////  // затем - кнопка переключения ввода
////    switchButton = buttons->addButton(left, top, KBD_BUTTON_WIDTH, KBD_BUTTON_HEIGHT, "q", BUTTON_SYMBOL);
////    buttons->setButtonBackColor(switchButton, TFT_MAROON);
////    buttons->setButtonFontColor(switchButton, TFT_WHITE);
////
////    left += KBD_BUTTON_WIDTH + KBD_SPACING;
////  
////  // затем - пробел,
////    spaceButton = buttons->addButton(left, top, KBD_BUTTON_WIDTH*5 + KBD_SPACING*4, KBD_BUTTON_HEIGHT, " ");
////    //buttons->setButtonBackColor(spaceButton, VGA_GRAY);
////    //buttons->setButtonFontColor(spaceButton, VGA_BLACK);
////       
////    left += KBD_BUTTON_WIDTH*5 + KBD_SPACING*5;
////   
////  // backspace, 
////    backspaceButton = buttons->addButton(left, top, KBD_BUTTON_WIDTH*2 + KBD_SPACING, KBD_BUTTON_HEIGHT, ":", BUTTON_SYMBOL);
////    buttons->setButtonBackColor(backspaceButton, TFT_MAROON);
////    buttons->setButtonFontColor(backspaceButton, TFT_WHITE);
////
////    left = KBD_SPACING;
////    top = screenHeight - KDB_BIG_BUTTON_HEIGHT - KBD_SPACING;
////   
////  // OK,
////    int okCancelButtonWidth = (screenWidth - KBD_SPACING*3)/2;
////    okButton = buttons->addButton(left, top, okCancelButtonWidth, KDB_BIG_BUTTON_HEIGHT, "OK");
////    left += okCancelButtonWidth + KBD_SPACING;
////  
////  // CANCEL
////    cancelButton = buttons->addButton(left, top, okCancelButtonWidth, KDB_BIG_BUTTON_HEIGHT, "ОТМЕНА");
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::applyType(KeyboardType keyboardType)
////{
////  if(ktFull == keyboardType)
////  {
////    buttons->enableButton(spaceButton);
////    buttons->enableButton(switchButton);
////
////    // включаем все кнопки
////    // у нас кнопки изменяемой клавиатуры начинаются с индекса 10
////    size_t startIdx = 10;
////  
////    for(size_t i=startIdx;i<altCaptions.size();i++)
////    {
////      buttons->enableButton(i);
////    }    
////
////    isRusInput = false;
////    switchInput(false);
////
////    return;
////  }
////
////  if(ktNumbers == keyboardType)
////  {
////    buttons->disableButton(spaceButton);
////    buttons->disableButton(switchButton);
////
////    // выключаем все кнопки, кроме номеров и точки
////    // у нас кнопки изменяемой клавиатуры начинаются с индекса 10
////    size_t startIdx = 10;
////  
////    for(size_t i=startIdx;i<altCaptions.size();i++)
////    {
////      if(*(altCaptions[i]) != ".")
////        buttons->disableButton(i);
////    }        
////
////    isRusInput = true;
////    switchInput(false);
////
////    return;
////  }
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void KeyboardScreen::show(const String& val, int ml, KeyboardInputTarget* it, KeyboardType keyboardType, bool eng)
////{
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////	
////  inputVal = val;
////  inputTarget = it;
////  maxLen = ml;
////
////  cursorPos = TFTScreen->getRusPrinter()->getStringLength(inputVal.c_str());
////
////  applyType(keyboardType);
////
////  if(eng && isRusInput)
////    switchInput(false);
////
////  TFTScreen->switchToScreen(this);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////AbstractTFTScreen* KeyboardScreen::create()
////{
////    Keyboard = new KeyboardScreen();
////    return Keyboard;  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////MessageBoxScreen* MessageBox;
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////MessageBoxScreen::MessageBoxScreen() : AbstractTFTScreen()
////{
////  targetOkScreen = NULL;
////  targetCancelScreen = NULL;
////  resultSubscriber = NULL;
////  caption = NULL;
////  
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////  
////
////  buttons = new TFT_Buttons_Rus(TFTScreen->getDC(), TFTScreen->getTouch(),TFTScreen->getRusPrinter());
////  
////  buttons->setTextFont(TFT_FONT);
////  buttons->setButtonColors(TFT_CHANNELS_BUTTON_COLORS);
////   
////  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::setup(TFTMenu* dc)
////{
////  if(!dc->getDC())
////  {
////    return;
////  }
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::update(TFTMenu* dc)
////{
////  if(!dc->getDC())
////  {
////    return;
////  }
////	
////    // тут обновляем внутреннее состояние    
//// 
////    int pressed_button = buttons->checkButtons(ButtonPressed, ButtonReleased);
////    if(pressed_button != -1)
////    {
////      // сообщаем, что у нас нажата кнопка
////      dc->resetIdleTimer();
////      
////       if(pressed_button == noButton && targetCancelScreen)
////       {
////        if(resultSubscriber)
////          resultSubscriber->onMessageBoxResult(false);
////          
////        dc->switchToScreen(targetCancelScreen);
////       }
////       else
////       if(pressed_button == yesButton && targetOkScreen)
////       {
////          if(resultSubscriber)
////            resultSubscriber->onMessageBoxResult(true);
////            
////            dc->switchToScreen(targetOkScreen);
////       }
////    
////    } // if(pressed_button != -1)
////
////    
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::draw(TFTMenu* hal)
////{
////  TFT_Class* dc = hal->getDC();
////  
////  if(!dc)
////  {
////    return;
////  }
////  
////  dc->setFreeFont(TFT_FONT);
////  TFTRus* rusPrinter = hal->getRusPrinter();
////  
////  uint8_t fontHeight = FONT_HEIGHT(dc);
////  
////  int displayWidth = dc->width();
////  int displayHeight = dc->height();
////  
////  int lineSpacing = 6; 
////  int topOffset = 10;
////  int curX = 0;
////  int curY = topOffset;
////
////  int lineLength = 0;
////
////  uint16_t fgColor = TFT_NAVY, bgColor = TFT_WHITE;
////  
////  // подложка под заголовок
////  if(boxType == mbHalt && errorColors)
////  {
////    fgColor = TFT_RED;
////  }
////  else
////  {
////    fgColor = TFT_NAVY;
////  }
////    
////  dc->fillRect(0, 0, displayWidth, topOffset + fontHeight+4,fgColor);
////  
////  if(caption)
////  {
////    if(boxType == mbHalt && errorColors)
////    {
////      bgColor = TFT_RED;
////      fgColor = TFT_WHITE;
////    }
////    else
////    {
////      bgColor = TFT_NAVY;
////      fgColor = TFT_WHITE;      
////    }
////    lineLength = rusPrinter->textWidth(caption);
////    curX = (displayWidth - lineLength)/2; 
////    rusPrinter->print(caption,curX,curY,bgColor,fgColor);
////  }
////
////  curY = (displayHeight - ALL_CHANNELS_BUTTON_HEIGHT - (lines.size()*fontHeight + (lines.size()-1)*lineSpacing))/2;
////
////  for(size_t i=0;i<lines.size();i++)
////  {
////    lineLength = rusPrinter->textWidth(lines[i]);
////    curX = (displayWidth - lineLength)/2;    
////    rusPrinter->print(lines[i],curX,curY,TFT_BACK_COLOR,TFT_FONT_COLOR);
////    curY += fontHeight + lineSpacing;
////  }
////
////  buttons->drawButtons(drawButtonsYield);
////
////  if(boxType == mbHalt && haltInWhile)
////  {
////    while(1)
////    {
////      #ifdef USE_EXTERNAL_WATCHDOG
////        updateExternalWatchdog();
////      #endif      
////    }
////  }
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::recreateButtons()
////{
////  buttons->deleteAllButtons();
////  yesButton = -1;
////  noButton = -1;
////  
////  TFT_Class* dc = TFTScreen->getDC();
////  
////  int screenWidth = dc->width();
////  int screenHeight = dc->height();
////  int buttonsWidth = 200;
////
////  int numOfButtons = boxType == mbShow ? 1 : 2;
////
////  int top = screenHeight - ALL_CHANNELS_BUTTON_HEIGHT - INFO_BOX_V_SPACING;
////  int left = (screenWidth - (buttonsWidth*numOfButtons + INFO_BOX_V_SPACING*(numOfButtons-1)))/2;
////  
////  yesButton = buttons->addButton(left, top, buttonsWidth, ALL_CHANNELS_BUTTON_HEIGHT, boxType == mbShow ? "OK" : "ДА");
////
////  if(boxType == mbConfirm)
////  {
////    left += buttonsWidth + INFO_BOX_V_SPACING;
////    noButton = buttons->addButton(left, top, buttonsWidth, ALL_CHANNELS_BUTTON_HEIGHT, "НЕТ");  
////  }
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::halt(const char* _caption, Vector<const char*>& _lines, bool _errorColors, bool _haltInWhile)
////{
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////	
////  lines = _lines;
////  caption = _caption;
////  boxType = mbHalt;
////  errorColors = _errorColors;
////  haltInWhile = _haltInWhile;
////
////  buttons->deleteAllButtons();
////  yesButton = -1;
////  noButton = -1;
////    
////  targetOkScreen = NULL;
////  targetCancelScreen = NULL;
////  resultSubscriber = NULL;  
////
////  TFTScreen->switchToScreen(this);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::show(const char* _caption, Vector<const char*>& _lines, AbstractTFTScreen* okTarget, MessageBoxResultSubscriber* sub)
////{
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////	
////  lines = _lines;
////  caption = _caption;
////  errorColors = false;
////
////  boxType = mbShow;
////  recreateButtons();
////    
////  targetOkScreen = okTarget;
////  targetCancelScreen = NULL;
////  resultSubscriber = sub;
////
////  TFTScreen->switchToScreen(this);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void MessageBoxScreen::confirm(const char* _caption, Vector<const char*>& _lines, AbstractTFTScreen* okTarget, AbstractTFTScreen* cancelTarget, MessageBoxResultSubscriber* sub)
////{
////  if(!TFTScreen->getDC())
////  {
////    return;
////  }
////	
////  lines = _lines;
////  caption = _caption;
////  errorColors = false;
////
////  boxType = mbConfirm;
////  recreateButtons();
////  
////  targetOkScreen = okTarget;
////  targetCancelScreen = cancelTarget;
////  resultSubscriber = sub;
////
////  TFTScreen->switchToScreen(this);
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////AbstractTFTScreen* MessageBoxScreen::create()
////{
////    MessageBox = new MessageBoxScreen();
////    return MessageBox;  
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////// TickerClass
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TickerClass Ticker;
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TickerClass::TickerClass()
////{
////  started = false;
////  beforeStartTickInterval = 1000;
////  tickInterval = 100;
////  waitBefore = true;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////TickerClass::~TickerClass()
////{
////  stop();
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TickerClass::setIntervals(uint16_t _beforeStartTickInterval,uint16_t _tickInterval)
////{
////  beforeStartTickInterval = _beforeStartTickInterval;
////  tickInterval = _tickInterval;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TickerClass::start(ITickHandler* h)
////{
////  if(started)
////    return;
////
////  handler = h;
////
////  timer = millis();
////  waitBefore = true;
////  started = true;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TickerClass::stop()
////{
////  if(!started)
////    return;
////
////  handler = NULL;
////
////  started = false;
////  waitBefore = true;
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////void TickerClass::tick()
////{
////  if(!started)
////    return;
////
////  uint32_t now = millis();
////
////  if(waitBefore)
////  {
////    if(now - timer > beforeStartTickInterval)
////    {
////      waitBefore = false;
////      timer = now;
////      if(handler)
////        handler->onTick();
////    }
////    return;
////  }
////
////  if(now - timer > tickInterval)
////  {
////    timer = now;
////    if(handler)
////      handler->onTick();
////  }
////
////}
//////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////
////
////
////
////
////
////
////
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif // USE_TFT_MODULE

