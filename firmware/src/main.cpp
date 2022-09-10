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
#include <fanPID_task.h>
//#include <display_task.h>

#include <DS3502_digital_pot.h>

using namespace OxCore;
static Core core;

/***** Declare your tasks here *****/

// OxApp::NetworkingTask networkingTask;
OxApp::CogTask cogTask;
OxApp::SerialTask serialTask;
OxApp::FaultTask faultTask;
OxApp::FanPIDTask fanPIDTask;

// OxApp::SensorReadTask sensorTask;
#include <machine.h>

MachineConfig cogConfig;
/***********************************/


DS3502DigitalPot* ds3502;


#define WIPER_VALUE_PIN A0

#define TEST_PWM DAC0

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
  // TODO: I am not sure where to put this; and it is
  // probably only valid on Due...nonetheless it is
  // absolutely needed byt the TF800A12K.cpp/SL_PS class...
  // it really should be there...
      pinMode(MAX31850_DATA_PIN, INPUT);
      pinMode(RF_FAN, OUTPUT);
      pinMode(RF_HEATER, OUTPUT);
      pinMode(RF_STACK, OUTPUT);
      //      pinMode(RF_FAN_TACH,INPUT_PULLUP);
      //      attachInterrupt(digitalPinToInterrupt(RF_FAN_TACH),OxApp::tachISR,FALLING);
#endif

      // Now we will set the machine state to "Off"
      cogConfig.ms = Off;

  /***** Configure and add your tasks here *****/

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.period = 5000;
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
  faultProperties.period = 3000;
  faultProperties.priority = OxCore::TaskPriority::High;
  faultProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&faultTask, &faultProperties);


  OxCore::TaskProperties fanPIDProperties;
  fanPIDProperties.name = "fanPID";
  fanPIDProperties.id = 23;
  fanPIDProperties.period = 2000;
  fanPIDProperties.priority = OxCore::TaskPriority::High;
  fanPIDProperties.state_and_config = (void *) &cogConfig;
  core.AddTask(&fanPIDTask, &fanPIDProperties);

  OxCore::Debug<const char *>("Added tasks\n");


  Eventually we will migrate all hardware to the MachineHAL..
  cogConfig.hal = new MachineHAL();
  bool initSuccess  = cogConfig.hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
    while(1);
  }
  Serial.print("FLOW SENSOR SERIAL NUMBER : ");
  Serial.println(cogConfig.hal->_flowsensor->flowSensor->serialNumber,HEX);

  if (cogConfig.hal->_flowsensor->flowSensor->serialNumber == 0xFFFFFFFF) {
    Serial.println("FLOW SENSOR NOT AVIALABLE!");
    Serial.println("THIS IS A CRITICAL ERROR!");
    while(1);
  }

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
