#include <Arduino.h>
#include <Wire.h>
#include "SPI.h"
#include "gfx_extensions.h"
#include "Adafruit_ILI9341.h"


#define TFT_CLK 13
#define TFT_MISO 12
#define TFT_MOSI 27
#define TFT_CS 33
#define TFT_DC 15

#define TFT_RST 32

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


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

  tft.setRotation(3);
    
}


void loop(void) {
  //testText();
  testFillScreen();
  delay(3000);
}


