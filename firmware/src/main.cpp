#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif
//#include <Wire.h>
//#include "SPI.h"
//#include "gfx_extensions.h"
//#include "Adafruit_ILI9341.h"
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
//#include <display.h>
//#include <shift.h>
#include <debug.h>
#include <inttypes.h>


void setup() {
  serial_init(115200);
  serial_out_s("Starting PIOC");

  /*pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
 */

 //display_init();
 //valve_init();

}

void loop(void) {
  //testText();
  //Serial.println(F("Loop"));
  //testFillScreen();
  //delay(3000);
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  //while(1) loop();
}
#endif
