#include "CONFIG.h"
#include "UTFTMenu.h"
#include "Buzzer.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const uint8_t utf8_rus_charmap[] PROGMEM = {'A',128,'B',129,130,'E',131,132,133,134,135,'K',136,'M','H','O',137,'P','C','T',138,139,'X',140,141,
142,143,144,145,146,147,148,149,'a',150,151,152,153,'e',154,155,156,157,158,159,160,161,162,'o',163,'p','c',164,'y',165,'x',166,167,168,169,170,
171,172,173,174,175};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void buttonPressed(int btn) // вызывается по нажатию на кнопку - тут можно пищать баззером, например)
{
    if(btn != -1)
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
#endif // DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int rusPrintFunc(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
  return Screen.print(st,x,y,deg,computeStringLengthOnly);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen::AbstractTFTScreen(const char* name)
{
  screenName = name;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen::~AbstractTFTScreen()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    delete screenButtons;
  #endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::setup(TFTMenu* menu)
{
#if DISPLAY_USED == DISPLAY_ILI9341  
  screenButtons = new UTFT_Buttons_Rus(menu->getDC(), menu->getTouch(), rusPrintFunc);
  screenButtons->setTextFont(SCREEN_SMALL_FONT);
  screenButtons->setSymbolFont(SCREEN_SYMBOL_FONT);
  screenButtons->setButtonColors(SCREEN_BUTTON_COLORS);
#endif  

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
  if(isActive())
  {
    #if DISPLAY_USED == DISPLAY_ILI9341 
      int pressedButton = screenButtons->checkButtons(buttonPressed);
      
      if(pressedButton != -1)
      {
        notifyButtonPressed(pressedButton);
        //menu->notifyAction(this);
        //onButtonPressed(menu, pressedButton);
      }
    #endif

    if(isActive())
      doUpdate(menu);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AbstractTFTScreen::draw(TFTMenu* menu)
{
  if(isActive())
  {
    doDraw(menu);
    
    if(isActive())
    {
      #if DISPLAY_USED == DISPLAY_ILI9341 
        screenButtons->drawButtons(buttonDrawed); 
      #endif
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
  curFont = NULL;

  #if DISPLAY_USED == DISPLAY_ILI9341
    halTouch = NULL;
  #endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::notifyAction(AbstractTFTScreen* screen)
{
  if(on_action)
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
       halDCDescriptor->InitLCD(SCREEN_ORIENTATION);
    
      #if DISPLAY_INIT_DELAY > 0
        delay(DISPLAY_INIT_DELAY);
      #endif
      
      setBackColor(SCREEN_BACK_COLOR);
      setFont(SCREEN_SMALL_FONT);
    
      halTouch->InitTouch(SCREEN_ORIENTATION);
      halTouch->setPrecision(TOUCH_PRECISION);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::setup()
{
  //создание библиотеки для экрана
  
    halDCDescriptor = new UTFT(TFT_MODEL,TFT_RS_PIN,TFT_WR_PIN,TFT_CS_PIN,TFT_RST_PIN,TFT_DC_PIN);
    halTouch = new URTouch(TFT_TOUCH_CLK_PIN,TFT_TOUCH_CS_PIN,TFT_TOUCH_DIN_PIN,TFT_TOUCH_DOUT_PIN,TFT_TOUCH_IRQ_PIN);

   
  // инициализируем дисплей
  initHAL();

  // добавляем экран мессадж-бокса
  addScreen(MessageBoxScreen::create());

  // добавляем экран экранной клавиатуры
  addScreen(KeyboardScreen::create());
  
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::update()
{

  if(requestedToActiveScreen != NULL)
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

  if(currentScreenIndex == -1) // ни разу не рисовали ещё ничего, исправляемся
  {
    if(screens.size())
     switchToScreen((unsigned int)0); // переключаемся на первый экран, если ещё ни разу не показали ничего     
  }

  if(currentScreenIndex == -1)
    return;

  // обновляем текущий экран
  AbstractTFTScreen* currentScreen = screens[currentScreenIndex];
  currentScreen->update(this);
  
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen* TFTMenu::getActiveScreen()
{
  if(currentScreenIndex < 0 || !screens.size())
    return NULL;

  return screens[currentScreenIndex];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::switchToScreen(AbstractTFTScreen* screen)
{

  if(requestedToActiveScreen != NULL) // ждём переключения на новый экран
    return;
  
  if(currentScreenIndex > -1 && screens.size())
  {
     AbstractTFTScreen* si = screens[currentScreenIndex];
     si->setActive(false);
     si->onDeactivate();
  }
  
  for(size_t i=0;i<screens.size();i++)
  {
    if(screens[i] == screen)
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
  if(screenIndex < screens.size())
      switchToScreen(screens[screenIndex]);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::switchToScreen(const char* screenName)
{
  
  // переключаемся на запрошенный экран
  for(size_t i=0;i<screens.size();i++)
  {
    AbstractTFTScreen* si = screens[i];
    if(!strcmp(si->getName(),screenName))
    {
      switchToScreen(si);
      break;
    }
  }

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
String TFTMenu::utf8rus(const char* source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = strlen(source); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
  
}
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::setFont(FONT_TYPE* font)
{
  curFont = font;
  
  #if DISPLAY_USED == DISPLAY_ILI9341
   halDCDescriptor->setFont(font);
   #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FONT_TYPE* TFTMenu::getFont()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return halDCDescriptor->getFont();
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::clearScreen()
{
    // halDCDescriptor->fillScr(SCREEN_BACK_COLOR);
	 halDCDescriptor->clrScr();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::fillScreen(COLORTYPE color)
{
    halDCDescriptor->fillScr(color);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::setBackColor(COLORTYPE color)
{
   halDCDescriptor->setBackColor(color);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
COLORTYPE  TFTMenu::getBackColor()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return halDCDescriptor->getBackColor();
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::setColor(COLORTYPE color)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->setColor(color);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
COLORTYPE  TFTMenu::getColor()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
  return halDCDescriptor->getColor();
  #else
    #error "Unsupported display!"
  #endif  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  TFTMenu::drawRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->drawRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  TFTMenu::drawRoundRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->drawRoundRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  TFTMenu::fillRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->fillRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  TFTMenu::fillRoundRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    halDCDescriptor->fillRoundRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  TFTMenu::clrRoundRect(int x1, int y1, int x2, int y2)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    //halDCDescriptor->fillRoundRect(x1,y1,x2,y2);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TFTMenu::updateDisplay()
{
 /* #if DISPLAY_USED == DISPLAY_NOKIA5110
    halDCDescriptor->update();
  #endif*/
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t TFTMenu::getFontWidth(FONT_TYPE* font)
{
  #if DISPLAY_USED == DISPLAY_ILI9341    
    return READ_FONT_BYTE(0);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t TFTMenu::getFontHeight(FONT_TYPE* font)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return READ_FONT_BYTE(1); 
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t TFTMenu::getScreenWidth()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return halDCDescriptor->getDisplayXSize(); 
  #else
    #error "Unsupported display!"
  #endif     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint16_t TFTMenu::getScreenHeight()
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return halDCDescriptor->getDisplayYSize(); 
  #else
    #error "Unsupported display!"
  #endif     
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
int TFTMenu::print(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
  #if DISPLAY_USED == DISPLAY_ILI9341
    return printILI(st,x,y,deg,computeStringLengthOnly);
  #else
    #error "Unsupported display!"
  #endif    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int TFTMenu::printILI(const char* st,int x, int y, int deg, bool computeStringLengthOnly)
{
    
 int stl, i;
  stl = strlen(st);

  if (halDCDescriptor->orient==PORTRAIT)
  {
    if (x==RIGHT) 
      x=(halDCDescriptor->disp_x_size+1)-(stl*halDCDescriptor->cfont.x_size);
  
    if (x==CENTER) 
      x=((halDCDescriptor->disp_x_size+1)-(stl*halDCDescriptor->cfont.x_size))/2;
  } 
  else 
  {
    if (x==RIGHT) 
      x=(halDCDescriptor->disp_y_size+1)-(stl*halDCDescriptor->cfont.x_size);
    
    if (x==CENTER) 
      x=((halDCDescriptor->disp_y_size+1)-(stl*halDCDescriptor->cfont.x_size))/2;
  }
  
  uint8_t utf_high_byte = 0;
  uint8_t ch, ch_pos = 0;
  
  for (i = 0; i < stl; i++) 
  {
    ch = st[i];
    
    if ( ch >= 128) 
    {
      if ( utf_high_byte == 0 && (ch ==0xD0 || ch == 0xD1)) 
      {
        utf_high_byte = ch;
        continue;
      } 
      else 
      {
        if ( utf_high_byte == 0xD0) 
        {
          if (ch == 0x81) 
          { //Ё
            ch = 6;
          } 
          else 
          {
            if(ch <= 0x95) 
            {
              ch -= 0x90;
            } 
            else if( ch < 0xB6)
            {
              ch -= (0x90 - 1);
            } 
            else 
            {
              ch -= (0x90 - 2);
            }
          }
          
          ch = pgm_read_byte((utf8_rus_charmap + ch));
        
        } 
        else if (utf_high_byte == 0xD1) 
        {
          if (ch == 0x91) 
          {//ё
            ch = 39;
          } 
          else 
          {
            ch -= 0x80;
            ch += 50;
          }
          
          ch = pgm_read_byte((utf8_rus_charmap + ch));
        }
        
        utf_high_byte = 0;
      }
    } 
    else 
    {
      utf_high_byte = 0;
    }
    

    if (deg==0) 
    {
      if(!computeStringLengthOnly)
        halDCDescriptor->printChar(ch, x + (ch_pos * (halDCDescriptor->cfont.x_size)), y);
    } 
    else 
    {
      if(!computeStringLengthOnly)
        halDCDescriptor->rotateChar(ch, x, y, ch_pos, deg);
    }
    ++ch_pos;
  } // for  

  return ch_pos;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // DISPLAY_USED == DISPLAY_ILI9341
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TFTMenu Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageBoxScreen* MessageBox;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageBoxScreen::MessageBoxScreen() : AbstractTFTScreen("MessageBox")
{
  targetOkScreen = NULL;
  targetCancelScreen = NULL; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::doSetup(TFTMenu* dc)
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::doUpdate(TFTMenu* dc)
{
    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::doDraw(TFTMenu* menu)
{
  menu->setFont(SCREEN_SMALL_FONT);
  
  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = menu->getFontWidth(SCREEN_SMALL_FONT);
  int displayWidth = menu->getScreenWidth();
  int lineSpacing = 2; 
  int curX = 0;
  int curY = 0;
  
  for(size_t i=0;i<lines.size();i++)
  {
    int lineLength = menu->print(lines[i],curX,curY,0,true);
    curX = (displayWidth - lineLength*fontWidth)/2;    
    menu->print(lines[i],curX,curY);
    curY += fontHeight + lineSpacing;
  }

  menu->updateDisplay();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  switch(boxType)
  {
    case mbShow: // нажатие на любую кнопку ведёт к выходу из окна
      if(targetOkScreen)
        menu->switchToScreen(targetOkScreen);
    break; // mbShow

    case mbConfirm: // нажатие на кнопку 1 - ОК, нажатие на кнопку 2 - отмена
    {
      switch(pressedButton)
      {
        case BUTTON_1:
          if(targetOkScreen)
            menu->switchToScreen(targetOkScreen);
        break;

        case BUTTON_2:
          if(targetCancelScreen)
            menu->switchToScreen(targetCancelScreen);
        break;
        
      } // switch
    }
    break; // mbConfirm
    
  } // switch

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::show(Vector<const char*>& _lines, const char* okTarget)
{
  lines = _lines;
  boxType = mbShow;
  targetOkScreen = okTarget;
  targetCancelScreen = NULL;

  Screen.switchToScreen(this);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageBoxScreen::confirm(Vector<const char*>& _lines, const char* okTarget, const char* cancelTarget)
{
  lines = _lines;
  boxType = mbConfirm;
  targetOkScreen = okTarget;
  targetCancelScreen = cancelTarget;

  Screen.switchToScreen(this);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen* MessageBoxScreen::create()
{
    MessageBox = new MessageBoxScreen();
    return MessageBox;  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// KeyboardScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
KeyboardScreen* ScreenKeyboard;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AbstractTFTScreen* KeyboardScreen::create()
{
	ScreenKeyboard = new KeyboardScreen();
	return ScreenKeyboard;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
KeyboardScreen::KeyboardScreen() : AbstractTFTScreen("KBD")
{
	targetInput = NULL;
	targetScreen = NULL;

	// максимум 8 символов в HEX-представлении
	input_maxlength = 8;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::doSetup(TFTMenu* menu)
{
	const char* capts[] =
	{
	  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
	};

	const uint8_t buttons_in_row = 4;
	const uint8_t total_rows = 4;

	const uint8_t btn_width = 50;
	const uint8_t btn_height = 25;

	int curTop = 60;
	int btnCntr = 0;

	int curLeft;

	for (uint8_t i = 0; i < total_rows; i++)
	{
		curLeft = 4;
		for (uint8_t j = 0; j < buttons_in_row; j++)
		{
			screenButtons->addButton(curLeft, curTop, btn_width, btn_height, capts[btnCntr]);
			btnCntr++;
			curLeft += btn_width + 2;
		} // for

		curTop += btn_height + 2;
	} // for

	curLeft = 4;
	curTop += 6;

	const uint8_t controlButtonWidth = 66;
	const uint8_t controlButtonHeight = 30;

	clearButton = screenButtons->addButton(curLeft, curTop, controlButtonWidth, controlButtonHeight, "DEL");
	curLeft += controlButtonWidth + 4;

	exitButton = screenButtons->addButton(curLeft, curTop, controlButtonWidth, controlButtonHeight, "ВЫХ");
	curLeft += controlButtonWidth + 4;

	enterButton = screenButtons->addButton(curLeft, curTop, controlButtonWidth, controlButtonHeight, "ВВОД");

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::doUpdate(TFTMenu* menu)
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::doDraw(TFTMenu* menu)
{
	// сначала рисуем бокс для поля ввода

	UTFT* dc = menu->getDC();
	int screenW = dc->getDisplayXSize();
	int boxX = 4;
	int boxY = 32;
	int boxX2 = screenW - 8;
	int boxY2 = boxY + 20;

	dc->setColor(VGA_WHITE);
	dc->drawRoundRect(boxX, boxY, boxX2, boxY2);

	drawValue(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	if (pressedButton < 16)
	{
		// нажата одна из кнопок ввода значений
		input += screenButtons->getLabel(pressedButton);

		//Тут проверяем на максимальную длину    
		if (input.length() > input_maxlength)
			input.remove(input_maxlength);

		drawValue(menu);
	}
	else
	{
		// нажата одна из управляющих кнопок
		if (pressedButton == exitButton)
		{
			if (targetInput)
				targetInput->onKeyboardInput(false, "");

			Screen.switchToScreen(targetScreen);
		}
		else
			if (pressedButton == clearButton)
			{
				input = "";
				drawValue(menu);
			}
			else
				if (pressedButton == enterButton)
				{
					if (targetInput)
						targetInput->onKeyboardInput(true, input);

					Screen.switchToScreen(targetScreen);
				}
	} // else
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::show(KeyboardType type, const String& initialValue, AbstractTFTScreen* _targetScreen, KeyboardInputTarget* _targetInput, int maxLength)
{
	targetScreen = _targetScreen;
	targetInput = _targetInput;

	input = initialValue;

	if (maxLength > 0)
		input_maxlength = maxLength;
	else
		input_maxlength = 8;

	//Тут проверяем на максимальную длину 
	if (input.length() > input_maxlength)
		input.remove(input_maxlength);


	//Тут убираем некоторые кнопки, если тип != ktHex
	switch (type)
	{
	case ktDigits:
	{
		for (int i = 10; i < 16; i++)
			screenButtons->hideButton(i);
	}
	break; // ktDigits

	case ktHex:
	{
		for (int i = 10; i < 16; i++)
			screenButtons->showButton(i);
	}
	break; // ktHex

	} // switch

	Screen.switchToScreen(this);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void KeyboardScreen::drawValue(TFTMenu* menu)
{
	//тут рисуем то, что введено
	int textX = 6;
	int textY = 34;

	UTFT* dc = menu->getDC();

	uint8_t* oldFont = dc->getFont();
	dc->setFont(BigRusFont);
	dc->setColor(VGA_WHITE);

	int fontWidth = dc->getFontXsize();

	int strLen = menu->print(input.c_str(), 0, 0, 0, true);
	menu->print(input.c_str(), textX, textY);

	textX += strLen * fontWidth;

	// забиваем пробелами оставшуюся часть
	for (size_t i = strLen; i < input_maxlength; i++)
	{
		menu->print(" ", textX, textY);
		textX += fontWidth;
	}


	dc->setFont(oldFont);
}