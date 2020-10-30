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
#include <debug.h>
//#include <shift.h>
#include <inttypes.h>
//#include <state.h>
#include <controller.h>
#include <config.h>
#include <timer.h>

using namespace PIOC_Debug;
using namespace PIOC_Controller;
using namespace PIOC_Timer;


ValveController vc(VALVES, NUM_VALVES);
Timer timer;

uint32_t tLast;


void setup() {
  serialBegin(115200);
  Debug<const char*>("Starting PIOC\n");

  #ifdef ARDUIO
  timer = Timer();
  #else
  timer = Timer(timeSinceEpochMs());
  #endif

  tLast = 0;
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

  timer.update();

  if (timer.elapsed() > tLast + 100 ){
    tLast = timer.elapsed();
    vc.update(&tLast);
  }

  //exit(0);
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  while(1) loop();
}
#endif
