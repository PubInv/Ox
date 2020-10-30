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

ValveController vc(&valveArray[0], NUM_VALVES);
Timer valveCycle;

unsigned int tLast;

void setup() {
  serialBegin(115200);
  Debug<const char*>("Starting PIOC\n");

  #ifdef ARDUINO
  timer = Timer();
  #else
  valveCycle = Timer(timeSinceEpochMs());
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

  valveCycle.update();

  if (valveCycle.elapsed() >= tLast + TIME_STEP){
    tLast = valveCycle.elapsed();
    vc.updateController(&tLast);
  } else if (valveCycle.elapsed() >= TOTAL_CYCLE_TIME){
    vc.resetValves();
    valveCycle = Timer(timeSinceEpochMs());
    tLast = 0;
  }

  //exit(0);
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  while(1) loop();
}
#endif
