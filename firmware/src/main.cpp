#include <Arduino.h>
#include <Wire.h>
#include "SPI.h"
#include "gfx_extensions.h"
#include "Adafruit_ILI9341.h"
//#include <Adafruit_TFTLCD.h> // Hardware-specific library


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

#define DS 13 // 747HC pin 14 - serial data
#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)
#define SH_CP 27 // 747HC pin 11 - shift register clock

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//SPI Display
#define TFT_CLK 5
#define TFT_MISO  19
#define TFT_MOSI 18
#define TFT_CS 14
#define TFT_DC 32
#define TFT_RST 15

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void shift()
{
  // Convert from 8 bit and output to shift register
  for (int j = 0; j < 256; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(ST_CP, LOW);
    shiftOut(DS, SH_CP, LSBFIRST, j);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(ST_CP, HIGH);
    delay(1000);
  }

}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
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

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
 /*   for (int16_t i = 0; i < x + w; i++){
        for (int j = 0; j < y + h; j++){
            tft.writePixel(i, j, color);
        }
    }
*/
    for (int16_t i = x; i < x + w; i++) {
    //writeFastVLine(i, y, h, color);
  //  drawFastVLine(x, y, h, color);
    tft.writeLine(x, y, x, y + h - 1, color);
  
  }
}

// source: http://www.barth-dev.de/online/rgb565-color-picker
uint16_t color_convert(uint8_t red, uint8_t green, uint8_t blue){
    return (((red & 0xf8)<<8) + ((green & 0xfc)<<3)+(blue>>3));
}

//unsigned long testFillScreen() {
void testFillScreen() {
    //uint16_t Rgb565 = (((red & 0xf8)<<8) + ((green & 0xfc)<<3)+(blue>>3));
    
    uint16_t Rgb565 = color_convert(129,198,247);
  //unsigned long start = micros();
  //fillRect(0,0, 240, 320, ILI9341_BLACK);
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0,0,320,50, Rgb565);
  tft.setCursor(20, 20);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Hello World!");
  /*yield();
  tft.fillScreen(ILI9341_RED);
  yield();
  tft.fillScreen(ILI9341_GREEN);
  yield();
  return micros() - start;*/
}

void setup() {
  Serial.begin(115200);
  Serial.println("ILI9341 Test!"); 
 
  /*pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
 */
  tft.begin();

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 

  Serial.println(F("Done!"));

  

  tft.setRotation(1);    
}



void loop(void) {
  testText();
  Serial.println(F("Loop"));
  //testFillScreen();
  delay(3000);
}