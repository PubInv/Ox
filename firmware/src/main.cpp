/* Copyright (C) 2023 Robert Read, Ben Coombs.

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

#include <retrieve_script_UDP_task.h>
#include <cog_task.h>
#include <serial_task.h>
#include <fault_task.h>
// #include <fanPID_task.h>
#ifdef TEST_FANS_ONLY
#include <fanTEST_task.h>
#endif

using namespace OxCore;
static Core core;

/***** Declare your tasks here *****/

OxApp::RetrieveScriptUDPTask retrieveScriptUDPTask;
OxApp::CogTask cogTask;
OxApp::SerialTask serialTask;
OxApp::FaultTask faultTask;
// OxApp::FanPIDTask fanPIDTask;
#ifdef TEST_FANS_ONLY
OxApp::FanTESTTask fanTESTTask;
#endif

#include <machine.h>

MachineConfig cogConfig;
/***********************************/

#define ETHERNET_BOARD_PRESENT 0

#define WIPER_VALUE_PIN A0

#define TEST_PWM DAC0

// TODO: we need to have setups for individual pieces
// of the Hardware Abstraction Layer
void setup()
{
  OxCore::serialBegin(115200UL);
  Debug<const char *>("Starting Ox...\n");

  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }


  //  Eventually we will migrate all hardware to the MachineHAL..
  cogConfig.hal = new MachineHAL();
  bool initSuccess  = cogConfig.hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
    //    while(1);
  }
  // Serial.print("FLOW SENSOR SERIAL NUMBER : ");
  // Serial.println(cogConfig.hal->_flowsensor->flowSensor->serialNumber,HEX);

  // if (cogConfig.hal->_flowsensor->flowSensor->serialNumber == 0xFFFFFFFF) {
  //   Serial.println("FLOW SENSOR NOT AVIALABLE!");
  //   Serial.println("THIS IS A CRITICAL ERROR!");
  //   //    while(1);
  // }



  //TODO: This needs to be placed inthe task init feature!
  //#if BUILD_ENV_NAME == due_ribbonfish
#ifdef RIBBONFISH
  // TODO: I am not sure where to put this; and it is
  // probably only valid on Due...nonetheless it is
  // absolutely needed byt the TF800A12K.cpp/SL_PS class...
  // it really should be there...
      pinMode(MAX31850_DATA_PIN, INPUT);
      pinMode(RF_FAN, OUTPUT);
      pinMode(RF_HEATER, OUTPUT);
      pinMode(RF_STACK, OUTPUT);
#endif

      // Now we will set the machine state to "Off"
      cogConfig.ms = Off;

  /***** Configure and add your tasks here *****/

#ifndef TEST_FANS_ONLY
  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.period = 10000;
  cogProperties.priority = OxCore::TaskPriority::High;
  // Note: The cogConfig is universal to all tasks.
  // It respresents the entire machine.
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
  faultProperties.name = "fault";
  faultProperties.id = 22;
  faultProperties.period = 30000;
  faultProperties.priority = OxCore::TaskPriority::High;
  faultProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&faultTask, &faultProperties);


  // OxCore::TaskProperties fanPIDProperties;
  // fanPIDProperties.name = "fanPID";
  // fanPIDProperties.id = 23;
  // fanPIDProperties.period = 40000;
  // fanPIDProperties.priority = OxCore::TaskPriority::High;
  // fanPIDProperties.state_and_config = (void *) &cogConfig;
  //     core.AddTask(&fanPIDTask, &fanPIDProperties);

      if (ETHERNET_BOARD_PRESENT) {
  OxCore::TaskProperties retrieveScriptUDPProperties;
  retrieveScriptUDPProperties.name = "retrieveScriptUDP";
  retrieveScriptUDPProperties.id = 24;
  retrieveScriptUDPProperties.period = 5000;
  retrieveScriptUDPProperties.priority = OxCore::TaskPriority::High;
  retrieveScriptUDPProperties.state_and_config = (void *) &cogConfig;

  core.AddTask(&retrieveScriptUDPTask, &retrieveScriptUDPProperties);
      }
#else
  OxCore::TaskProperties fanTESTProperties;
  fanTESTProperties.name = "fanTEST";
  fanTESTProperties.id = 25;
  fanTESTProperties.period = 10000;
  fanTESTProperties.priority = OxCore::TaskPriority::High;
  fanTESTProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&fanTESTTask, &fanTESTProperties);
#endif

  OxCore::Debug<const char *>("Added tasks\n");

  /*********************************************/
}

#define WIPER_VALUE_PIN A0

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
