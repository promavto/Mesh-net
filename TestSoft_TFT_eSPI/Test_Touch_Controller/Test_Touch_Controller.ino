// This sketch is to test the touch controller, nothing is displayed
// on the TFT.  The TFT_eSPI library must be configured to suit your
// pins used. Make sure both the touch chip select and the TFT chip
// select are correctly defined to avoid SPI bus contention.

// Make sure you have defined a pin for the touch controller chip
// select line in the user setup file or you will see "no member"
// compile errors for the touch functions!

// It is a support and diagnostic sketch for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

// The "raw" (unprocessed) touch sensor outputs are sent to the
// serial port. Touching the screen should show changes to the x, y
// and z values. x and y are raw ADC readings, not pixel coordinates.

#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
uint16_t datat[5];
uint16_t x, y;
//====================================================================

void setup(void) {
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);


  tft.calibrateTouch(datat, TFT_WHITE, TFT_BLACK,30);

  Serial.println(datat[0]);
  Serial.println(datat[1]);
  Serial.println(datat[2]);
  Serial.println(datat[3]);
  Serial.println(datat[4]);

  tft.setTouch(datat);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(50, 50);
  tft.println("Test touch End");
  tft.println(datat[0]);
  tft.println(datat[1]);
  tft.println(datat[2]);
  tft.println(datat[3]);
  tft.println(datat[4]);
  tft.println("Data saved");
}

//====================================================================

void loop() {

 /* uint16_t x, y;

  tft.getTouchRaw(&x, &y);
  
  Serial.printf("x: %i     ", x);

  Serial.printf("y: %i     ", y);

  Serial.printf("z: %i \n", tft.getTouchRawZ());

  delay(250);
*/
}

//====================================================================

