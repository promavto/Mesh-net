


///*
// * Hardware:
// * - TFT LCD (SD card + touch) using ILI9341 via 8bit parallel interface: http://www.lcdwiki.com/3.2inch_16BIT_Module_ILI9341_SKU:MRB3205
// * - ESP-WROOM-32 dev Board
// *
// * Wiring: just follow the pin definitios below
// * NOTE: In order to make everything work you HAVE to solder the SMD resistor (actually it's a jumper) in 8bit position.
// * */
//
#include <URTouch_ILI9341.h>      //https://github.com/PaulStoffregen/XPT2046_Touchscreen
#include <SPI.h>                  //https://github.com/espressif/arduino-esp32/tree/master/libraries/SPI
#include <TFT_eSPI.h>             // https://github.com/Bodmer/TFT_eSPI



 //VSPI
#define CS_PIN 13
#define IRQ_PIN 14
#define MISO 19
#define MOSI 23
#define SCK 18

#define TFT_W 320
#define TFT_H 240

URTouch ts(CS_PIN, IRQ_PIN);   // initializes
TFT_eSPI tft = TFT_eSPI();
bool ON_OFF = false;          // flag that indicates the button state
bool canTouchThis = true;     // flag created to avoid switching states while hold the button
float lcd_x = 0;              //holds X touch position
float lcd_y = 0;              //holds Y touch position

#define TFT_GREY 0x7BEF
unsigned long runTime = 0;

void setup(void) {
	Serial.begin(115200);
	randomSeed(analogRead(A0));
	tft.init();
	ts.begin();
	tft.setRotation(1);
	ts.setRotation(3);
	while (!Serial && (millis() <= 1000));
	drawScreen("Initiated...", ILI9341_WHITE, ILI9341_DARKGREY);
}

void loop() 
{

	if (ts.tirqTouched()) 
	{
		if (ts.touched()) 
		{
			TS_Point p = ts.getPoint();
			//lcd_x = p.x/12.8;
			lcd_x = p.x / (4096 / TFT_W); // calculates the point based on screen width
			//lcd_y = p.y/17.06;
			lcd_y = p.y / (4096 / TFT_H); // calculates the point based on screen height
			if ((lcd_x > 80 and lcd_x < 239) and (lcd_y > 70 and lcd_y < 169) and canTouchThis) 
			{
				if (ON_OFF) 
				{
					drawScreen("[OFF]", ILI9341_WHITE, ILI9341_RED);
				}
				else 
				{
					drawScreen("[ON]", ILI9341_BLACK, ILI9341_GREEN);
				}
				ON_OFF = !ON_OFF; //switch states
				canTouchThis = false;
			}
			Serial.print("(x,y) = (");
			Serial.print(lcd_x);
			Serial.print(",");
			Serial.print(lcd_y);
			Serial.print(")");
			Serial.println();
		}
	}
	else 
	{
		canTouchThis = true; 
		testTFT();
	}
	delay(50); // delay used to make things slower in serial output
}

void drawScreen(String s, uint16_t color, uint16_t bg) {
	tft.fillScreen(ILI9341_BLACK);

	tft.fillRoundRect(80, 70, 239 - 80, 169 - 70, 3, bg);
	//tft.setTextColor(ILI9341_WHITE,TFT_DARK_GREY);
	tft.setTextColor(color);
	tft.drawCentreString(s, 160, 112, 4);

	tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
	tft.drawCentreString("Under development...", 160, 210, 2);
}

//Use native colors
//ILI9341_BLACK       0x0000
//ILI9341_NAVY        0x000F
//ILI9341_DARKGREEN   0x03E0
//ILI9341_DARKCYAN    0x03EF
//ILI9341_MAROON      0x7800
//ILI9341_PURPLE      0x780F
//ILI9341_OLIVE       0x7BE0
//ILI9341_LIGHTGREY   0xC618
//ILI9341_DARKGREY    0x7BEF
//ILI9341_BLUE        0x001F
//ILI9341_GREEN       0x07E0
//ILI9341_CYAN        0x07FF
//ILI9341_RED         0xF800
//ILI9341_MAGENTA     0xF81F
//ILI9341_YELLOW      0xFFE0
//ILI9341_WHITE       0xFFFF
//ILI9341_ORANGE      0xFD20
//ILI9341_GREENYELLOW 0xAFE5
//ILI9341_PINK        0xF81F

