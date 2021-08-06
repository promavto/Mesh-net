#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Globals.h"
#include "SPI.h"

#ifdef USE_TFT_MODULE

#include "TinyVector.h"

#include "TFTRus.h"
#include "TFT_Buttons_Rus.h"
#include "TFT_Includes.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INFO_BOX_WIDTH 60
#define INFO_BOX_HEIGHT 20
#define INFO_BOX_V_SPACING 5
#define INFO_BOX_CONTENT_PADDING 8

#define CHANNELS_BUTTONS_PER_LINE 4
#define CHANNELS_BUTTON_WIDTH 60
#define CHANNELS_BUTTON_HEIGHT 10
#define ALL_CHANNELS_BUTTON_WIDTH 50
#define ALL_CHANNELS_BUTTON_HEIGHT 20
//
#define TFT_TEXT_INPUT_WIDTH 60
#define TFT_TEXT_INPUT_HEIGHT 20
#define TFT_ARROW_BUTTON_WIDTH 20
#define TFT_ARROW_BUTTON_HEIGHT 20
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenu;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  int x;
  int y;
  int w;
  int h;
} TFTInfoBoxContentRect;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTInfoBox
{
  public:
    TFTInfoBox(const char* caption, int width, int height, int x, int y, int captionXOffset=0);
    ~TFTInfoBox();

    void draw(TFTMenu* menuManager);
    void drawCaption(TFTMenu* menuManager, const char* caption);
    int getWidth() {return boxWidth;}
    int getHeight() {return boxHeight;}
    int getX() {return posX;}
    int getY() {return posY;}
    const char* getCaption() {return boxCaption;}

    TFTInfoBoxContentRect getContentRect(TFTMenu* menuManager);


   private:

    int boxWidth, boxHeight, posX, posY, captionXOffset;
    const char* boxCaption;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void drawValueInBox(TFTInfoBox* box, const String& strVal,FONTTYPE font = SEVEN_SEG_NUM_FONT_MDS);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ITickHandler
{
  virtual void onTick() = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TickerClass
{
  public:
    TickerClass();
    ~TickerClass();

   void setIntervals(uint16_t beforeStartTickInterval,uint16_t tickInterval);
   void start(ITickHandler* h);
   void stop();
   void tick();

private:

  uint16_t beforeStartTickInterval;
  uint16_t tickInterval;

  uint32_t timer;
  bool started, waitBefore;

  ITickHandler* handler;
  
};
extern TickerClass Ticker;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// абстрактный класс экрана для TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractTFTScreen
{
  public:

    virtual void setup(TFTMenu* menuManager) = 0;
    virtual void update(TFTMenu* menuManager) = 0;
    virtual void draw(TFTMenu* menuManager) = 0;
    virtual void onActivate(TFTMenu* menuManager){}
    virtual void onButtonPressed(TFTMenu* menuManager,int buttonID) {}
    virtual void onButtonReleased(TFTMenu* menuManager,int buttonID) {}
  
    AbstractTFTScreen();
    virtual ~AbstractTFTScreen();
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTCalibrationScreen : public AbstractTFTScreen
{
public:

  TFTCalibrationScreen();
  ~TFTCalibrationScreen();

  void setup(TFTMenu* menuManager);
  void update(TFTMenu* menuManager);
  void draw(TFTMenu* menuManager);

private:

  bool canSwitch;

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTStartScreen : public AbstractTFTScreen
{
public:

  TFTStartScreen();
  ~TFTStartScreen();

  void setup(TFTMenu* menuManager);
  void update(TFTMenu* menuManager);
  void draw(TFTMenu* menuManager);
  void onActivate(TFTMenu* menuManager);

private:

  uint16_t measureTime;
  uint32_t timer;
  int lastTextWidth;

  void drawMeasureTime(TFTMenu* menuManager);
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenuScreen : public AbstractTFTScreen, public ITickHandler
{
public:

	TFTMenuScreen();
	~TFTMenuScreen();

	void setup(TFTMenu* menuManager);
	void update(TFTMenu* menuManager);
	void draw(TFTMenu* menuManager);
	void onActivate(TFTMenu* menuManager);
	void onButtonPressed(TFTMenu* menuManager, int buttonID);
	void onButtonReleased(TFTMenu* menuManager, int buttonID);
	void onTick();

private:

  int tickerButton;

  int startButton, menuButton;
	TFT_Buttons_Rus* screenButtons;	
	bool isActive;

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct MessageBoxResultSubscriber
{
  virtual void onMessageBoxResult(bool okPressed) = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct KeyboardInputTarget
{
  virtual void onKeyboardInputResult(const String& input, bool okPressed) = 0;
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс экрана ожидания
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool isWindowsOpen : 1;
  bool windowsAutoMode : 1;

  bool isWaterOn : 1;
  bool waterAutoMode : 1;

  bool isLightOn : 1;
  bool lightAutoMode : 1;
  
} IdleScreenFlags;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  const char* screenName;
  AbstractTFTScreen* screen;
  
} TFTScreenInfo;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<TFTScreenInfo> TFTScreensList; // список экранов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс-менеджер работы с TFT
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool isLCDOn : 1;
  byte pad : 7;
  
} TFTMenuFlags;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  mbShow,
  mbConfirm,
  mbHalt
  
} MessageBoxType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MessageBoxScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create();

  void confirm(const char* caption, Vector<const char*>& lines, AbstractTFTScreen* okTarget, AbstractTFTScreen* cancelTarget, MessageBoxResultSubscriber* sub=NULL);
  void show(const char* caption, Vector<const char*>& lines, AbstractTFTScreen* okTarget, MessageBoxResultSubscriber* sub=NULL);
  
  void halt(const char* caption, Vector<const char*>& lines, bool errorColors=true, bool haltInWhile=false);
    
protected:

    virtual void setup(TFTMenu* dc);
    virtual void update(TFTMenu* dc);
    virtual void draw(TFTMenu* dc);

private:
      MessageBoxScreen();

      AbstractTFTScreen* targetOkScreen;
      AbstractTFTScreen* targetCancelScreen;
      Vector<const char*> lines;

      const char* caption;

      MessageBoxResultSubscriber* resultSubscriber;
      bool errorColors, haltInWhile;

      TFT_Buttons_Rus* buttons;
      int yesButton, noButton;

      MessageBoxType boxType;

      void recreateButtons();
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MessageBoxScreen* MessageBox;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  ktFull, // полная клавиатура
  ktNumbers, // ввод только чисел и точки
  
} KeyboardType;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define KBD_BUTTONS_IN_ROW  10
#define KBD_BUTTON_WIDTH  25
#define KBD_BUTTON_HEIGHT  20
#define KBD_SPACING  6
#define KDB_BIG_BUTTON_HEIGHT 30
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class KeyboardScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create();

  void show(const String& val, int maxLen, KeyboardInputTarget* inputTarget, KeyboardType keyboardType=ktFull, bool eng=false);

  ~KeyboardScreen();  
protected:

    virtual void setup(TFTMenu* dc);
    virtual void update(TFTMenu* dc);
    virtual void draw(TFTMenu* dc);

private:
      KeyboardScreen();

      KeyboardInputTarget* inputTarget;
      String inputVal;
      int maxLen;
      bool isRusInput;

      TFT_Buttons_Rus* buttons;
      Vector<String*> captions;
      Vector<String*> altCaptions;

      int backspaceButton, okButton, cancelButton, switchButton, spaceButton;

      void createKeyboard(TFTMenu* dc);
      void drawValue(TFTMenu* dc, bool deleteCharAtCursor=false);
      void switchInput(bool redraw);

      void applyType(KeyboardType keyboardType);

      int cursorPos;
      void redrawCursor(TFTMenu* dc, bool erase);

      char utf8Bytes[7];
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern KeyboardScreen* Keyboard;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenu
{

public:
  TFTMenu();

  void setup();
  void update();

  void switchToScreen(const char* screenName);
  void switchToScreen(AbstractTFTScreen* to);
  
  TFT_Class* getDC() { return tftDC; };
  TOUCH_Class* getTouch() { return tftTouch; };
  TFTRus* getRusPrinter() { return &rusPrint; };
 
  void resetIdleTimer();

  void onButtonPressed(int button);
  void onButtonReleased(int button);

private:

  TFTScreensList screens;
  TFT_Class* tftDC;
  TOUCH_Class* tftTouch;
  TFTRus rusPrint;

  int currentScreenIndex;
  
  AbstractTFTScreen* switchTo;
  int switchToIndex;

  unsigned long idleTimer;
  
  TFTMenuFlags flags;

  void lcdOn();
  void lcdOff();
  void switchBacklight(uint8_t level);
  
};
extern TFTMenu* TFTScreen;
#endif // USE_TFT_MODULE
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

