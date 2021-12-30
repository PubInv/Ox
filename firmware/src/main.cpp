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
#include <cstdint>

#include <psa_valve_task.h>
//#include <display_task.h>

using namespace OxCore;
Core core;

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


/***** Declare your tasks here *****/

OxPSA::PsaValveTask psaTask;

/***********************************/

void setup()
{
#ifdef ARDUINO
  serialBegin(115200);
#endif
  OxCore::Debug<const char *>("Starting Ox\n");

  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }

  /***** Configure and add your tasks here *****/

  TaskProperties psaProperties;
  psaProperties.id = 20;
  psaProperties.period = 50;
  psaProperties.priority = 50;
  psaProperties.hardTiming = true;
  core.AddTask(&psaTask, &psaProperties);

  /*********************************************/
    
  // Blocking call
  if (core.Run() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToRun);
      //return EXIT_FAILURE;
      return;
  }
}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  //while (1)
  //  loop();
  return 0;
}
#endif