void testTFT()
{
	randomSeed(millis());
	//randomSeed(1234); // This ensure test is repeatable with exact same draws each loop
	int buf[318];
	int x, x2;
	int y, y2;
	int r;
	runTime = millis();
	// Clear the screen and draw the frame
	tft.fillScreen(TFT_BLACK);


	tft.fillRect(0, 0, 319, 14, TFT_RED);

	tft.fillRect(0, 226, 319, 14, TFT_GREY);

	tft.setTextColor(TFT_BLACK, TFT_RED);
	tft.drawCentreString("* TFT_eSPI *", 160, 4, 1);
	tft.setTextColor(TFT_YELLOW, TFT_GREY);
	tft.drawCentreString("Adapted by Bodmer", 160, 228, 1);

	tft.drawRect(0, 14, 319, 211, TFT_BLUE);

	// Draw crosshairs
	tft.drawLine(159, 15, 159, 224, TFT_BLUE);
	tft.drawLine(1, 119, 318, 119, TFT_BLUE);
	for (int i = 9; i < 310; i += 10)
		tft.drawLine(i, 117, i, 121, TFT_BLUE);
	for (int i = 19; i < 220; i += 10)
		tft.drawLine(157, i, 161, i, TFT_BLUE);

	// Draw sin-, cos- and tan-lines  
	tft.setTextColor(TFT_CYAN);
	tft.drawString("Sin", 5, 15, 2);
	for (int i = 1; i < 318; i++)
	{
		tft.drawPixel(i, 119 + (sin(((i*1.13)*3.14) / 180) * 95), TFT_CYAN);
	}
	tft.setTextColor(TFT_RED);
	tft.drawString("Cos", 5, 30, 2);
	for (int i = 1; i < 318; i++)
	{
		tft.drawPixel(i, 119 + (cos(((i*1.13)*3.14) / 180) * 95), TFT_RED);
	}
	tft.setTextColor(TFT_YELLOW);
	tft.drawString("Tan", 5, 45, 2);
	for (int i = 1; i < 318; i++)
	{
		tft.drawPixel(i, 119 + (tan(((i*1.13)*3.14) / 180)), TFT_YELLOW);
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	tft.drawLine(159, 15, 159, 224, TFT_BLUE);
	tft.drawLine(1, 119, 318, 119, TFT_BLUE);
	int col = 0;
	// Draw a moving sinewave
	x = 1;
	for (int i = 1; i < (317 * 20); i++)
	{
		x++;
		if (x == 318)
			x = 1;
		if (i > 318)
		{
			if ((x == 159) || (buf[x - 1] == 119))
				col = TFT_BLUE;
			else
				tft.drawPixel(x, buf[x - 1], TFT_BLACK);
		}
		y = 119 + (sin(((i*1.1)*3.14) / 180)*(90 - (i / 100)));
		tft.drawPixel(x, y, TFT_BLUE);
		buf[x - 1] = y;
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some filled rectangles
	for (int i = 1; i < 6; i++)
	{
		switch (i)
		{
		case 1:
			col = TFT_MAGENTA;
			break;
		case 2:
			col = TFT_RED;
			break;
		case 3:
			col = TFT_GREEN;
			break;
		case 4:
			col = TFT_BLUE;
			break;
		case 5:
			col = TFT_YELLOW;
			break;
		}
		tft.fillRect(70 + (i * 20), 30 + (i * 20), 60, 60, col);
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some filled, rounded rectangles
	for (int i = 1; i < 6; i++)
	{
		switch (i)
		{
		case 1:
			col = TFT_MAGENTA;
			break;
		case 2:
			col = TFT_RED;
			break;
		case 3:
			col = TFT_GREEN;
			break;
		case 4:
			col = TFT_BLUE;
			break;
		case 5:
			col = TFT_YELLOW;
			break;
		}
		tft.fillRoundRect(190 - (i * 20), 30 + (i * 20), 60, 60, 3, col);
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some filled circles
	for (int i = 1; i < 6; i++)
	{
		switch (i)
		{
		case 1:
			col = TFT_MAGENTA;
			break;
		case 2:
			col = TFT_RED;
			break;
		case 3:
			col = TFT_GREEN;
			break;
		case 4:
			col = TFT_BLUE;
			break;
		case 5:
			col = TFT_YELLOW;
			break;
		}
		tft.fillCircle(100 + (i * 20), 60 + (i * 20), 30, col);
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some lines in a pattern

	for (int i = 15; i < 224; i += 5)
	{
		tft.drawLine(1, i, (i*1.44) - 10, 223, TFT_RED);
	}

	for (int i = 223; i > 15; i -= 5)
	{
		tft.drawLine(317, i, (i*1.44) - 11, 15, TFT_RED);
	}

	for (int i = 223; i > 15; i -= 5)
	{
		tft.drawLine(1, i, 331 - (i*1.44), 15, TFT_CYAN);
	}

	for (int i = 15; i < 224; i += 5)
	{
		tft.drawLine(317, i, 330 - (i*1.44), 223, TFT_CYAN);
	}

	delay(0);


	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some random circles
	for (int i = 0; i < 100; i++)
	{
		x = 32 + random(256);
		y = 45 + random(146);
		r = random(30);
		tft.drawCircle(x, y, r, random(0xFFFF));
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some random rectangles
	for (int i = 0; i < 100; i++)
	{
		x = 2 + random(316);
		y = 16 + random(207);
		x2 = 2 + random(316);
		y2 = 16 + random(207);
		if (x2 < x) {
			r = x; x = x2; x2 = r;
		}
		if (y2 < y) {
			r = y; y = y2; y2 = r;
		}
		tft.drawRect(x, y, x2 - x, y2 - y, random(0xFFFF));
	}

	delay(0);


	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// Draw some random rounded rectangles
	for (int i = 0; i < 100; i++)
	{
		x = 2 + random(316);
		y = 16 + random(207);
		x2 = 2 + random(316);
		y2 = 16 + random(207);
		// We need to get the width and height and do some window checking
		if (x2 < x) {
			r = x; x = x2; x2 = r;
		}
		if (y2 < y) {
			r = y; y = y2; y2 = r;
		}
		// We need a minimum size of 6
		if ((x2 - x) < 6) x2 = x + 6;
		if ((y2 - y) < 6) y2 = y + 6;
		tft.drawRoundRect(x, y, x2 - x, y2 - y, 3, random(0xFFFF));
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	//randomSeed(1234);
	int colour = 0;
	for (int i = 0; i < 100; i++)
	{
		x = 2 + random(316);
		y = 16 + random(209);
		x2 = 2 + random(316);
		y2 = 16 + random(209);
		colour = random(0xFFFF);
		tft.drawLine(x, y, x2, y2, colour);
	}

	delay(0);

	tft.fillRect(1, 15, 317, 209, TFT_BLACK);

	// This test has been modified as it takes more time to calculate the random numbers
	// than to draw the pixels (3 seconds to produce 30,000 randoms)!
	for (int i = 0; i < 10000; i++)
	{
		tft.drawPixel(2 + random(316), 16 + random(209), random(0xFFFF));
	}

	// Draw 10,000 pixels to fill a 100x100 pixel box
	// use the coords as the colour to produce the banding
	//byte i = 100;
	//while (i--) {
	//  byte j = 100;
	//  while (j--) tft.drawPixel(i+110,j+70,i+j);
	//  //while (j--) tft.drawPixel(i+110,j+70,0xFFFF);
	//}
	delay(0);

	tft.fillScreen(TFT_BLUE);
	tft.fillRoundRect(80, 70, 239 - 80, 169 - 70, 3, TFT_RED);

	tft.setTextColor(TFT_WHITE, TFT_RED);
	tft.drawCentreString("That's it!", 160, 93, 2);
	tft.drawCentreString("Restarting in a", 160, 119, 2);
	tft.drawCentreString("few seconds...", 160, 132, 2);

	runTime = millis() - runTime;
	tft.setTextColor(TFT_GREEN, TFT_BLUE);
	tft.drawCentreString("Runtime: (msecs)", 160, 210, 2);
	tft.setTextDatum(TC_DATUM);
	tft.drawNumber(runTime, 160, 225, 2);
	//delay(5000);

}






//#include <XPT2046_Touchscreen.h>
//#include <SPI.h>
//
////#define CS_PIN  8
//// MOSI=11, MISO=12, SCK=13
//
////XPT2046_Touchscreen ts(CS_PIN);
////#define TIRQ_PIN  2
//
//
////XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
////XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
//
// //VSPI
//#define CS_PIN 13
//#define IRQ_PIN 14
//#define MISO 19
//#define MOSI 23
//#define SCK 18
//
//
//
//
//XPT2046_Touchscreen ts(CS_PIN, IRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
//
//void setup() {
//	Serial.begin(115200);
//	ts.begin();
//	ts.setRotation(1);
//	while (!Serial && (millis() <= 1000));
//}
//
//void loop() {
//	if (ts.touched()) {
//		TS_Point p = ts.getPoint();
//		Serial.print("Pressure = ");
//		Serial.print(p.z);
//		Serial.print(", x = ");
//		Serial.print(p.x);
//		Serial.print(", y = ");
//		Serial.print(p.y);
//		delay(30);
//		Serial.println();
//	}
//}
//
//
