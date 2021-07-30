#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "UTFTMenu.h"
#include "Settings.h"
#include "FileUtils.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// главный экран
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MainScreen : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new MainScreen();
  }
  
   virtual void onActivate();
   virtual void onDeactivate();


protected:
  
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
    MainScreen();

    Si7021Data lastSensorData;
    void drawTemperature(TFTMenu* menu);

    Temperature ds18b20Value;
    void drawDS18B20(TFTMenu* menu);
    
	  int oldsecond;
	  void drawTime(TFTMenu* menu);

    bool lastLogActiveFlag;
    void drawLogState(TFTMenu* menu);
    void drawLogDuration(TFTMenu* menu);

   bool lastDoorOpen;
   bool doorStateVisible;
   uint32_t doorStateBlinkTimer;
   bool blinkDoorState;
   void drawDoorState(TFTMenu* menu);
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern MainScreen* mainScreen;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MenuScreen1 : public AbstractTFTScreen
{
  public:

  static AbstractTFTScreen* create()
  {
    return new MenuScreen1();
  }
  
   virtual void onActivate();
   virtual void onDeactivate();


protected:
  
    virtual void doSetup(TFTMenu* menu);
    virtual void doUpdate(TFTMenu* menu);
    virtual void doDraw(TFTMenu* menu);
    virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
    MenuScreen1();

    void drawGUI(TFTMenu* menu);
    //LinkList<uint8_t> intervalsRingBuffer;
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  exportToSerial,
  exportToWiFi,
  exportToPrinter
  
} ExportMode;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SCREEN_FILES_COUNT 4 // кол-во файлов на одном экране
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ExportToSerialScreen : public AbstractTFTScreen
{
public:

  static AbstractTFTScreen* create()
  {
    return new ExportToSerialScreen();
  }

  virtual void onActivate();
  virtual void onDeactivate();

  void setFile(FileEntry* entry);


protected:

  virtual void doSetup(TFTMenu* menu);
  virtual void doUpdate(TFTMenu* menu);
  virtual void doDraw(TFTMenu* menu);
  virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
  ExportToSerialScreen();
  FileEntry* selectedFile;

  bool isExportDone;
  void startExport();
};  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ExportLogsScreen : public AbstractTFTScreen
{
public:

  static AbstractTFTScreen* create()
  {
    return new ExportLogsScreen();
  }

  virtual void onActivate();
  virtual void onDeactivate();

  void setMode(ExportMode mode);
  void rescanFiles();


protected:

  virtual void doSetup(TFTMenu* menu);
  virtual void doUpdate(TFTMenu* menu);
  virtual void doDraw(TFTMenu* menu);
  virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
  ExportLogsScreen();

  ExportMode exportMode;

  bool hasSD;
  int totalFilesCount;
  FileEntry** files;
  void clearFiles();

  int selectedFileNumber;
  int currentPage;
  void selectFile(TFTMenu* menu, int8_t step);

  void drawGUI(TFTMenu* menu);
  void drawFiles(TFTMenu* menu);

  void exportSelectedFile(TFTMenu* menu);

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MenuScreen2 : public AbstractTFTScreen
{
public:

	static AbstractTFTScreen* create()
	{
		return new MenuScreen2();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(TFTMenu* menu);
	virtual void doUpdate(TFTMenu* menu);
	virtual void doDraw(TFTMenu* menu);
	virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
	MenuScreen2();

  void drawGUI(TFTMenu* menu);
  void drawStartScreen(TFTMenu* menu);


};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class BordersScreen : public AbstractTFTScreen
{
public:

	static AbstractTFTScreen* create()
	{
		return new BordersScreen();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(TFTMenu* menu);
	virtual void doUpdate(TFTMenu* menu);
	virtual void doDraw(TFTMenu* menu);
	virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
	BordersScreen();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
  temperatureBorder,
  humidityBorder,
  adcBorder
  
} WhichBorder;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class EditBorderScreen : public AbstractTFTScreen
{
public:

  static AbstractTFTScreen* create(const char* screenName, WhichBorder b)
  {
    return new EditBorderScreen(screenName, b);
  }

  virtual void onActivate();
  virtual void onDeactivate();


protected:

  virtual void doSetup(TFTMenu* menu);
  virtual void doUpdate(TFTMenu* menu);
  virtual void doDraw(TFTMenu* menu);
  virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
  EditBorderScreen(const char* screenName, WhichBorder b);

  WhichBorder whichBorder;
  int16_t minVal, maxVal, currentMinVal, currentMaxVal;
  uint8_t changeStep;
  uint8_t editedVal;
  const char* caption;

  void saveValues();
  void reloadValues();

  void drawGUI(TFTMenu* menu);

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MenuScreen4 : public AbstractTFTScreen
{
public:

	static AbstractTFTScreen* create()
	{
		return new MenuScreen4();
	}

	virtual void onActivate();
	virtual void onDeactivate();


protected:

	virtual void doSetup(TFTMenu* menu);
	virtual void doUpdate(TFTMenu* menu);
	virtual void doDraw(TFTMenu* menu);
	virtual void onButtonPressed(TFTMenu* menu, int pressedButton);

private:
	MenuScreen4();

};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
