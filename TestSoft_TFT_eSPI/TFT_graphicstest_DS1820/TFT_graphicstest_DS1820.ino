/*
 Adapted from the Adafruit graphicstest sketch, see original header at end
 of sketch.

 This sketch uses the GLCD font (font 1) only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 ###### Õ≈ «¿¡”ƒ‹“≈ Œ¡ÕŒ¬Àﬂ“‹ ‘¿…À User_Setup.h ¬ ¡»¡À»Œ“≈ ≈ #############
 #########################################################################
*/


#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

unsigned long total = 0;
unsigned long tn = 0;


// Libraries for SD card
#include "FS.h"
#include "SD.h"
//#include "SPI.h"

//DS18B20 libraries
#include "OneWire.h"
#include "DallasTemperature.h"

// Libraries to get time from NTP Server
#include "WiFi.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

// Define deep sleep options
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// Sleep for 10 minutes = 600 seconds
uint64_t TIME_TO_SLEEP = 600;

// Replace with your network credentials
const char* ssid = "ZTE-0FEE34";
const char* password = "panasonic";

// Define CS pin for the SD card module
#define SD_CS 5

// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;

String dataMessage;

// Data wire is connected to ESP32 GPIO 21
#define ONE_WIRE_BUS 26
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Temperature Sensor variables
float temperature;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

bool SwitchOn = false;

// Comment out to stop drawing black spots
//#define BLACK_SPOT

// Switch position and size
#define FRAME_X 100
#define FRAME_Y 195
#define FRAME_W 120
#define FRAME_H 40

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

uint16_t x, y;


#define rele1 32
#define rele2 33

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time time was updated

