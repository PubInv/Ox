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
#include <cstdint>
#include <config.h>
#include <task.h>
#include <scheduler.h>
#include <psa_valve_task.h>
#include <display_task.h>

namespace OxCore {

void setup()
{
#ifdef ARDUINO
  serialBegin(115200);
#endif
  Debug<const char *>("Starting Ox\n");

  // Init the shift register
  shiftInit();

  // Init and add tasks to scheduler
  OxPSA::PsaCycleTask psa;
  psa.init(0, 10);

  OxDisplay::DisplayTask display;
  display.init(1, 20);

  AddTask(&psa, 0);
  AddTask(&display, 1);
}

void loop(void)
{
  int t_now = 142124124;
  bool success = RunNextTask(t_now);
  if (false == success) {
    // Task failed
  }

  //exit(0);
}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  while (1)
    loop();
}
#endif

}