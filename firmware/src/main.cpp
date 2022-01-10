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
//#include <display.h>
#else // Native
#include <iostream>
#endif

#include <core.h>

#include <psa_task.h>
#include <device_task.h>
//#include <display_task.h>
#include <sensor_read_task.h>

using namespace OxCore;
static Core core;

class MockTask: public Task {
    private:
        bool _init() override {
            //std::cout << "Inited mock task\n";
            return true;
        }
        bool _run() override {
            #ifndef ARDUINO
            std::cout << "Run mock task\n";
            #endif
            return true;
        }
};


/***** Declare your tasks here *****/

OxApp::DeviceTask deviceTask;
OxApp::PsaTask psaTask;
OxApp::SensorReadTask sensorTask;

/***********************************/

void setup()
{
  OxCore::serialBegin(115200UL);
  OxCore::Debug<const char *>("Starting Ox...\n");

  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }

  /***** Configure and add your tasks here *****/

  TaskProperties deviceProperties;
  deviceProperties.id = 10;
  deviceProperties.period = 100;
  deviceProperties.priority = TaskPriority::Medium;
  core.AddTask(&deviceTask, &deviceProperties);

  TaskProperties psaProperties;
  psaProperties.id = 20;
  psaProperties.period = 100;
  psaProperties.priority = TaskPriority::High;
  core.AddTask(&psaTask, &psaProperties);

  TaskProperties sensorProperties;
  sensorProperties.id = 30;
  sensorProperties.period = 25;
  sensorProperties.priority = TaskPriority::High;
  core.AddTask(&sensorTask, &sensorProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  /*********************************************/
}

void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");
  // Blocking call
  if (core.Run() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
      // Loop endlessly to stop the program from running
      while (true) {}
#endif
      return;
  }
}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  while (true) {
    loop();
  }
  return 0;
}
#endif
