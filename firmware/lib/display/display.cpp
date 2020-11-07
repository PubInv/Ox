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

 String Key[4][4] = {
  { "7", "8", "9", "/" },
  { "4", "5", "6", "*" },
  { "1", "2", "3", "-" },
  { "C", "0", "=", "+" }
};

String N1, N2, ShowSC, opt;
bool updata=false;
float answers=-1;


int screenWidth = 320;
int screenHeight = 240;


void PIOC_Display::drawButton(){

    int x = 260;
    tft.fillScreen(ILI9341_BLACK);
    //tft.fillRect(0, 80, 240, 240, WHITE);
    tft.drawFastHLine(x, 0, 60, WHITE);
    tft.drawFastHLine(x, 60, 60, WHITE);
    tft.drawFastHLine(x, 120, 60, WHITE);
    tft.drawFastHLine(x, 180, 60, WHITE);
    tft.drawFastHLine(x, 239, 60, WHITE);

    tft.drawFastVLine(x, 0, 240, WHITE);
    tft.drawFastVLine(319, 0, 240, WHITE);

    tft.drawFastVLine(0, 120, 119, WHITE);
    tft.drawFastHLine(0, 120, 260, WHITE);
    tft.drawFastHLine(0, 239, 260, WHITE);

    //tft.drawFastHLine(0, 180, 260, WHITE);
    //tft.drawFastVLine(x, 0, 240, WHITE);
    

    /*tft.drawFastVLine(60, 80, 240, BLACK);
    tft.drawFastVLine(120, 80, 240, BLACK);
    tft.drawFastVLine(180, 80, 240, BLACK);
    tft.drawFastVLine(240-1, 80, 240, BLACK);

    for (int y=0;y<4;y++) {
      for (int x=0;x<4;x++) {
        tft.setCursor(22 + (60*x), 100 + (60*y));
        tft.setTextSize(3);
        tft.setTextColor(BLACK);
        tft.println(Key[y][x]);
      }
    }*/
}

int graphYBase = 190;
int graphYMin = 235;
int graphYZero = 180;
int graphYMax = 125;
int graphXMin = 2;
int graphXMax = 258;
int barWidth = 30;


void PIOC_Display::updateGraph() {
  int x = graphXMin;
  int y1 = graphYZero;
  bool up = true;
  srand (time(NULL));

  //double temp;

  int yLast = y1;

  for (;;) {
    
    // Moving bar
    tft.drawRect(x, graphYMax, barWidth, graphYMin-graphYMax+1, BLACK);
    
    if (abs(y1-yLast) <= 1) {
      tft.drawPixel(x, y1, YELLOW);
    } else {

      tft.drawFastVLine(x, y1, (yLast-y1), YELLOW);
    }

    yLast = y1;
    

    // Update the graph (this is a test function)
    // minus is up!!
    y1 = graphYZero - 20*sin(0.1*x) - 5*sin(0.5*x) - 2*sin(0.75*x); // rand() % 10;
    
    // X step is constant
    x++;
    if (x > graphXMax-barWidth){
      barWidth--;
      if (barWidth == 0){
        x = graphXMin;
        barWidth = 30;
      }
    }

    delay(20);
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
