//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Screen5.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Screen5::Screen5() : AbstractTFTScreen("DateSettings")
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen5::doSetup(TFTMenu* menu)
{
	// тут настраиваемся, например, можем добавлять кнопки

	int menu_height = 30;
	int button_gap = 15;
	int button_time_gap = 45;

	screenButtons->setSymbolFont(Various_Symbols_16x32);

	screenButtons->addButton(button_time_gap, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "c",BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME,"c", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2),  menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "c", BUTTON_SYMBOL);
	menu_height += BUTTON_HEIGHT_TIME + button_gap +30;

	screenButtons->addButton(button_time_gap, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + (button_gap *2)+ (BUTTON_HEIGHT_TIME *2), menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);

	//menu_height += BUTTON_HEIGHT_TIME + button_gap + 30;
	//button_time_gap = 85;

	screenButtons->setTextFont(SCREEN_BIG_FONT);
	screenButtons->addButton(5, 215, 230, 45, "СОХРАНИТЬ"); // кнопка Часы 
	screenButtons->addButton(5, 270, 230, 45, "ВЫХОД");


}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen5::doUpdate(TFTMenu* menu)
{
    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen5::doDraw(TFTMenu* menu)
{
  UTFT* dc = menu->getDC();
  dc->setColor(VGA_WHITE);
  dc->setBackColor(VGA_BLACK);
  dc->setFont(BigRusFont);
 
 // получаем текущую дату
  DS3231Time dt = RealtimeClock.getTime();

  t_temp_date = dt.dayOfMonth;
  t_temp_mon = dt.month;
  t_temp_year = dt.year;
  t_temp_dow = dt.dayOfWeek;
  t_temp_hour = dt.hour;
  t_temp_min = dt.minute;
  t_temp_sec = dt.second;

  if (t_temp_date == 0)
  {
	  t_temp_date = 10;
	  t_temp_mon = 1;
	  t_temp_year = 2018;
	  t_temp_dow = 2;
	  t_temp_hour = 9;
	  t_temp_min = 0;
	  t_temp_sec = 0;
  }

  int menu_height = 30;
  int button_gap = 15;
  int button_time_gap =50;
  menu_height += BUTTON_HEIGHT_TIME + button_gap;

  if (t_temp_hour<10)
  {
	  dc->printNumI(0, button_time_gap, menu_height);
	  dc->printNumI(t_temp_hour, button_time_gap+16, menu_height);
  }
  else
  {
	  dc->printNumI(t_temp_hour, button_time_gap, menu_height);
  }
  if (t_temp_min<10)
  {
	  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME+16, menu_height);
  }
  else
  {
	  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
  }
  if (t_temp_sec<10)
  {
	  dc->printNumI(0, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
	  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)+16, menu_height);
  }
  else
  {
	  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
  }
  
  dc->setFont(SmallRusFont);
  // тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
  menu->print("УСТАНОВКА ВРЕМЕНИ", 53, 5);
  dc->setFont(BigRusFont);
  menu->print("ВРЕМЯ", 80, 170);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen5::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	UTFT* dc = menu->getDC();
	dc->setColor(VGA_WHITE);
	dc->setBackColor(VGA_BLACK);
	int menu_height = 30;
	int button_gap = 15;
	int button_time_gap = 50;
	menu_height += BUTTON_HEIGHT_TIME + button_gap;
  // обработчик нажатия на кнопку. Номера кнопок начинаются с 0 и идут в том порядке, в котором мы их добавляли
  if(pressedButton == 7)
    menu->switchToScreen("RTCSettings"); // переключаемся на первый экран
  else if (pressedButton == 6)
  {
	  RealtimeClock.setTime(t_temp_sec, t_temp_min, t_temp_hour, t_temp_dow, t_temp_date, t_temp_mon, t_temp_year);
  }
  else if (pressedButton == 0)
  {
	  t_temp_hour += 1;
	  if (t_temp_hour == 24)
		  t_temp_hour = 0;
	  if (t_temp_hour<10)
	  {
		  dc->printNumI(0, button_time_gap, menu_height);
		  dc->printNumI(t_temp_hour, button_time_gap+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_hour, button_time_gap, menu_height);
	  }
  }
  else if (pressedButton == 1)
  {
	  t_temp_min += 1;
	  if (t_temp_min == 60)
		  t_temp_min = 0;
	  if (t_temp_min<10)
	  {
		  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
		  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  }
  }
  else if (pressedButton == 2)
  {
	  t_temp_sec += 1;
	  if (t_temp_sec == 60)
		  t_temp_sec = 0;
	  if (t_temp_sec<10)
	  {
		  dc->printNumI(0, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
		  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
	  }
  }
  else if (pressedButton == 3)
  {
	  t_temp_hour -= 1;
	  if (t_temp_hour < 0)
		  t_temp_hour = 23;
	  if (t_temp_hour<10)
	  {
		  dc->printNumI(0, button_time_gap, menu_height);
		  dc->printNumI(t_temp_hour, button_time_gap+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_hour, button_time_gap, menu_height);
	  }
  }
  else if (pressedButton == 4)
  {
	  t_temp_min -= 1;
	  if (t_temp_min < 0)
		  t_temp_min = 59;
	  if (t_temp_min<10)
	  {
		  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
		  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_min, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  }
  }
  else if (pressedButton == 5)
  {
	  t_temp_sec -= 1;
	  if (t_temp_sec < 0)
		  t_temp_sec = 59;
	  if (t_temp_sec<10)
	  {
		  dc->printNumI(0, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
		  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)+16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_sec, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height);
	  }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
