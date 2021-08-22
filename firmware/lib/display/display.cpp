/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "display.h"
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "stdint.h"
#include "Adafruit_ILI9341.h"

namespace Ox {

  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

  void Ox_Display::displayInit(){
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


  void Ox_Display::startScreen(){
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
    tft.println("Public Invention");
    tft.println("Oxygen Concentrator");
  }

  void Ox_Display::debugScreen(){
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print("Timer:");
    tft.setCursor(0,12);
    tft.print("Valves:");
  }

  void Ox_Display::updateDebugScreen(char s){
    tft.setCursor(50,0);
    tft.print(s);
  }

  void Ox_Display::valveState(uint32_t vs, uint8_t state){
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


void Ox_Display::drawButton(){

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
int graphXMin = 1;
int graphXMax = 258;

int maxBarWidth = 20;
int barWidth = maxBarWidth;


void Ox_Display::updateGraph() {
  int x = graphXMin;
  int y1 = graphYZero;
  bool up = true;
  srand (time(NULL));

  int yLast = y1;
  int graphHeight = graphYMin-graphYMax+1;

  for (;;) {
    printf("x %d\n", x);

    // Black bar that moves at a constant rate.
    if (x == graphXMin){
      tft.startWrite();
      tft.writeFillRect(x, graphYMax, barWidth, graphHeight, BLACK);
      tft.endWrite();
      yLast = graphYZero;
      y1 = graphYZero;
      printf("x == graphXMin\n");
    } else {
      tft.drawRect(x, graphYMax, barWidth, graphYMin-graphYMax+1, BLACK); // may be faster as 2 vertical lines
    }

    // Draw a pixel when the line is 1 pixel long, otherwise draw a line.
    // This draws a continuous line no matter the delta.
    /*if (abs(y1-yLast) <= 1) {
      tft.drawPixel(x, y1, YELLOW);
    } else {
      tft.drawFastVLine(x, y1, (yLast-y1), YELLOW);
    }*/

    if ((abs(y1-yLast) <= 1)) { // || (x == graphXMin)
      tft.drawPixel(x, y1, YELLOW);
    } else {
      tft.drawFastVLine(x, y1, (yLast-y1), YELLOW);
    }


    yLast = y1;

    // Update the graph (this is a test function)
    // minus is up!!
    // https://en.wikipedia.org/wiki/Square_wave

    float f = 0.01; //Hz
    float a = 10;
    int noise = 2;
    float ytemp = 0;
    float b = 0;
    for (int i = 0; i < 5; i++){
      b = 2*PI*(2*i-1)*f*x;
      ytemp += ((a*sin(b)) / (2*i-1)) + (-noise+rand()%(noise*2));
    }
    y1 = graphYZero - (int)(4/PI*(ytemp));

    // X step is constant 1 pixel per update.
    x++;

    // At the end of the frame, the black bar shrinks to allow
    // the graph to draw to the end of the frame.
    if (x > graphXMax-barWidth){
      barWidth--;
      if (x >= graphXMax){
        x = graphXMin;
        barWidth = maxBarWidth;
        //yLast = y1;

      }
    }

    delay(20);
  }
}

  /*void Ox_Display::printLine(){

  }

  void Ox_Display::drawGraph(){

  }*/


  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////

  void Ox_Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
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
  uint16_t Ox_Display::color_convert(uint8_t red, uint8_t green, uint8_t blue){
      return (((red & 0xf8)<<8) + ((green & 0xfc)<<3)+(blue>>3));
  }

}
