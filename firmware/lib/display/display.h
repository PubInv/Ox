#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

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

void display_init();
unsigned long testText();
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
uint16_t color_convert(uint8_t red, uint8_t green, uint8_t blue);
void testFillScreen();

#endif
