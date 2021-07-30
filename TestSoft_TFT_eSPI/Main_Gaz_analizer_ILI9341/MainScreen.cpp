//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "MainScreen.h"
#include "DS3231.h"
#include "CONFIG.h"
#include "Settings.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MainScreen* mainScreen = NULL;        
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MainScreen::MainScreen() : AbstractTFTScreen("Main")
{
  mainScreen = this;
  ds18b20Value.value = NO_TEMPERATURE_DATA;
  memset(&lastSensorData,0,sizeof(lastSensorData));
  lastSensorData.temperature = NO_TEMPERATURE_DATA;
  lastSensorData.humidity = NO_TEMPERATURE_DATA;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onDeactivate()
{
  // станем неактивными
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onActivate()
{
  // мы активизируемся
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doSetup(TFTMenu* menu)
{
  // первоначальная настройка экрана
	screenButtons->setTextFont(SCREEN_BIG_FONT);

	screenButtons->addButton(5, 200, 110, 60, "СТАРТ");
	screenButtons->addButton(125, 200, 110, 60, "СТОП");
  	screenButtons->addButton(5, 270, 190, 45, "НАСТРОЙКИ");
  	screenButtons->addButton(200, 270, 35, 45, "z", BUTTON_SYMBOL); // кнопка Часы 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doUpdate(TFTMenu* menu)
{
  if(!isActive())
    return;

      
	// обновление экрана
  static uint32_t tempUpdateTimer = 0;
  bool wantDrawTemp = false, wantDrawDS18B20 = false, wantDrawTime = false, wantDrawDoorState = false;
    
  if(millis() - tempUpdateTimer > SENSORS_UPDATE_FREQUENCY)
  {
    tempUpdateTimer = millis();
    
   /* Si7021Data thisData = Settings.readSensor();
    
    if(memcmp(&thisData,&lastSensorData,sizeof(Si7021Data)))
    {
      memcpy(&lastSensorData,&thisData,sizeof(Si7021Data));      
      wantDrawTemp = true;
    }

    Temperature thisDS18B20Val = Settings.getDS18B20SensorTemperature();
    if(thisDS18B20Val.value != ds18b20Value.value || thisDS18B20Val.fract != ds18b20Value.fract)
    {
      ds18b20Value = thisDS18B20Val;      
      wantDrawDS18B20 = true;
    }*/
	
  } // if(millis() - ....

  DS3231Time tm = RealtimeClock.getTime();
  if (oldsecond != tm.second)
  {
    oldsecond = tm.second;
    wantDrawTime = true;
  }

  bool curDoorOpen = Settings.isDoorOpen();
  if(lastDoorOpen != curDoorOpen)
  {
    lastDoorOpen = curDoorOpen;
    wantDrawDoorState = true;
  }

  bool curLogActive = Settings.isLoggingEnabled();
  bool wantDrawLogState = false;
  if(curLogActive != lastLogActiveFlag)
  {
    lastLogActiveFlag = curLogActive;
    wantDrawLogState = true;
  }

  bool lastBlinkState = blinkDoorState;

  if(lastLogActiveFlag && lastDoorOpen) // открыто при включенном логгировании - надо мигать
  {
   /* if(!blinkDoorState)
    {
      blinkDoorState = true;
      doorStateBlinkTimer = millis();
      wantDrawDoorState = true;
      doorStateVisible = true;
    }*/
  }
  else
    blinkDoorState = false;


  if(blinkDoorState)
  {
    if(millis() - doorStateBlinkTimer > 750)
    {
      doorStateBlinkTimer = millis();
      doorStateVisible = !doorStateVisible;
      wantDrawDoorState = true;
    }
  }

  if(lastBlinkState != blinkDoorState)
  {
    doorStateVisible = true;
    wantDrawDoorState = true;
  }
  

  if(wantDrawTemp || wantDrawDS18B20 || wantDrawTime)
  {
     //drawTemperature(menu);
     //drawDS18B20(menu);
     drawTime(menu);
 
  }  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawLogDuration(TFTMenu* menu)
{
    // рисуем продолжительность логгирования - в часах
    String durationCaption;
    uint32_t duration = Settings.getLoggingDuration();

    duration /= 60; // в минуты
    duration /= 60; // в часы
    
    durationCaption = duration;

    menu->setFont(SCREEN_SMALL_FONT);
    menu->setColor(SCREEN_TEXT_COLOR);
  
    uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
    uint8_t fontWidth = menu->getFontWidth(SCREEN_SMALL_FONT);
  
    uint16_t screenWidth = menu->getScreenWidth();
    uint16_t screenHeight = menu->getScreenHeight();
  
    uint16_t captionWidth = fontWidth*durationCaption.length();

    // рисуем слева от бокса интервала логгирования
    String logCaption = "0";
    if(lastLogActiveFlag)
    {
      logCaption = Settings.getLoggingInterval();
    }
    
    uint16_t drawX = screenWidth - captionWidth - 12 - logCaption.length()*fontWidth;
    uint16_t drawY = screenHeight - fontHeight - 2;
  
    menu->print(durationCaption.c_str(),drawX, drawY);
    
    menu->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);

  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawDS18B20(TFTMenu* menu)
{  
  // отрисовка показаний ADC
  /*
  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
  
  String adcString = F("Темп2: ");
  adcString += adcValue;
  menu->print(adcString.c_str(),0,fontHeight*2 + 2*2);
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawLogState(TFTMenu* menu)
{
  String logCaption = "0";
  if(lastLogActiveFlag)
  {
    logCaption = Settings.getLoggingInterval();
  }

  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = menu->getFontWidth(SCREEN_SMALL_FONT);

  uint16_t screenWidth = menu->getScreenWidth();
  uint16_t screenHeight = menu->getScreenHeight();

  uint16_t captionWidth = fontWidth*logCaption.length();
  uint16_t drawX = screenWidth - captionWidth - 4;
  uint16_t drawY = screenHeight - fontHeight - 2;


  menu->print(logCaption.c_str(),drawX, drawY);
    
  menu->drawRoundRect(drawX - 2, drawY - 2, drawX + captionWidth + 2, drawY + fontHeight);



}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTemperature(TFTMenu* menu)
{ 
  // отрисовка температуры/влажности
  String displayString;

  uint16_t screenWidth = menu->getScreenWidth();
  uint16_t screenHeight = menu->getScreenHeight();   

  menu->setFont(SmallRusFont);
  menu->setColor(SCREEN_TEXT_COLOR); 

  uint8_t fontHeight = menu->getFontHeight(SmallRusFont);
  uint8_t fontWidth = menu->getFontWidth(SmallRusFont);

  if(lastSensorData.temperature == NO_TEMPERATURE_DATA) // нет температуры
  {
    displayString = "";
  }
  else // есть температура
  {   
    displayString += lastSensorData.temperature;
    /*
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.temperatureFract < 10)
      displayString += '0';

    displayString += lastSensorData.temperatureFract;
    */
    displayString += ';'; // градус
  }

  uint16_t stringWidth = displayString.length()*fontWidth;

  uint16_t drawX = 0;//(screenWidth - stringWidth)/2;
  uint16_t drawY = 0;

  menu->print(displayString.c_str(), drawX, drawY);



  String percents;
  bool hasHumidity = false;
  if(lastSensorData.humidity == NO_TEMPERATURE_DATA) // нет влажности
  {
    displayString = "";
  }
  else // есть влажность
  {   
    hasHumidity = true;
    displayString = lastSensorData.humidity;
    /*
    displayString += DECIMAL_SEPARATOR;

    if(lastSensorData.humidityFract < 10)
      displayString += '0';

    displayString += lastSensorData.humidityFract;
    */
    percents = '%';
  }

  drawY += fontHeight;
  stringWidth = displayString.length()*fontWidth;
  drawX = 0;//(screenWidth - stringWidth - ( hasHumidity ? fontWidth : 0 ))/2;
  menu->print(displayString.c_str(), drawX, drawY);

  drawX += stringWidth;
  menu->setFont(SCREEN_SMALL_FONT);
  menu->print(percents.c_str(), drawX, drawY);   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::drawTime(TFTMenu* menu)
{
	DS3231Time tm = RealtimeClock.getTime();
	
		menu->setFont(SCREEN_SMALL_FONT);
		menu->setColor(SCREEN_TEXT_COLOR);

    uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
    uint16_t screenWidth = menu->getScreenWidth();
    uint16_t screenHeight = menu->getScreenHeight();  
	menu->setFont(BigRusFont);

    String strDate;
	String strTime;

    if(tm.dayOfMonth < 10)
      strDate += '0';
    strDate += tm.dayOfMonth;
    strDate += '/';
  
   if(tm.month < 10)
      strDate += '0';
    strDate += tm.month;
	strDate += '/';

	strDate += tm.year;

   // strDate += ' ';  
 
    if(tm.hour < 10)
		strTime += '0';
	strTime += tm.hour;
	strTime += ':';
  
    if(tm.minute < 10)
		strTime += '0';
	strTime += tm.minute;
	strTime += ':';

	if (tm.second < 10)
		strTime += '0';
	strTime += tm.second;
      
		// печатаем их
    int drawX = 2;
	int drawY = 2;
    //int drawY = screenHeight - fontHeight;
    
	menu->print(strDate.c_str(), drawX, drawY);
	menu->print(strTime.c_str(), drawX, 22);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::doDraw(TFTMenu* menu)
{
	menu->setFont(SCREEN_SMALL_FONT);
	menu->print(SOFTWARE_VERSION, 172, 2);
   
 /*  drawTemperature(menu);

   ds18b20Value = Settings.getDS18B20SensorTemperature();
   drawDS18B20(menu);*/
   drawTime(menu);
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MainScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обрабатываем нажатия на кнопки

  // Для TFT-экрана кнопки начинаются с нуля
  
    if (pressedButton == 0)
    {
      //menu->switchToScreen("MenuScreen1"); // переключаемся на экран настроек
    }   
	if (pressedButton == 1)
	{
		//menu->switchToScreen("RTCSettings"); // переключаемся на экран настроек даты и времени
	}
	if (pressedButton == 2)
	{
		menu->switchToScreen("MenuScreen1"); // переключаемся на экран настроек
	}
	if (pressedButton == 3)
	{
		menu->switchToScreen("RTCSettings"); // переключаемся на экран настроек даты и времени
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen1
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen1::MenuScreen1() : AbstractTFTScreen("MenuScreen1")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onDeactivate()
{
  // станем неактивными
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onActivate()
{
  // мы активизируемся
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::doSetup(TFTMenu* menu)
{
  // первоначальная настройка экрана  
   
	int butoon_Y = 10;
	int butoon_Y1 = 10;
	int butoon_Height = 40;
	screenButtons->setTextFont(SCREEN_BIG_FONT);

	screenButtons->addButton(5, butoon_Y, 235, butoon_Height, "Калибр. 0% О2");
	butoon_Y += (butoon_Y1 + butoon_Height);

	screenButtons->addButton(5, butoon_Y, 235, butoon_Height, "Калибр. 29,9%");
	butoon_Y += (butoon_Y1 + butoon_Height);

	screenButtons->addButton(5, butoon_Y, 235, butoon_Height, "Время измерен.");
	butoon_Y += (butoon_Y1 + butoon_Height);

	screenButtons->addButton(5, butoon_Y, 235, butoon_Height, "Время калибр.");
	butoon_Y += butoon_Y1 + butoon_Height;

	screenButtons->addButton(5, butoon_Y, 235, butoon_Height, "СИСТЕМА");

    screenButtons->addButton(5, 270, 235, 45, "ВЫХОД");
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::doUpdate(TFTMenu* menu)
{
  if(!isActive())
    return;   
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::doDraw(TFTMenu* menu)
{




 // drawGUI(menu); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::drawGUI(TFTMenu* menu)
{
  menu->clearScreen();
  
  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeightSmall = menu->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidthSmall = menu->getFontWidth(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  uint16_t screenWidth = menu->getScreenWidth();
  uint16_t screenHeight = menu->getScreenHeight();

  menu->print("Интервал, мин:", drawX, drawY);

  drawY += fontHeightSmall + 8;


  
  menu->setFont(SmallRusFont);
  uint8_t fontHeightBig = menu->getFontHeight(SmallRusFont);
  uint8_t fontWidthBig = menu->getFontWidth(SmallRusFont);

  /*menu->setFont(MediumNumbers);
  uint8_t fontHeightBig = menu->getFontHeight(MediumNumbers);
  uint8_t fontWidthBig = menu->getFontWidth(MediumNumbers);*/

  // получаем текущий интервал
  String strInterval = String(Settings.getLoggingInterval());

  // вычисляем, с какой позиции выводить интервал, чтобы вывести его по центру экрана
  drawX = (screenWidth - (strInterval.length()*fontWidthBig))/2;
  menu->print(strInterval.c_str(), drawX, drawY);

  drawY = screenHeight - fontHeightSmall;

  // рисуем статус логгирования
  menu->setFont(SCREEN_SMALL_FONT);

  String logCaption;

  if(Settings.isLoggingEnabled())
  {
    // логгирование активно
    logCaption = "Лог активен";    
  }
  else
  {
    // логгирование неактивно
    logCaption = "Лог неактивен";
  }

  int strLen = menu->print(logCaption.c_str(),0,0,0,true);
  drawX = (screenWidth - strLen*fontWidthSmall)/2;

  menu->print(logCaption.c_str(), drawX, drawY);

  menu->updateDisplay();
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen1::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обрабатываем нажатия на кнопки

   // Для TFT-экрана кнопки начинаются с нуля
  
    if (pressedButton == 0)
    {
      menu->switchToScreen("MenuScreen2"); // переключаемся на экран настроек
    }   
	if (pressedButton == 1)
	{
		//menu->switchToScreen("Main"); // переключаемся на экран настроек
	}

	if (pressedButton == 2)
	{
		//menu->switchToScreen("Main"); // переключаемся на экран настроек
	}

	if (pressedButton == 3)
	{
		menu->switchToScreen("ExportLogsScreen"); // переключаемся на экран настроек
	}
	if (pressedButton == 4)
	{
		menu->switchToScreen("ExportToSerialScreen"); // переключаемся на экран настроек
	}
	if (pressedButton == 5)
	{
		menu->switchToScreen("Main"); // переключаемся на экран настроек
	}

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ExportToSerialScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportToSerialScreen* exportToSerialScreen = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportToSerialScreen::ExportToSerialScreen() : AbstractTFTScreen("ExportToSerialScreen")
{
  exportToSerialScreen = this;
  selectedFile = NULL;
  isExportDone = false;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::startExport()
{
  isExportDone = false;
  
  if(!selectedFile)
  {
    isExportDone = true;
    return;
  }

  // тут вывод файла в Serial
  String fileName = LOGS_DIRECTORY;
  fileName += '/';
  fileName += selectedFile->getName(LOGS_DIRECTORY);
  FileUtils::SendToStream(&Serial,fileName);
  // вывод в Serial закончен



  // вызываем окошко c сообщением
  Vector<const char*> lines;
  lines.push_back("Экспорт файла");
  lines.push_back("завершен.");
  lines.push_back("");
  lines.push_back("Любая кнопка");
  lines.push_back("для выхода.");

  MessageBox->show(lines,"ExportLogsScreen");
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::setFile(FileEntry* entry)
{
  selectedFile = entry;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onActivate()
{
  // мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::doSetup(TFTMenu* menu)
{
  // первоначальная настройка экрана


  screenButtons->addButton(5, 270, 190, 45, "ПРОБА 3");
  // screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::doUpdate(TFTMenu* menu)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::doDraw(TFTMenu* menu)
{

  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);

  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);

  menu->print("Идёт экспорт", 0, 0);
  menu->print("файла, ждите...", 0, fontHeight + 2);

  startExport();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportToSerialScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обрабатываем нажатия на кнопки


  // Для TFT-экрана кнопки начинаются с нуля
  
  if (pressedButton == 0)
  {
  menu->switchToScreen("Main"); // переключаемся на экран настроек
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ExportLogsScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportLogsScreen* exportLogsScreen = NULL;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ExportLogsScreen::ExportLogsScreen() : AbstractTFTScreen("ExportLogsScreen")
{
  exportLogsScreen = this;
  totalFilesCount = 0;
  files = NULL;
  selectedFileNumber = 0;
  currentPage = 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::selectFile(TFTMenu* menu, int8_t step)
{
  if(!files || !totalFilesCount) // нечего рисовать
    return;
    
  // прибавляем смещение
  selectedFileNumber += step;

  // проверяем попадание в границы диапазона
  if(selectedFileNumber < 0)
    selectedFileNumber = 0;

  if(selectedFileNumber >= totalFilesCount)
    selectedFileNumber = totalFilesCount - 1;
  
  // рисуем файлы текущей страницы
  drawGUI(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::drawFiles(TFTMenu* menu)
{
  if(!files || !totalFilesCount) // нечего рисовать
    return;


  int fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
  int lineSpacing = 2;

   // вычисляем общее кол-во страниц
  int totalPages = totalFilesCount/SCREEN_FILES_COUNT;
  if(totalFilesCount % SCREEN_FILES_COUNT)
    totalPages++;

  // вычисляем текущую страницу, на которой находится выбранный файл
  currentPage = selectedFileNumber/SCREEN_FILES_COUNT;
        
  // выбираем диапазон, с которого нам выводить файлы
  int startIndex = currentPage*SCREEN_FILES_COUNT;
  int endIndex = startIndex + SCREEN_FILES_COUNT;

  if(endIndex > totalFilesCount)
    endIndex = totalFilesCount;
  
  // выводим файлы одной страницы, отмечая выбранный галочкой
  String lineToDraw;
  int drawX = 0;
  int drawY = fontHeight + lineSpacing;
  
  for(int i=startIndex;i<endIndex;i++)
  {
      FileEntry* entry = files[i];

      if(i == selectedFileNumber)
        lineToDraw = char(0x10); // толстая стрелка вправо
      else
        lineToDraw = ' ';

      lineToDraw += entry->getName(LOGS_DIRECTORY);

      menu->print(lineToDraw.c_str(), drawX, drawY);
      drawY += fontHeight + lineSpacing;
      
      /*
      filesNames[buttonCounter] = entry->getName(linkedDir);
      
      filesButtons->relabelButton(buttonCounter,filesNames[buttonCounter].c_str());
      filesButtons->showButton(buttonCounter,isActive());
      
      buttonCounter++;
      */
    } // for  
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::rescanFiles()
{
  if(!hasSD)
    return;

   int lastFilesCount = totalFilesCount;
   String dirName = LOGS_DIRECTORY;
   totalFilesCount = FileUtils::CountFiles(dirName);

   if(lastFilesCount != totalFilesCount)
   {
    clearFiles();

    files = new FileEntry*[totalFilesCount];

    SdFile file, root;
    
    for(int i=0;i<totalFilesCount;i++)
    {
      files[i] = new  FileEntry;
    } // for

    root.open(dirName.c_str(),O_READ);

    int cntr = 0;
    while (file.openNext(&root, O_READ)) 
    {
      if(cntr < totalFilesCount)
      {
        files[cntr]->dirIndex = file.dirIndex();
      }
      
      file.close();
      cntr++;
    }      
    root.close();

     
   } // if(lastFilesCount != totalFilesCount)
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::clearFiles()
{
  if(!files)
    return;
    
  for(int i=0;i<totalFilesCount;i++)
    delete files[i];

  delete [] files;
  files = NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::setMode(ExportMode mode)
{
  exportMode = mode;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onActivate()
{
  // мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::doSetup(TFTMenu* menu)
{
  // первоначальная настройка экрана

  // добавляем экран экспорта в Serial
  menu->addScreen(ExportToSerialScreen::create());

  //TODO: ТУТ ДОБАВЛЯТЬ ЭКРАНЫ ЭКСПОРТА по WiFi И НА ПРИНТЕР!!!

  hasSD = SDInit::InitSD();

  screenButtons->addButton(5, 270, 190, 45, "ПРОБА 4");
  // screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::doUpdate(TFTMenu* menu)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::drawGUI(TFTMenu* menu)
{
  menu->clearScreen();
  
  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);

  String header = "";
    
  switch(exportMode)
  {
    case exportToSerial:
      header += "В COM-порт";
    break;
    
    case exportToWiFi:
      header += "По WiFi";
    break;

    case exportToPrinter:
      header += "На принтер";
    break;
    
  }
  menu->print(header.c_str(), 0, 0);

  // тестовое кол-во файлов в папке логов
  //menu->print(String(totalFilesCount).c_str(),0,20);

  
  drawFiles(menu);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::doDraw(TFTMenu* menu)
{
  drawGUI(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::exportSelectedFile(TFTMenu* menu)
{
  if(!files || !totalFilesCount) // нечего выводить
    return;

  // получаем выбранный файл
  FileEntry* entry = files[selectedFileNumber];
    
  switch(exportMode)
  {
    case exportToSerial:
    {
      exportToSerialScreen->setFile(entry);
      menu->switchToScreen(exportToSerialScreen);
    }
    break;

    case exportToWiFi:
    {
      //TODO: Тут вызов экрана экспорта по WiFi !!!
    }
    break;
    
    case exportToPrinter:
    {
      //TODO: Тут вызов экрана экспорта на принтер !!!      
    }
    break;
    
  } // switch
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExportLogsScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обрабатываем нажатия на кнопки


  // Для TFT-экрана кнопки начинаются с нуля
  
  if (pressedButton == 0)
  {
  menu->switchToScreen("Main"); // переключаемся на экран настроек
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen2
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen2::MenuScreen2() : AbstractTFTScreen("MenuScreen2")
{
  /*
  ignoreKeys = false;
  exportActive = false;
  drawMode = dmStartScreen;
  */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doSetup(TFTMenu* menu)
{
	// первоначальная настройка экрана

  // добавляем экран вывода списка файлов для экспорта
  menu->addScreen(ExportLogsScreen::create());


	screenButtons->addButton(5, 270, 190, 45, "ПРОБА 5");
	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doUpdate(TFTMenu* menu)
{
	if (!isActive())
		return;

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::drawStartScreen(TFTMenu* menu)
{
  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  menu->print("Экспорт:", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 1 - Serial", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 2 - WiFi", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 3 - Принтер", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 4 - Выход", drawX, drawY);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::drawGUI(TFTMenu* menu)
{
  menu->clearScreen();
  
  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);


  drawStartScreen(menu);
 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::doDraw(TFTMenu* menu)
{
  drawGUI(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen2::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	// обрабатываем нажатия на кнопки



	// Для TFT-экрана кнопки начинаются с нуля
	
	if (pressedButton == 0)
	{
	menu->switchToScreen("Main"); // переключаемся на экран настроек
	}
	
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// EditBorderScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
EditBorderScreen::EditBorderScreen(const char* screenName, WhichBorder b) : AbstractTFTScreen(screenName)
{
  whichBorder = b;
  editedVal = 0;
  changeStep = 1;
  caption = "";
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onDeactivate()
{
  // станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onActivate()
{
  // мы активизируемся
  reloadValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::reloadValues()
{
  switch(whichBorder)
  {
    case temperatureBorder:
    {
      minVal = TEMPERATURE_MIN_BORDER;
      maxVal = TEMPERATURE_MAX_BORDER;
      changeStep = TEMPERATURE_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinTempBorder();
      currentMaxVal = Settings.getMaxTempBorder();
      caption = "Порог темп-ры";
    }
    break;

    case humidityBorder:
    {
      minVal = HUMIDITY_MIN_BORDER;
      maxVal = HUMIDITY_MAX_BORDER;
      changeStep = HUMIDITY_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinHumidityBorder();
      currentMaxVal = Settings.getMaxHumidityBorder();
      caption = "Порог влажн.";
    }
    break;

    case adcBorder:
    {
      minVal = ADC_MIN_BORDER;
      maxVal = ADC_MAX_BORDER;
      changeStep = ADC_BORDER_CHANGE_STEP;
      currentMinVal = Settings.getMinADCBorder();
      currentMaxVal = Settings.getMaxADCBorder();
      caption = "Порог АЦП";
    }
    break;
    
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doSetup(TFTMenu* menu)
{
  // первоначальная настройка экрана
  reloadValues();


   screenButtons->addButton(5, 270, 190, 45, "ПРОБА 6");
  // screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::saveValues()
{
 switch(whichBorder)
  {
    case temperatureBorder:
    {
      Settings.setMinTempBorder(currentMinVal);
      Settings.setMaxTempBorder(currentMaxVal);
    }
    break;

    case humidityBorder:
    {
      Settings.setMinHumidityBorder(currentMinVal);
      Settings.setMaxHumidityBorder(currentMaxVal);
    }
    break;

    case adcBorder:
    {
      Settings.setMinADCBorder(currentMinVal);
      Settings.setMaxADCBorder(currentMaxVal);
    }
    break;
    
  }  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doUpdate(TFTMenu* menu)
{
  if (!isActive())
    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::drawGUI(TFTMenu* menu)
{
  menu->setFont(SCREEN_SMALL_FONT);
  menu->setColor(SCREEN_TEXT_COLOR);
  
  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);
  uint8_t fontWidth = menu->getFontWidth(SCREEN_SMALL_FONT);
  int screenWidth = menu->getScreenWidth();
  int screenHeight = menu->getScreenHeight();
  
  int drawX = 0, drawY = 0, spacing = 2;

  menu->print(caption, drawX, drawY);

  drawY += fontHeight*2 + spacing*4;
  drawX = spacing;

  const int MAX_VAL_LEN = 5;

  // считаем, что у нас на каждое значение - максимум 4 символа
  int boxContentWidth = MAX_VAL_LEN*fontWidth;

  // рисуем левый бокс с текущим значением
  String toDraw;
  
  toDraw = currentMinVal;
  while(toDraw.length() < MAX_VAL_LEN)
    toDraw += ' ';

  menu->print("min", drawX, drawY - fontHeight - spacing);
  menu->print(toDraw.c_str(),drawX, drawY);

  // если левый бокс - текущий, выделяем его
  if(editedVal == 0)
  {
    menu->drawRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  }
  else
    menu->clrRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  

  // теперь вычисляем позицию по X для правого значения
  drawX = screenWidth - boxContentWidth - spacing*2;

  // рисуем правый бокс
  toDraw = currentMaxVal;
  while(toDraw.length() < MAX_VAL_LEN)
    toDraw += ' ';

  menu->print("max", drawX, drawY - fontHeight - spacing);
  menu->print(toDraw.c_str(),drawX, drawY);

  // если правый бокс - текущий, то рисуем вокруг него рамку
  if(editedVal == 1)
  {
    menu->drawRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);
  }
  else
    menu->clrRoundRect(drawX - spacing, drawY - spacing, drawX + boxContentWidth + spacing, drawY + fontHeight);


  menu->print("3 - выбор",0, screenHeight - fontHeight);

  menu->updateDisplay();
   
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::doDraw(TFTMenu* menu)
{
  drawGUI(menu);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EditBorderScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
  // обрабатываем нажатия на кнопки

  // Для TFT-экрана кнопки начинаются с нуля
  
  if (pressedButton == 0)
  {
  menu->switchToScreen("Main"); // переключаемся на экран настроек
  }
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// BordersScreen
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BordersScreen::BordersScreen() : AbstractTFTScreen("BordersScreen")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doSetup(TFTMenu* menu)
{
	// первоначальная настройка экрана

	screenButtons->addButton(5, 275, 190, 40, "ПРОБА 7");
	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doUpdate(TFTMenu* menu)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::doDraw(TFTMenu* menu)
{

	menu->setFont(SCREEN_SMALL_FONT);
	menu->setColor(SCREEN_TEXT_COLOR);
  uint8_t fontHeight = menu->getFontHeight(SCREEN_SMALL_FONT);

  int drawX = 0, drawY = 0;

  menu->print("Пороги:", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 1 - Темп-тура", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 2 - Влажность", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 3 - АЦП", drawX, drawY);

  drawY += fontHeight + 2;  
  menu->print(" 4 - Выход", drawX, drawY);	

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BordersScreen::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	// обрабатываем нажатия на кнопки
	// Для TFT-экрана кнопки начинаются с нуля
	
	if (pressedButton == 0)
	{
	menu->switchToScreen("Main"); // переключаемся на экран настроек
	}
	
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MenuScreen4
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MenuScreen4::MenuScreen4() : AbstractTFTScreen("MenuScreen4")
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onDeactivate()
{
	// станем неактивными

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onActivate()
{
	// мы активизируемся

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::doSetup(TFTMenu* menu)
{
	// первоначальная настройка экрана

	screenButtons->addButton(5, 275, 190, 40, "ПРОБА 8");
	// screenButtons->addButton(200, 275, 35, 40, "z", BUTTON_SYMBOL); // кнопка Часы 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::doUpdate(TFTMenu* menu)
{
	if (!isActive())
		return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::doDraw(TFTMenu* menu)
{

	menu->setFont(SCREEN_SMALL_FONT);
	menu->setColor(SCREEN_TEXT_COLOR);

	menu->print("Экран #4", 0, 0);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MenuScreen4::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	// обрабатываем нажатия на кнопки

	// Для TFT-экрана кнопки начинаются с нуля
	
	if (pressedButton == 0)
	{
	menu->switchToScreen("Main"); // переключаемся на экран настроек
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
