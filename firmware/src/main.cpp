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

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif

#ifdef ARDUINO
#include <display.h>
#endif
#include <debug.h>
#include <shift.h>
#include <inttypes.h>
#include <controller.h>
#include <config.h>
#include <timer.h>

using namespace OxDebug;
using namespace OxController;
using namespace OxTimer;

ValveController vc(&valveArray[0], NUM_VALVES);
Timer valveCycle;
unsigned int tLast;

#ifdef ARDUINO
Ox_Display display;
#endif
unsigned int displayTick;

void setup() {
  serialBegin(115200);
  Debug<const char*>("Starting Ox\n");


  shiftInit();


  #ifdef ARDUINO
  display = Ox_Display();
  display.displayInit();
  display.startScreen();
  delay(2000);
  display.debugScreen();

  valveCycle.Init(millis());
  #else
  valveCycle.Init(TimeSinceEpochMs());
  #endif

  /*// Test display layout and graph experiment
  display.drawButton();
  display.updateGraph();*/

  tLast = 0;
  displayTick = 0;
}

void printValveState(uint8_t vs){
#ifdef ARDUINO
  Serial.print("Valves: ");
  for (int b = 7; b >= 0; b--)
  {
    Serial.print(bitRead(vs, b));
  }
  Serial.println("");
#endif
}

void loop(void) {
  valveCycle.Update();

  if (valveCycle.GetElapsed() >= tLast + TIME_STEP){
    tLast = valveCycle.GetElapsed();
    vc.updateController(&tLast);
#ifdef ARDUINO
    uint8_t out = vc.getValveBits();
    shiftOutValves(out);
#endif
    printValveState(vc.getValveBits());
  } else if (valveCycle.GetElapsed() >= TOTAL_CYCLE_TIME){
    vc.resetValves();
#ifdef ARDUINO
    valveCycle.Init(millis());
#else
    valveCycle.Init(TimeSinceEpochMs());
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
