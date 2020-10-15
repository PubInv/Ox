#include <Arduino.h>
//#include <Wire.h>
//#include "SPI.h"
//#include "gfx_extensions.h"
//#include "Adafruit_ILI9341.h"
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <display.h>
#include <shift.h>

// ADAFRUIT FEATHER ESP32
// RST
// 3V
// NC
// GND
// 26 DAC2 A0   BATT
// 25 DAC1 A1   EN
// 34 A2     USB
// 39 A3     13 A12
// 36 A4     12 A11
// 4 A5      27 A10
// 5 SCK     33 A9
// 18 MO     15 A8
// 19 MI     32 A7
// 16 RX     14 A6
// 17 TX     SCL 22
// 21 21     SDA 23

// DISPLAY
/*
#define TOUCH_Y1 5
#define TOUCH_X1 6
#define TOUCH_Y2 34
#define TOUCH_X2 39

#define LCD_RST 36
#define LCD_RD 4
#define LCD_WR 5
#define LCD_CD 18
#define LCD_CS 19

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RST);
*/

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!");

  /*pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
 */

 display_init();
 valve_init();

}



void loop(void) {
  testText();
  Serial.println(F("Loop"));
  //testFillScreen();
  delay(3000);
}
