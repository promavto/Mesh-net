//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Screen6.h"
#include "DS3231.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Screen6::Screen6() : AbstractTFTScreen("TimeSettings")
{
  
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen6::doSetup(TFTMenu* menu)
{
	// тут настраиваемся, например, можем добавлять кнопки

	int menu_height = 30;
	int button_gap = 15;
	int button_time_gap = 50;

	screenButtons->setSymbolFont(Various_Symbols_16x32);

	screenButtons->addButton(button_time_gap, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "c", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "c", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "c", BUTTON_SYMBOL);
	menu_height += BUTTON_HEIGHT_TIME + button_gap + 30;

	screenButtons->addButton(button_time_gap, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);
	screenButtons->addButton(button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2), menu_height, BUTTON_HEIGHT_TIME, BUTTON_HEIGHT_TIME, "d", BUTTON_SYMBOL);

	//menu_height += BUTTON_HEIGHT_TIME + button_gap + 30;
	//button_time_gap = 85;

	screenButtons->setTextFont(SCREEN_BIG_FONT);
	screenButtons->addButton(5, 215, 230, 45, "СОХРАНИТЬ"); // кнопка Часы 
	screenButtons->addButton(5, 270, 230, 45, "ВЫХОД");
	
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen6::doUpdate(TFTMenu* menu)
{
    // тут обновляем внутреннее состояние
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen6::doDraw(TFTMenu* menu)
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
  int button_time_gap = 55;
  menu_height += BUTTON_HEIGHT_TIME + button_gap;

  if (t_temp_date < 10)
  {
	  dc->printNumI(0, button_time_gap, menu_height);
	  dc->printNumI(t_temp_date, button_time_gap + 16, menu_height);
  }
  else
  {
	  dc->printNumI(t_temp_date, button_time_gap, menu_height);
  }
  if (t_temp_mon < 10)
  {
	  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME + 16, menu_height);
  }
  else
  {
	  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
  }

	  dc->printNumI(t_temp_year, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)-15, menu_height);


  // тут рисуем, что надо именно нам, кнопки прорисуются сами после того, как мы тут всё отрисуем
	  dc->setFont(SmallRusFont);
      menu->print("УСТАНОВКА ДАТЫ", 72, 5);
      dc->setFont(BigRusFont);
      menu->print("ДАТА", 93, 170);

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Screen6::onButtonPressed(TFTMenu* menu, int pressedButton)
{
	UTFT* dc = menu->getDC();
	dc->setColor(VGA_WHITE);
	dc->setBackColor(VGA_BLACK);
	screenButtons->setTextFont(SCREEN_BIG_FONT);
	int menu_height = 30;
	int button_gap = 15;
	int button_time_gap = 55;
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
	  t_temp_date += 1;
	  t_temp_date = validateDate(t_temp_date, t_temp_mon, t_temp_year);
	  if (t_temp_date<10)
	  {
		  dc->printNumI(0, button_time_gap, menu_height);
		  dc->printNumI(t_temp_date, button_time_gap + 16, menu_height);
	  }
	  else
	  {
		   dc->printNumI(t_temp_date, button_time_gap, menu_height);
	  }
  }
  else if (pressedButton == 1)
  {
	  t_temp_mon += 1;
	  if (t_temp_mon == 13)
		  t_temp_mon = 1;
	  if (t_temp_mon<10)
	  {
		  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
		  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME + 16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  }
  }
  else if (pressedButton == 2)
  {
	  t_temp_year += 1;
	  if (t_temp_year >2099)
		  t_temp_year = 2099;
	   dc->printNumI(t_temp_year - 2000, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)+16, menu_height);
  }
  else if (pressedButton == 3)
  {
	  t_temp_date -= 1;
	  t_temp_date = validateDate(t_temp_date, t_temp_mon, t_temp_year);
	  if (t_temp_date<10)
	  {
		  dc->printNumI(0, button_time_gap, menu_height);
		  dc->printNumI(t_temp_date, button_time_gap + 16, menu_height);
	  }
	  else
	  {
		 dc->printNumI(t_temp_date, button_time_gap, menu_height);
	  }

  }
  else if (pressedButton == 4)
  {
	  t_temp_mon -= 1;
	  if (t_temp_mon == 0)
		  t_temp_mon = 12;
	  if (t_temp_mon<10)
	  {
		  dc->printNumI(0, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
		  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME + 16, menu_height);
	  }
	  else
	  {
		  dc->printNumI(t_temp_mon, button_time_gap + button_gap + BUTTON_HEIGHT_TIME, menu_height);
	  }
  }
  else if (pressedButton == 5)
  {
	  t_temp_year -= 1;
	  if (t_temp_year < 2017)
	  t_temp_year = 2017;
      dc->printNumI(t_temp_year - 2000, button_time_gap + (button_gap * 2) + (BUTTON_HEIGHT_TIME * 2)+16, menu_height);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte  Screen6::validateDate(byte d, byte m, word y)
{
	byte mArr[12] = { 31,0,31,30,31,30,31,31,30,31,30,31 };
	byte od;

	if (m == 2)
	{
		if ((y % 4) == 0)
		{
			if (d == 30)
				od = 1;
			else if (d == 0)
				od = 29;
			else
				od = d;
		}
		else
		{
			if (d == 29)
				od = 1;
			else if (d == 0)
				od = 28;
			else
				od = d;
		}
	}
	else
	{
		if (d == 0)
			od = mArr[m - 1];
		else if (d == (mArr[m - 1] + 1))
			od = 1;
		else
			od = d;
	}

	return od;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte  Screen6::validateDateForMonth(byte d, byte m, word y)
{
	byte mArr[12] = { 31,0,31,30,31,30,31,31,30,31,30,31 };
	byte od;

	if (m == 2)
	{
		if ((y % 4) == 0)
		{
			if (d>29)
			{
				d = 29;
			}
		}
		else
		{
			if (d>28)
			{
				d = 28;
			}
		}
	}
	else
	{
		if (d>mArr[m - 1])
		{
			d = mArr[m - 1];
		}
	}

	return d;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

