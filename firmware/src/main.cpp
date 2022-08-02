/* Copyright (C) 2021 Robert Read, Ben Coombs.

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
#include <cog_task.h>
#include <serial_task.h>
#include <fault_task.h>
//#include <display_task.h>

using namespace OxCore;
static Core core;

/***** Declare your tasks here *****/

// OxApp::NetworkingTask networkingTask;
OxApp::CogTask cogTask;
OxApp::SerialTask serialTask;
OxApp::FaultTask faultTask;

// OxApp::SensorReadTask sensorTask;
#include <machine.h>

COGConfig cogConfig;
/***********************************/

/* HACK: Rob is testing the DS3502 POT Here. */
#include <DS3502_digital_pot.h>

/* For this example, make the following connections:
    * DS3502 RH to 5V
    * DS3502 RL to GND
    * DS3502 RW to the pin specified by WIPER_VALUE_PIN
*/

#define WIPER_VALUE_PIN A0

DS3502DigitalPot* ds3502;

// TODO: we need to have setups for individual pieces
// of the Hardware Abstraction Layer
void setup()
{
  OxCore::serialBegin(115200UL);
  Debug<const char *>("Starting Ox...\n");

  ds3502 = new DS3502DigitalPot();

  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }

  //TODO: This needs to be placed inthe task init feature!
  //#if BUILD_ENV_NAME == due_ribbonfish
#ifdef RIBBONFISH
      pinMode(2, INPUT);
      pinMode(3, OUTPUT);
      pinMode(4, OUTPUT);
      pinMode(5, OUTPUT);
#endif

      // Now we will set the machine state to "Off"
      cogConfig.ms = Off;

  /***** Configure and add your tasks here *****/

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.period = 3000;
  cogProperties.priority = OxCore::TaskPriority::High;
  cogProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&cogTask, &cogProperties);


  OxCore::TaskProperties serialProperties;
  serialProperties.name = "serial";
  serialProperties.id = 21;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&serialTask, &serialProperties);

  OxCore::TaskProperties faultProperties;
  serialProperties.name = "fault";
  serialProperties.id = 22;
  serialProperties.period = 3000;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&faultTask, &faultProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  /*********************************************/
}

#define WIPER_VALUE_PIN A0

float n = 0;
#define TEST_DS3502 0
void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

      OxCore::Debug<const char *>("Found Pot\n");
      OxCore::DebugLn<int>(ds3502->foundPot);

      if (TEST_DS3502) {
        if (ds3502->foundPot) {
          OxCore::Debug<const char *>("Settting wiper\n");
          // This is a HACK to test the DS3502..
          delay(5000);
          // Count up the Wiper value as a fraction

          OxCore::DebugLn<float>( n / 128.0);
          ds3502->setWiper(0);
          delay(300);
          ds3502->setWiper(n / 128.0);
          n = n + 127.0;
          n = (((int) n) % 128);
          return;
        }
      }
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
