#include "display.h"
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "stdint.h"
#include "Adafruit_ILI9341.h"

namespace PIOC {

  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

  void PIOC_Display::displayInit(){
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

    Serial.println(F("Display started!"));
    tft.setRotation(1);
  }


  void PIOC_Display::startScreen(){
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("Public Invention");
    tft.println("Oxygen Concentrator");
  }

  void PIOC_Display::debugScreen(){
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print("Timer:");
    tft.setCursor(0,12);
    tft.print("Valves:");
  }

  void PIOC_Display::updateDebugScreen(char s){
    tft.setCursor(50,0);
    tft.print(s);
  }

  void PIOC_Display::valveState(uint32_t vs, uint8_t state){
    tft.fillRect(50,0,42,7,ILI9341_BLACK);
    tft.setCursor(50,0);
    tft.print(vs);
    tft.fillRect(50,12,48,19,ILI9341_BLACK);
    tft.setCursor(50,12);
    for (int b = 7; b >= 0; b--)
    {
      tft.print(bitRead(state, b));
    }
  }

  /*void PIOC_Display::printLine(){

  }

  void PIOC_Display::drawGraph(){

  }*/


  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

  void PIOC_Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
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
  uint16_t PIOC_Display::color_convert(uint8_t red, uint8_t green, uint8_t blue){
      return (((red & 0xf8)<<8) + ((green & 0xfc)<<3)+(blue>>3));
  }

}
