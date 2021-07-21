#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "SPI.h"
#include <TFT_eSPI.h>

#include "Configuration_ESP32.h"

#ifdef USE_TFT_MODULE

#include "TinyVector.h"

#include "TFTRus.h"
#include "TFT_Buttons_Rus.h"
#include "TFT_Includes.h"


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define TFT_IDLE_SCREEN_BUTTON_WIDTH 128
//#define TFT_IDLE_SCREEN_BUTTON_HEIGHT 90
//#define TFT_IDLE_SCREEN_BUTTON_SPACING 5
//#define TFT_BACK_BUTTON_WIDTH 250
//
#define INFO_BOX_WIDTH 60
#define INFO_BOX_HEIGHT 20
#define INFO_BOX_V_SPACING 5
#define INFO_BOX_CONTENT_PADDING 8
//
//#define SENSOR_BOX_WIDTH 240
//#define SENSOR_BOX_HEIGHT 120
//#define SENSOR_BOX_V_SPACING 30
//#define SENSOR_BOX_H_SPACING 20
//#define SENSOR_BOXES_PER_LINE 3
//
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

typedef TFT_eSPI HalDCDescriptor;







//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// абстрактный класс экрана
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AbstractTFTScreen
{
public:

	void setup(TFTMenu* menu);
	void update(TFTMenu* menu);
	void draw(TFTMenu* menu);

	const char* getName() { return screenName; }
	bool isActive() { return isActiveScreen; }
	void setActive(bool val) { isActiveScreen = val; }

	AbstractTFTScreen(const char* name);
	virtual ~AbstractTFTScreen();

	// вызывается, когда переключаются на экран
	virtual void onActivate() {}

	// вызывается, когда экран становится неактивным
	virtual void onDeactivate() {}

	void notifyButtonPressed(int pressedButton);

protected:


	virtual void doSetup(TFTMenu* menu) = 0;
	virtual void doUpdate(TFTMenu* menu) = 0;
	virtual void doDraw(TFTMenu* menu) = 0;
	virtual void onButtonPressed(TFTMenu* menu, int pressedButton) = 0;


	TFT_Buttons_Rus* screenButtons; // кнопки на экране    

private:
	const char* screenName;
	bool isActiveScreen;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<AbstractTFTScreen*> HALScreensList; // список экранов
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// класс-менеджер работы с экраном
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void(*OnScreenAction)(AbstractTFTScreen* screen);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TFTMenu
{

public:
	TFTMenu();

	void setup();
	void update();
	void initHAL();



	TFT_eSPI* getDC() { return halDCDescriptor; }
	URTouch* getTouch() { return halTouch; }
	TFTRus* getRusPrinter() { return &rusPrint; };

	void addScreen(AbstractTFTScreen* screen);

	AbstractTFTScreen* getActiveScreen();
	void onAction(OnScreenAction handler) { on_action = handler; }
	void notifyAction(AbstractTFTScreen* screen);

	void switchToScreen(AbstractTFTScreen* screen);
	void switchToScreen(const char* screenName);
	void switchToScreen(unsigned int screenIndex);

	// HARDWARE HAL
	//int print(const char* str, int x, int y, int deg = 0, bool computeStringLengthOnly = false);

	//void setFont(FONT_TYPE* font);
	//FONT_TYPE* getFont();
	//void setBackColor(COLORTYPE color);
	//COLORTYPE  getBackColor();
	//void setColor(COLORTYPE color);
	//COLORTYPE  getColor();
	//void fillScreen(COLORTYPE color);
	//void  drawRect(int x1, int y1, int x2, int y2);
	//void  drawRoundRect(int x1, int y1, int x2, int y2);
	//void  fillRect(int x1, int y1, int x2, int y2);
	//void  fillRoundRect(int x1, int y1, int x2, int y2);
	//void clrRoundRect(int x1, int y1, int x2, int y2);
	//uint16_t getFontWidth(FONT_TYPE* font);
	//uint16_t getFontHeight(FONT_TYPE* font);

	//uint16_t getScreenWidth();
	//uint16_t getScreenHeight();

	void clearScreen();




private:


	//int printILI(const char* str, int x, int y, int deg = 0, bool computeStringLengthOnly = false);



	//String utf8rus(const char* source);

	AbstractTFTScreen* requestedToActiveScreen;
	int requestedToActiveScreenIndex;

	OnScreenAction on_action;

	HALScreensList screens;
	HalDCDescriptor* halDCDescriptor;
	int currentScreenIndex;


	URTouch* halTouch;

	TFTRus rusPrint;

	//FONT_TYPE* curFont;


};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern TFTMenu Screen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------



































//=============================================================================================================
//typedef struct
//{
//  int x;
//  int y;
//  int w;
//  int h;
//} TFTInfoBoxContentRect;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class TFTInfoBox
//{
//  public:
//    TFTInfoBox(const char* caption, int width, int height, int x, int y, int captionXOffset=0);
//    ~TFTInfoBox();
//
//    void draw(TFTMenu* menuManager);
//    void drawCaption(TFTMenu* menuManager, const char* caption);
//    int getWidth() {return boxWidth;}
//    int getHeight() {return boxHeight;}
//    int getX() {return posX;}
//    int getY() {return posY;}
//    const char* getCaption() {return boxCaption;}
//
//    TFTInfoBoxContentRect getContentRect(TFTMenu* menuManager);
//
//
//   private:
//
//    int boxWidth, boxHeight, posX, posY, captionXOffset;
//    const char* boxCaption;
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void drawValueInBox(TFTInfoBox* box, const String& strVal,FONTTYPE font = SEVEN_SEG_NUM_FONT_MDS);
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//struct ITickHandler
//{
//  virtual void onTick() = 0;
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class TickerClass
//{
//  public:
//    TickerClass();
//    ~TickerClass();
//
//   void setIntervals(uint16_t beforeStartTickInterval,uint16_t tickInterval);
//   void start(ITickHandler* h);
//   void stop();
//   void tick();
//
//private:
//
//  uint16_t beforeStartTickInterval;
//  uint16_t tickInterval;
//
//  uint32_t timer;
//  bool started, waitBefore;
//
//  ITickHandler* handler;
//  
//};
//extern TickerClass Ticker;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// абстрактный класс экрана для TFT
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class AbstractTFTScreen
//{
//  public:
//
//    virtual void setup(TFTMenu* menuManager) = 0;
//    virtual void update(TFTMenu* menuManager) = 0;
//    virtual void draw(TFTMenu* menuManager) = 0;
//    virtual void onActivate(TFTMenu* menuManager){}
//    virtual void onButtonPressed(TFTMenu* menuManager,int buttonID) {}
//    virtual void onButtonReleased(TFTMenu* menuManager,int buttonID) {}
//  
//    AbstractTFTScreen();
//    virtual ~AbstractTFTScreen();
//};
//
//
////========================================================================================================================================================================
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class TFTWMScreen : public AbstractTFTScreen, public ITickHandler
//{
//public:
//
//	TFTWMScreen(uint8_t channel);
//	~TFTWMScreen();
//
//	void setup(TFTMenu* menuManager);
//	void update(TFTMenu* menuManager);
//	void draw(TFTMenu* menuManager);
//	void onActivate(TFTMenu* menuManager);
//	void onButtonPressed(TFTMenu* menuManager, int buttonID);
//	void onButtonReleased(TFTMenu* menuManager, int buttonID);
//	void onTick();
//
//private:
//
//	uint8_t channel;
//	int tickerButton;
//
//	int backButton, saveButton, onOffButton;
//	TFT_Buttons_Rus* screenButtons;
//
//	TFTInfoBox* percents25Box;
//	int16_t percents25Temp;
//	int dec25PercentsButton, inc25PercentsButton;
//	void inc25Temp(int val);
//
//	TFTInfoBox* percents50Box;
//	int16_t percents50Temp;
//	int dec50PercentsButton, inc50PercentsButton;
//	void inc50Temp(int val);
//
//	TFTInfoBox* percents75Box;
//	int16_t percents75Temp;
//	int dec75PercentsButton, inc75PercentsButton;
//	void inc75Temp(int val);
//
//	TFTInfoBox* percents100Box;
//	int16_t percents100Temp;
//	int dec100PercentsButton, inc100PercentsButton;
//	void inc100Temp(int val);
//
//	TFTInfoBox* histeresisBox;
//	int16_t histeresis;
//	int decHisteresisButton, incHisteresisButton;
//	String formatHisteresis();
//
//	TFTInfoBox* sensorBox;
//	int16_t sensor;
//	int decSensorButton, incSensorButton;
//
//	bool isActive;
//
//	void saveSettings();
//
//	uint32_t blinkTimer;
//	bool blinkOn, blinkActive;
//
//	void blinkSaveSettingsButton(bool bOn);
//
//	int sensorDataLeft, sensorDataTop;
//	String sensorDataString;
//	void drawSensorData(TFTMenu* menuManager);
//	void getSensorData();
//
//	uint16_t tempSensorsCount, humiditySensorsCount;
//
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class TFTMenuScreen : public AbstractTFTScreen, public ITickHandler
//{
//public:
//
//	TFTMenuScreen(uint8_t channel);
//	~TFTMenuScreen();
//
//	void setup(TFTMenu* menuManager);
//	void update(TFTMenu* menuManager);
//	void draw(TFTMenu* menuManager);
//	void onActivate(TFTMenu* menuManager);
//	void onButtonPressed(TFTMenu* menuManager, int buttonID);
//	void onButtonReleased(TFTMenu* menuManager, int buttonID);
//	void onTick();
//
//private:
//
//	uint8_t channel;
//	int tickerButton;
//
//	int backButton, saveButton, onOffButton;
//	TFT_Buttons_Rus* screenButtons;
//	
//	bool isActive;
//
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
////class TFTWMenuScreen : public AbstractTFTScreen
////{
////public:
////
////	TFTWMenuScreen();
////	~TFTWMenuScreen();
////
////	void setup(TFTMenu* menuManager);
////	void update(TFTMenu* menuManager);
////	void draw(TFTMenu* menuManager);
////	void onActivate(TFTMenu* menuManager);
////
////private:
////
////	int backButton;
////	int windowsButton, rainButton, orientationButton, windButton;
////	int temp1Button, temp2Button, temp3Button, temp4Button;
////
////	TFT_Buttons_Rus* screenButtons;
////
////};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//
//
//
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////========================================================================================================================================================================
//struct MessageBoxResultSubscriber
//{
//  virtual void onMessageBoxResult(bool okPressed) = 0;
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//struct KeyboardInputTarget
//{
//  virtual void onKeyboardInputResult(const String& input, bool okPressed) = 0;
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// класс экрана ожидания
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#pragma pack(push,1)
//typedef struct
//{
//  bool isWindowsOpen : 1;
//  bool windowsAutoMode : 1;
//
//  bool isWaterOn : 1;
//  bool waterAutoMode : 1;
//
//  bool isLightOn : 1;
//  bool lightAutoMode : 1;
//  
//} IdleScreenFlags;
//#pragma pack(pop)
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef struct
//{
//  TFTInfoBox* box;
//  
//} TFTInfoBoxWithIndex;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define TFT_IDLE_SCREENS 6 // сколько у нас экранов на экране ожидания
//#define TFT_IDLE_STATUSES_SCREEN_NUMBER 2
//#define TFT_IDLE_SENSORS_SCREEN_NUMBER 0
//#define TFT_IDLE_SENSORS_SCREEN_NUMBER2 1
//#define TFT_IDLE_HEAT_SCREEN_NUMBER 3
//#define TFT_IDLE_FLOW_SCREEN_NUMBER 4
//#define TFT_IDLE_PH_SCREEN_NUMBER 5
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef struct
//{
//  const char* screenName;
//  AbstractTFTScreen* screen;
//  
//} TFTScreenInfo;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef Vector<TFTScreenInfo> TFTScreensList; // список экранов
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//// класс-менеджер работы с TFT
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#pragma pack(push,1)
//typedef struct
//{
//  bool isLCDOn : 1;
//  byte pad : 7;
//  
//} TFTMenuFlags;
//#pragma pack(pop)
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef enum
//{
//  mbShow,
//  mbConfirm,
//  mbHalt
//  
//} MessageBoxType;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class MessageBoxScreen : public AbstractTFTScreen
//{
//  public:
//
//  static AbstractTFTScreen* create();
//
//  void confirm(const char* caption, Vector<const char*>& lines, AbstractTFTScreen* okTarget, AbstractTFTScreen* cancelTarget, MessageBoxResultSubscriber* sub=NULL);
//  void show(const char* caption, Vector<const char*>& lines, AbstractTFTScreen* okTarget, MessageBoxResultSubscriber* sub=NULL);
//  
//  void halt(const char* caption, Vector<const char*>& lines, bool errorColors=true, bool haltInWhile=false);
//    
//protected:
//
//    virtual void setup(TFTMenu* dc);
//    virtual void update(TFTMenu* dc);
//    virtual void draw(TFTMenu* dc);
//
//private:
//      MessageBoxScreen();
//
//      AbstractTFTScreen* targetOkScreen;
//      AbstractTFTScreen* targetCancelScreen;
//      Vector<const char*> lines;
//
//      const char* caption;
//
//      MessageBoxResultSubscriber* resultSubscriber;
//      bool errorColors, haltInWhile;
//
//      TFT_Buttons_Rus* buttons;
//      int yesButton, noButton;
//
//      MessageBoxType boxType;
//
//      void recreateButtons();
//  
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//extern MessageBoxScreen* MessageBox;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//typedef enum
//{
//  ktFull, // полная клавиатура
//  ktNumbers, // ввод только чисел и точки
//  
//} KeyboardType;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define KBD_BUTTONS_IN_ROW  10
//#define KBD_BUTTON_WIDTH  25
//#define KBD_BUTTON_HEIGHT  20
//#define KBD_SPACING  6
//#define KDB_BIG_BUTTON_HEIGHT 30
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class KeyboardScreen : public AbstractTFTScreen
//{
//  public:
//
//  static AbstractTFTScreen* create();
//
//  void show(const String& val, int maxLen, KeyboardInputTarget* inputTarget, KeyboardType keyboardType=ktFull, bool eng=false);
//
//  ~KeyboardScreen();  
//protected:
//
//    virtual void setup(TFTMenu* dc);
//    virtual void update(TFTMenu* dc);
//    virtual void draw(TFTMenu* dc);
//
//private:
//      KeyboardScreen();
//
//      KeyboardInputTarget* inputTarget;
//      String inputVal;
//      int maxLen;
//      bool isRusInput;
//
//      TFT_Buttons_Rus* buttons;
//      Vector<String*> captions;
//      Vector<String*> altCaptions;
//
//      int backspaceButton, okButton, cancelButton, switchButton, spaceButton;
//
//      void createKeyboard(TFTMenu* dc);
//      void drawValue(TFTMenu* dc, bool deleteCharAtCursor=false);
//      void switchInput(bool redraw);
//
//      void applyType(KeyboardType keyboardType);
//
//      int cursorPos;
//      void redrawCursor(TFTMenu* dc, bool erase);
//
//      char utf8Bytes[7];
//  
//};
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//extern KeyboardScreen* Keyboard;
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//class TFTMenu
//{
//
//public:
//  TFTMenu();
//
//  void setup();
//  void update();
//
//  void switchToScreen(const char* screenName);
//  void switchToScreen(AbstractTFTScreen* to);
//  
//  TFT_Class* getDC() { return tftDC; };
//  TOUCH_Class* getTouch() { return tftTouch; };
//  TFTRus* getRusPrinter() { return &rusPrint; };
// 
//  void resetIdleTimer();
//
//  void onButtonPressed(int button);
//  void onButtonReleased(int button);
//
//private:
//
//  TFTScreensList screens;
//  TFT_Class* tftDC;
//  TOUCH_Class* tftTouch;
//  TFTRus rusPrint;
//
//  int currentScreenIndex;
//  
//  AbstractTFTScreen* switchTo;
//  int switchToIndex;
//
//  unsigned long idleTimer;
//  
//  TFTMenuFlags flags;
//
//  void lcdOn();
//  void lcdOff();
//  void switchBacklight(uint8_t level);
//
//  
//};
//extern TFTMenu* TFTScreen;
#endif // USE_TFT_MODULE
////------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
