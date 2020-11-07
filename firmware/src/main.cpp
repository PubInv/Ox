#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif

#include <display.h>
#include <debug.h>
#include <shift.h>
#include <inttypes.h>
#include <controller.h>
#include <config.h>
#include <timer.h>

using namespace PIOC_Debug;
using namespace PIOC_Controller;
using namespace PIOC_Timer;
using namespace PIOC;

ValveController vc(&valveArray[0], NUM_VALVES);
Timer valveCycle;
unsigned int tLast;

PIOC_Display display;
unsigned int displayTick;

void setup() {
  serialBegin(115200);
  //Debug<const char*>("Starting PIOC\n");


  shiftInit();


  #ifdef ARDUINO
  display = PIOC_Display(); 
  display.displayInit();
  //display.startScreen();
  //delay(2000);
  //display.debugScreen();
  
  display.drawButton();

  valveCycle = Timer(millis());
  #else
  valveCycle = Timer(timeSinceEpochMs());
  #endif
  
  display.updateGraph();

  tLast = 0;
  displayTick = 0;
}

void printValveState(byte vs){
  Serial.print("Valves: ");
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(vs, b));
  }
  Serial.println("");
}

void loop(void) {
  valveCycle.update();

  if (valveCycle.elapsed() >= tLast + TIME_STEP){
    tLast = valveCycle.elapsed();
    vc.updateController(&tLast);
#ifdef ARDUINO
    uint8_t out = vc.getValveBits();
    shiftOutValves(out);
#endif
    printValveState(vc.getValveBits());
  } else if (valveCycle.elapsed() >= TOTAL_CYCLE_TIME){
    vc.resetValves();
#ifdef ARDUINO
    valveCycle = Timer(millis()); 
#else
    valveCycle = Timer(timeSinceEpochMs());
#endif
    tLast = 0; // TODO: put this in the timer class
  }
  
#ifdef ARDUINO
  displayTick++; // TODO: make this a timer
  if (displayTick >= 10000){
    display.valveState(valveCycle.elapsed(), vc.getValveBits());

    displayTick = 0;
  }
#endif

  //exit(0);
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  while(1) loop();
}
#endif
