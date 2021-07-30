#ifndef _UTFTMENU_H
#define _UTFTMENU_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "CONFIG.h"
#include "TinyVector.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenu;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341
#include <UTFT.h>
#include <URTouchCD_ILI9341.h>
#include <URTouch_ILI9341.h>
#include "UTFT_Buttons_Rus.h"
#else
  #error "Unsupported display!"
#endif  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if DISPLAY_USED == DISPLAY_ILI9341

  #if defined (__arm__)
    #define READ_FONT_BYTE(x) font[x]  
  #elif defined(__AVR__)  
    #define READ_FONT_BYTE(x) pgm_read_byte(&(font[x]))  
  #endif
#else
  #error "Unsupported display!"
#endif  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  typedef UTFT HalDCDescriptor;
  typedef uint16_t COLORTYPE;
  typedef uint8_t FONT_TYPE;
  extern FONT_TYPE BigRusFont[];                    // какой шрифт используем
  extern FONT_TYPE SmallRusFont[];                  // какой шрифт используем
  extern FONT_TYPE Various_Symbols_16x32[];         // какой шрифт используем
  extern FONT_TYPE Various_Symbols_32x32[];         // какой шрифт используем
  
  #define SCREEN_BACK_COLOR VGA_BLACK               // цвет фона
  #define SCREEN_TEXT_COLOR VGA_WHITE               // цвет шрифта
  #define SCREEN_BIG_FONT BigRusFont                // большой шрифт
  #define SCREEN_SMALL_FONT SmallRusFont            // маленький шрифт
  #define SCREEN_SYMBOL_FONT Various_Symbols_16x32  // символьный шрифт
  //#define SCREEN_BIG_SYMBOL_FONT Various_Symbols_32x32  // символьный шрифт
  #define SCREEN_ORIENTATION  PORTRAIT              // ориентация экрана вертикальная
  #define TOUCH_PRECISION PREC_HI
  #define SCREEN_BUTTON_COLORS VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_BLUE // цвета для кнопок
  #define SCREEN_BUTTON_COLORS2 VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, 0x4DC9 // цвета для кнопок
  

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// абстрактный класс экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractTFTScreen
{
  public:

    void setup(TFTMenu* menu);
    void update(TFTMenu* menu);
    void draw(TFTMenu* menu);

    const char* getName() {return screenName;}
    bool isActive() {return isActiveScreen; }
    void setActive(bool val){ isActiveScreen = val; }
  
    AbstractTFTScreen(const char* name);
    virtual ~AbstractTFTScreen();

    // вызывается, когда переключаются на экран
    virtual void onActivate(){}

    // вызывается, когда экран становится неактивным
    virtual void onDeactivate() {}

    void notifyButtonPressed(int pressedButton);

  protected:

    
    virtual void doSetup(TFTMenu* menu) = 0;
    virtual void doUpdate(TFTMenu* menu) = 0;
    virtual void doDraw(TFTMenu* menu) = 0;
    virtual void onButtonPressed(TFTMenu* menu,int pressedButton) = 0;

  
	UTFT_Buttons_Rus* screenButtons; // кнопки на экране    
 
    private:
      const char* screenName;
      bool isActiveScreen;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<AbstractTFTScreen*> HALScreensList; // список экранов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс-менеджер работы с экраном
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (*OnScreenAction)(AbstractTFTScreen* screen);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenu
{

public:
  TFTMenu();

  void setup();
  void update();
  void initHAL();


  #if DISPLAY_USED == DISPLAY_ILI9341
    UTFT* getDC() {return halDCDescriptor; }
    URTouch* getTouch() {return halTouch; }
  #endif

  void addScreen(AbstractTFTScreen* screen);

  AbstractTFTScreen* getActiveScreen();
  void onAction(OnScreenAction handler) {on_action = handler;}
  void notifyAction(AbstractTFTScreen* screen);

  void switchToScreen(AbstractTFTScreen* screen);
  void switchToScreen(const char* screenName);
  void switchToScreen(unsigned int screenIndex);

  // HARDWARE HAL
  int print(const char* str,int x, int y, int deg=0, bool computeStringLengthOnly=false);

  void setFont(FONT_TYPE* font);
  FONT_TYPE* getFont();
  void setBackColor(COLORTYPE color);
  COLORTYPE  getBackColor();
  void setColor(COLORTYPE color);
  COLORTYPE  getColor();
  void fillScreen(COLORTYPE color);
  void  drawRect(int x1, int y1, int x2, int y2);
  void  drawRoundRect(int x1, int y1, int x2, int y2);
  void  fillRect(int x1, int y1, int x2, int y2);
  void  fillRoundRect(int x1, int y1, int x2, int y2);
  void clrRoundRect(int x1, int y1, int x2, int y2);
  uint16_t getFontWidth(FONT_TYPE* font);
  uint16_t getFontHeight(FONT_TYPE* font);

  uint16_t getScreenWidth();
  uint16_t getScreenHeight();

  void clearScreen();

  void updateDisplay();
  

private:

#if DISPLAY_USED == DISPLAY_ILI9341
  int printILI(const char* str,int x, int y, int deg=0, bool computeStringLengthOnly=false);
#endif


  //String utf8rus(const char* source);

  AbstractTFTScreen* requestedToActiveScreen;
  int requestedToActiveScreenIndex;

  OnScreenAction on_action;
  
  HALScreensList screens;
  HalDCDescriptor* halDCDescriptor;
  int currentScreenIndex;

  #if DISPLAY_USED == DISPLAY_ILI9341
    URTouch* halTouch;
  #endif

  FONT_TYPE* curFont;

  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern TFTMenu Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  mbShow,
  mbConfirm
  
} MessageBoxType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MessageBoxScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create();

  void confirm(Vector<const char*>& lines, const char* okTarget, const char* cancelTarget);
  void show(Vector<const char*>& lines, const char* okTarget);
    
protected:

    virtual void doSetup(TFTMenu* dc);
    virtual void doUpdate(TFTMenu* dc);
    virtual void doDraw(TFTMenu* dc);
    virtual void onButtonPressed(TFTMenu* dc, int pressedButton);

private:
      MessageBoxScreen();

      const char* targetOkScreen;
      const char* targetCancelScreen;
      Vector<const char*> lines;

      MessageBoxType boxType;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MessageBoxScreen* MessageBox;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// работа с экранной клавиатурой
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	ktDigits, // только 0-9
	ktHex, // 0-9, A-F

} KeyboardType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// подписчик на результат ввода данных с экранной клавиатуры
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct KeyboardInputTarget
{
	virtual void onKeyboardInput(bool enterPressed, const String& enteredValue) = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс экранной клавиатуры
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class KeyboardScreen : public AbstractTFTScreen
{
public:

	static AbstractTFTScreen* create();

	void show(KeyboardType type, const String& initialValue, AbstractTFTScreen* targetScreen, KeyboardInputTarget* targetInput, int maxLength = -1);

protected:

	virtual void doSetup(TFTMenu* menu);
	virtual void doUpdate(TFTMenu* menu);
	virtual void doDraw(TFTMenu* menu);
	virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
	KeyboardScreen();

	String input;
	KeyboardInputTarget* targetInput;
	AbstractTFTScreen* targetScreen;

	size_t input_maxlength;

	int clearButton, enterButton, exitButton;


	void drawValue(TFTMenu* menu);

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern KeyboardScreen* ScreenKeyboard;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif
