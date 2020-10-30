#ifndef CONFIG_H
#define CONFIG_H

//#include <valve.h>

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

// Pins
#define CONTROL_VALVE_A 12
#define CONTROL_VALVE_B 13
#define BALANCE_VALVE_A 33
#define BALANCE_VALVE_B 27


// SPI Display
#define TFT_CLK 5
#define TFT_MISO  19
#define TFT_MOSI 18
#define TFT_CS 14
#define TFT_DC 32
#define TFT_RST 15

// Shift register
#define DS 13 // 747HC pin 14 - serial data
#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)
#define SH_CP 27 // 747HC pin 11 - shift register clock

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

///////// VALVES //////////

#define TOTAL_CYCLE_TIME 12900 //ms
#define TIME_STEP 100 //ms
#define NUM_VALVES 4

// Valve start and stop times must be a multiple of TIME_STEP
// or they will go undetected!
// Minimum start time is TIME_STEP (not 0!)
PIOC_Controller::valve VALVES[NUM_VALVES] = {
  { .name = 'A',
    .num = 0,
    .status = 0,
    .err = 0,
    .pin = 1,//CONTROL_VALVE_A,
    .start = 100,
    .stop = 6000 },
  { .name = 'B',
      .num = 1,
      .status = 0,
      .err = 0,
      .pin = 2,//CONTROL_VALVE_B,
      .start = 6000,
      .stop = 6400 },
  { .name = 'C',
      .num = 2,
      .status = 0,
      .err = 0,
      .pin = 4,//BALANCE_VALVE_A,
      .start = 6400,
      .stop = 12400 },
  { .name = 'D',
      .num = 3,
      .status = 0,
      .err = 0,
      .pin = 8,//BALANCE_VALVE_B,
      .start = 12400,
      .stop = 12900 }};

#endif
