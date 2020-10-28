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

Timer valveTimer;
ValveController vc(VALVES, NUM_VALVES);

void setup() {
  serialBegin(115200);
  Debug<const char*>("Starting PIOC\n");

  valveTimer.tStart = startTimer();
  
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

  ////// UPDATE VALVES //////
  valveTimer.tickTime = valveTimer.tElapsed * 1000; //convert double seconds to int milliseconds
  
  // Update valve controller every timestep
  // BUG: doesnt trigger on t=0!
  // Will this compound timing errors? Probably better ways to do this.
  if (valveTimer.tickTime >= valveTimer.tLast + TIME_STEP) {
    vc.tick(valveTimer.tickTime);
    valveTimer.tLast = valveTimer.tickTime;
  }

  // One valve cycle has been completed
  if (valveTimer.tickTime >= TOTAL_CYCLE_TIME){
    Debug<const char*>("End of cycle!\n");
    valveTimer.tStart = startTimer();
    valveTimer.tLast = 0;
    valveTimer.numCycles++;
  }

  valveTimer.tElapsed = checkTimer(false, valveTimer.tStart);

#ifndef ARDUINO
  if (valveTimer.numCycles > 5) // Exit condition for testing
    exit(0);
#endif

  ////// END OF UPDATE VALVES //////
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  while(1) loop();
}
#endif
