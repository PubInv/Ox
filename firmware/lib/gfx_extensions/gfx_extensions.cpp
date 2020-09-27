#include "Adafruit_GFX.h"
#include "stdint.h"

class GFX_Extensions : public Adafruit_GFX {

public:
  //Adafruit_GFX(int16_t w, int16_t h); // Constructor
/*
    void drawPixel(int16_t x, int16_t y, uint16_t color) override
    {
        
    
    }
*/


/*    void drawFastVLine(int16_t x, int16_t y, int16_t h,
                                 uint16_t color) override {
        writeLine(x, y, x, y + h - 1, color);
    }
*/
    /*void writeFastVLine(int16_t x, int16_t y, int16_t h,
                                  uint16_t color) override {
        // Overwrite in subclasses if startWrite is defined!
        // Can be just writeLine(x, y, x, y+h-1, color);
        // or writeFillRect(x, y, 1, h, color);
        //drawFastVLine(x, y, h, color);
        writeLine(x, y, x, y+h-1, color);
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                            uint16_t color) override {
    
        for (int16_t i = x; i < x + w; i++) {
            writeFastVLine(i, y, h, color);
        }
    }*/

};