// constants won't change:
const long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
	Serial.begin(115200);
	while (!Serial);
	pinMode(rele1, OUTPUT);
	pinMode(rele2, OUTPUT);
	digitalWrite(rele1, LOW);
	digitalWrite(rele2, LOW);

	Serial.println(""); Serial.println("");
	Serial.println("TFT_eSPI library test!");

	tft.init();
	tft.setRotation(1);
	tn = micros();

	touch_calibrate(); // ‡ÎË·ÛÂÏ Ó‰ËÌ ‡Á, ‰‡ÌÌ˚Â Í‡ÎË·Ó‚ÍË ÒÓı‡Ìˇ˛ÚÒˇ ‚ Ù‡ÈÎÂ.

	// clear screen
	tft.fillScreen(TFT_BLUE);
	delay(500);
	
	//*****************************************************************************
	yield(); Serial.println(F("Benchmark                Time (microseconds)"));

	yield(); Serial.print(F("Screen fill              "));
	yield(); Serial.println(testFillScreen());
	//total+=testFillScreen();
	//delay(500);

	yield(); Serial.print(F("Text                     "));
	yield(); Serial.println(testText());
	//total+=testText();
	//delay(3000);

	yield(); Serial.print(F("Lines                    "));
	yield(); Serial.println(testLines(TFT_CYAN));
	//total+=testLines(TFT_CYAN);
	//delay(500);

	yield(); Serial.print(F("Horiz/Vert Lines         "));
	yield(); Serial.println(testFastLines(TFT_RED, TFT_BLUE));
	//total+=testFastLines(TFT_RED, TFT_BLUE);
	//delay(500);

	yield(); Serial.print(F("Rectangles (outline)     "));
	yield(); Serial.println(testRects(TFT_GREEN));
	//total+=testRects(TFT_GREEN);
	//delay(500);

	yield(); Serial.print(F("Rectangles (filled)      "));
	yield(); Serial.println(testFilledRects(TFT_YELLOW, TFT_MAGENTA));
	//total+=testFilledRects(TFT_YELLOW, TFT_MAGENTA);
	//delay(500);

	yield(); Serial.print(F("Circles (filled)         "));
	yield(); Serial.println(testFilledCircles(10, TFT_MAGENTA));
	//total+= testFilledCircles(10, TFT_MAGENTA);

	yield(); Serial.print(F("Circles (outline)        "));
	yield(); Serial.println(testCircles(10, TFT_WHITE));
	//total+=testCircles(10, TFT_WHITE);
	//delay(500);

	yield(); Serial.print(F("Triangles (outline)      "));
	yield(); Serial.println(testTriangles());
	//total+=testTriangles();
	//delay(500);

	yield(); Serial.print(F("Triangles (filled)       "));
	yield(); Serial.println(testFilledTriangles());
	//total += testFilledTriangles();
	//delay(500);

	yield(); Serial.print(F("Rounded rects (outline)  "));
	yield(); Serial.println(testRoundRects());
	//total+=testRoundRects();
	//delay(500);

	yield(); Serial.print(F("Rounded rects (filled)   "));
	yield(); Serial.println(testFilledRoundRects());
	//total+=testFilledRoundRects();
	//delay(500);

	yield(); Serial.println(F("Done!")); yield();
	//Serial.print(F("Total = ")); Serial.println(total);

	//yield();Serial.println(millis()-tn);

	//*****************************************************************************

	tft.fillScreen(TFT_BLACK);
	tft.setRotation(1);
	tft.setCursor(0, 0);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);;  tft.setTextSize(2);
	tft.print("Connecting WiFi ");
	tft.println(ssid);

	// Connect to Wi-Fi network with SSID and password
	int count_connect = 0;
	bool WiFi_connect = true;

	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
		count_connect++;
		if (count_connect > 30)
		{
			WiFi_connect = false;
			break;
		}
	}

	if (WiFi_connect)
	{
		Serial.println("");
		Serial.println("WiFi connected.");
		tft.println("WiFi connected.");
	}
	else
	{
		Serial.println("");
		Serial.println("WiFi not connected.");
		tft.println("WiFi not connected.");
    }

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  //timeClient.setTimeOffset(3600);
  timeClient.setTimeOffset(10800);
  // Initialize SD card
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
	  Serial.println("Card Mount Failed");
	  tft.println("Card Mount Failed");
	 // return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
	  Serial.println("No SD card attached");
	  tft.println("No SD card attached");
	 // return;
  }
  Serial.println("Initializing SD card...");
  tft.println("Initializing SD card");
  if (!SD.begin(SD_CS)) {
	  Serial.println("ERROR - SD card initialization failed!");
	 // return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if (!file) {
	  Serial.println("File doens't exist");
	  tft.println("File doens't exist");
	  Serial.println("Creating file...");
	  tft.println("Creating file...");
	  writeFile(SD, "/data.txt", "Reading ID, Date, Hour, Temperature \r\n");
  }
  else {
	  Serial.println("File already exists");
	  tft.println("File already exists");
  }
  file.close();

  // Enable Timer wake_up
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // Start the DallasTemperature library
  sensors.begin();

  getReadings();
  getTimeStamp();
  logSDCard();

  // Increment readingID on every new reading
  readingID++;


  if (!rtc.begin()) {
	  Serial.println("Couldn't find RTC");
	  Serial.flush();
	  abort();
  }

  if (rtc.lostPower()) {
	  Serial.println("RTC lost power, let's set the time!");
	  // When time needs to be set on a new device, or after a power loss, the
	  // following line sets the RTC to the date & time this sketch was compiled
	  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	  // This line sets the RTC with an explicit date & time, for example to set
	  // January 21, 2014 at 3am you would call:
	  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  redBtn();


  // Start deep sleep
  //Serial.println("DONE! Going to sleep now.");
  //esp_deep_sleep_start();






}

void loop(void) 
{
	unsigned long currentMillis = millis();

	if (currentMillis - previousMillis >= interval) 
	{
		// save the last time you
		previousMillis = currentMillis;
		getReadings();
		getTime();
	}

	button_run();

  //for (uint8_t rotation = 0; rotation < 4; rotation++) 
  //{
  //  tft.setRotation(rotation);
  //  testText();
  //  delay(2000);
  //}
}


unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_GREEN);
  tft.fillScreen(TFT_BLUE);
  tft.fillScreen(TFT_BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(TFT_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(TFT_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(TFT_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  //tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(TFT_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(TFT_BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for (x2 = 0; x2 < w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for (y2 = 0; y2 < h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (y = 0; y < h; y += 5) tft.drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(TFT_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = min(tft.width(), tft.height());
  for (i = n - 1; i > 0; i -= 6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (x = radius; x < w; x += r2) {
    for (y = radius; y < h; y += r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                      w = tft.width()  + radius,
                      h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for (x = 0; x < w; x += r2) {
    for (y = 0; y < h; y += r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n     = min(cx, cy);
  start = micros();
  for (i = 0; i < n; i += 5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (i = min(cx, cy); i > 10; i -= 5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(i, i, 0));
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for (i = 0; i < w; i += 6) {
    i2 = i / 2;
    tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  start = micros();
  for (i = min(tft.width(), tft.height()); i > 20; i -= 6) {
    i2 = i / 2;
    tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
  }

  return micros() - start;
}

/***************************************************
  Original Adafruit text:

  This is an example sketch for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/



 // Function to get temperature
void getReadings() {
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0); // Temperature in Celsius
	//temperature = sensors.getTempFByIndex(0); // Temperature in Fahrenheit
	Serial.print("Temperature: ");
	Serial.println(temperature);
	tft.setCursor(0, 80);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);;  tft.setTextSize(2);
	tft.print("Temperature DS1820: ");
	tft.setCursor(240, 80);
	tft.println(temperature);

}

// Function to get date and time from NTPClient
void getTimeStamp() {
	while (!timeClient.update()) {
		timeClient.forceUpdate();
	}
	// The formattedDate comes with the following format:
	// 2018-05-28T16:00:13Z
	// We need to extract date and time
	formattedDate = timeClient.getFormattedDate();
	Serial.println(formattedDate);

	// Extract date
	int splitT = formattedDate.indexOf("T");
	dayStamp = formattedDate.substring(0, splitT);
	Serial.println(dayStamp);
	// Extract time
	timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
	Serial.println(timeStamp);
}

// Write the sensor readings on the SD card
void logSDCard() {
	dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(timeStamp) + "," +
		String(temperature) + "\r\n";
	Serial.print("Save data: ");
	Serial.println(dataMessage);
	appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
	Serial.printf("Writing file: %s\n", path);

	File file = fs.open(path, FILE_WRITE);
	if (!file) {
		Serial.println("Failed to open file for writing");
		return;
	}
	if (file.print(message)) {
		Serial.println("File written");
	}
	else {
		Serial.println("Write failed");
	}
	file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
	Serial.printf("Appending to file: %s\n", path);

	File file = fs.open(path, FILE_APPEND);
	if (!file) {
		Serial.println("Failed to open file for appending");
		return;
	}
	if (file.print(message)) {
		Serial.println("Message appended");
	}
	else {
		Serial.println("Append failed");
	}
	file.close();
}

void getTime()
{

	DateTime now = rtc.now();

	Serial.print(now.year(), DEC);
	Serial.print('/');
	Serial.print(now.month(), DEC);
	Serial.print('/');
	Serial.print(now.day(), DEC);
	Serial.print(" (");
	Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
	Serial.print(") ");
	Serial.print(now.hour(), DEC);
	Serial.print(':');
	Serial.print(now.minute(), DEC);
	Serial.print(':');
	Serial.print(now.second(), DEC);
	Serial.println();

	tft.setCursor(0, 110);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);;  tft.setTextSize(2);
	tft.print(now.year(), DEC);
	tft.print('/');
	tft.print(now.month(), DEC);
	tft.print('/');
	tft.print(now.day(), DEC);
	tft.print(" (");
	tft.print(daysOfTheWeek[now.dayOfTheWeek()]);

	tft.println(") ");
	tft.println();
	tft.print(now.hour(), DEC);
	tft.print(':');
	tft.print(now.minute(), DEC);
	tft.print(':');
	tft.print(now.second(), DEC);
	tft.println();
	

	Serial.print(" since midnight 1/1/1970 = ");
	Serial.print(now.unixtime());
	Serial.print("s = ");
	Serial.print(now.unixtime() / 86400L);
	Serial.println("d");

	// calculate a date which is 7 days, 12 hours, 30 minutes, 6 seconds into the future
	DateTime future(now + TimeSpan(7, 12, 30, 6));

	Serial.print(" now + 7d + 12h + 30m + 6s: ");
	Serial.print(future.year(), DEC);
	Serial.print('/');
	Serial.print(future.month(), DEC);
	Serial.print('/');
	Serial.print(future.day(), DEC);
	Serial.print(' ');
	Serial.print(future.hour(), DEC);
	Serial.print(':');
	Serial.print(future.minute(), DEC);
	Serial.print(':');
	Serial.print(future.second(), DEC);
	Serial.println();

	Serial.print("Temperature: ");
	Serial.print(rtc.getTemperature());
	Serial.println(" C");

	tft.setCursor(0, 165);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(2);
	tft.print("Temperature RTC: ");
	tft.print(rtc.getTemperature());
	tft.println(" C");


	Serial.println();
}


void touch_calibrate()
{
	uint16_t calData[5];
	uint8_t calDataOK = 0;

	// check file system exists
	if (!SPIFFS.begin()) {
		Serial.println("Formating file system");
		SPIFFS.format();
		SPIFFS.begin();
	}

	// check if calibration file exists and size is correct
	if (SPIFFS.exists(CALIBRATION_FILE)) {
		if (REPEAT_CAL)
		{
			// Delete if we want to re-calibrate
			SPIFFS.remove(CALIBRATION_FILE);
		}
		else
		{
			File f = SPIFFS.open(CALIBRATION_FILE, "r");
			if (f) {
				if (f.readBytes((char *)calData, 14) == 14)
					calDataOK = 1;
				f.close();
			}
		}
	}

	if (calDataOK && !REPEAT_CAL) {
		// calibration data valid
		tft.setTouch(calData);
	}
	else {
		// data not valid so recalibrate
		tft.fillScreen(TFT_BLACK);
		tft.setCursor(20, 0);
		tft.setTextFont(2);
		tft.setTextSize(1);
		tft.setTextColor(TFT_WHITE, TFT_BLACK);

		tft.println("Touch corners as indicated");

		tft.setTextFont(1);
		tft.println();

		if (REPEAT_CAL) {
			tft.setTextColor(TFT_RED, TFT_BLACK);
			tft.println("Set REPEAT_CAL to false to stop this running again!");
		}

		tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.println("Calibration complete!");

		// store data
		File f = SPIFFS.open(CALIBRATION_FILE, "w");
		if (f) {
			f.write((const unsigned char *)calData, 14);
			f.close();
		}
	}
}

void drawFrame()
{
	tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}

// Draw a red button
void redBtn()
{
	tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_RED);
	tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_DARKGREY);
	drawFrame();
	tft.setTextColor(TFT_WHITE);
	tft.setTextSize(2);
	tft.setTextDatum(MC_DATUM);
	tft.drawString("ON", GREENBUTTON_X + (GREENBUTTON_W / 2), GREENBUTTON_Y + (GREENBUTTON_H / 2));
	SwitchOn = false;
	digitalWrite(rele1, SwitchOn);
}

// Draw a green button
void greenBtn()
{
	tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_GREEN);
	tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_DARKGREY);
	drawFrame();
	tft.setTextColor(TFT_WHITE);
	tft.setTextSize(2);
	tft.setTextDatum(MC_DATUM);
	tft.drawString("OFF", REDBUTTON_X + (REDBUTTON_W / 2) + 1, REDBUTTON_Y + (REDBUTTON_H / 2));
	SwitchOn = true;
	digitalWrite(rele1, SwitchOn);
}

void button_run()
{

	// See if there's any touch data for us
	if (tft.getTouch(&x, &y))
	{
		// Draw a block spot to show where touch was calculated to be
	#ifdef BLACK_SPOT
			tft.fillCircle(x, y, 2, TFT_BLACK);
	#endif

		if (SwitchOn)
		{
			if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
				if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
					Serial.println("Red btn hit");
					redBtn();
				}
			}
		}
		else //Record is off (SwitchOn == false)
		{
			if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
				if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
					Serial.println("Green btn hit");
					greenBtn();
				}
			}
		}

		Serial.println(SwitchOn);

	}



}