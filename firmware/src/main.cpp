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

/*
Public Invention's Ox Project is an open source hardware design for an oxygen concentrator for use by field hospitals around the world. This design concept aims to design an oxygen concentrator that can be manufactured locally while overcoming challenges posed by human resources, hospital location (geographically), infrastructure and logistics; in addition, this project attempts minimum documentation of their design standards necessary for local approval whilst tackling regulatory requirements for medical devices. Copyright (C) 2021 Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free Firmware/Hardware designs: you can redistribute, use, study it and/or modify it under the terms of the CERN Open Hardware License Version 2 as published here: https://ohwr.org/cern_ohl_s_v2.txt

See the CERN Open Hardware License Version 2 for more details.
You should have received a copy of the CERN Open Hardware License Version 2 along with this program.  If not, see https://ohwr.org/cern_ohl_s_v2.txt.

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
  Debug<const char*>("Starting PIOC\n");


  shiftInit();


  #ifdef ARDUINO
  display = PIOC_Display();
  display.displayInit();
  display.startScreen();
  delay(2000);
  display.debugScreen();

  valveCycle = Timer(millis());
  #else
  valveCycle = Timer(timeSinceEpochMs());
  #endif

  /*// Test display layout and graph experiment
  display.drawButton();
  display.updateGraph();*/

  tLast = 0;
  displayTick = 0;
}

void printValveState(uint8_t vs){
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
