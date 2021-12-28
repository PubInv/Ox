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
#include <display.h>
#else // Native
#include <iostream>
#endif

#include <core.h>
//#include <shift.h>
#include <cstdint>

#include <psa_valve_task.h>
//#include <display_task.h>

//////////////////////

/*OxPSA::ValveConfig valveArray[NUM_VALVES] = {
  { 'A', 0, 0, 1, 100, 4000, },
  { 'B', 1, 0, 2, 4000, 8000, },
  { 'C', 2, 0, 4, 3700, 4000, },
  { 'D', 3, 0, 8, 7700, 8000, }};
*/
//////////////////////


using namespace OxCore;

class MockTask: public Task {
    private:
        bool _init() override {
            //std::cout << "Inited mock task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Run mock task\n";
            return true;
        }
};

Core core;


// Add task references here //
OxPSA::PsaValveTask psa;

void setup()
{
#ifdef ARDUINO
  serialBegin(115200);
#endif
  //OxCore::Debug<const char *>("Starting Ox\n");
  bool success = false;
  
  success = core.Boot();
  if (!success) {
    //return 1;
  }

  
#ifdef ARDUINO
  // Init the shift register
  //shiftInit();

// OxDisplay::DisplayTask display;
//  display.init(1, 20);
//  AddTask(&display, 1);
#endif

  // Add tasks to core here //
  core.AddTask(&psa, 1, 10);

  core.Run();
}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  //while (1)
  //  loop();
}
#endif
