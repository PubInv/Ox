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
//
#endif

#include <core.h>

// #include <networking_task.h>
#include <psa_task.h>
//#include <display_task.h>

// using namespace OxCore;
static OxCore::Core core;

/***** Declare your tasks here *****/

// OxApp::NetworkingTask networkingTask;
OxApp::PsaTask psaTask;
// OxApp::SensorReadTask sensorTask;

/***********************************/

void setup()
{
  OxCore::serialBegin(115200UL);
  OxCore::Debug<const char *>("Starting Ox...\n");

  if (core.Boot() == false) {
      OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }

  /***** Configure and add your tasks here *****/

  OxCore::TaskProperties psaProperties;
  psaProperties.name = "psa";
  psaProperties.id = 20;
  psaProperties.period = 1000;
  psaProperties.priority = OxCore::TaskPriority::High;
  core.AddTask(&psaTask, &psaProperties);

  // OxCore::TaskProperties sensorProperties;
  // sensorProperties.name = "sensor";
  // sensorProperties.id = 30;
  // sensorProperties.period = 30;
  // sensorProperties.priority = OxCore::TaskPriority::High;
  // core.AddTask(&sensorTask, &sensorProperties);

  // TaskProperties networkingProperties = {"networking", 40, 500, TaskPriority::Medium};
  // core.AddTask(&networkingTask, &networkingProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  /*********************************************/
}

void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");
  // Blocking call
  if (core.Run() == false) {
      OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
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
