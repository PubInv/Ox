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
#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <temp_refresh_task.h>

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

HeaterPIDTask heaterPIDTask;
DutyCycleTask dutyCycleTask;
ReadTempsTask readTempsTask;
TempRefreshTask tempRefreshTask;

#include <machine.h>

MachineConfig machineConfig;
/***********************************/

#define ETHERNET_BOARD_PRESENT 1


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
  machineConfig.hal = new MachineHAL();
  bool initSuccess  = machineConfig.hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
    //    while(1);
  }
  // Serial.print("FLOW SENSOR SERIAL NUMBER : ");
  // Serial.println(machineConfig.hal->_flowsensor->flowSensor->serialNumber,HEX);

  // if (machineConfig.hal->_flowsensor->flowSensor->serialNumber == 0xFFFFFFFF) {
  //   Serial.println("FLOW SENSOR NOT AVIALABLE!");
  //   Serial.println("THIS IS A CRITICAL ERROR!");
  //   //    while(1);
  // }

  machineConfig.report = new MachineStatusReport();


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
      machineConfig.ms = Off;

  /***** Configure and add your tasks here *****/

  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 20;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&readTempsTask, &readTempsProperties);

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 21;
  cogProperties.period = 10000;
  cogProperties.priority = OxCore::TaskPriority::High;
  // Note: The machineConfig is universal to all tasks.
  // It respresents the entire machine.
  cogProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&cogTask, &cogProperties);

  OxCore::TaskProperties serialProperties;
  serialProperties.name = "serial";
  serialProperties.id = 22;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&serialTask, &serialProperties);


  OxCore::TaskProperties TempRefreshProperties;
  TempRefreshProperties.name = "TempRefresh";
  TempRefreshProperties.id = 23;
  TempRefreshProperties.period = tempRefreshTask.PERIOD_MS;
  TempRefreshProperties.priority = OxCore::TaskPriority::Low;
  TempRefreshProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&tempRefreshTask, &TempRefreshProperties);

  cogTask.tempRefreshTask = &tempRefreshTask;


  // OxCore::TaskProperties faultProperties;
  // faultProperties.name = "fault";
  // faultProperties.id = 23;
  // faultProperties.period = 30000;
  // faultProperties.priority = OxCore::TaskPriority::High;
  // faultProperties.state_and_config = (void *) &machineConfig;
  // core.AddTask(&faultTask, &faultProperties);


  if (ETHERNET_BOARD_PRESENT) {
    OxCore::TaskProperties retrieveScriptUDPProperties;
    retrieveScriptUDPProperties.name = "retrieveScriptUDP";
    retrieveScriptUDPProperties.id = 24;
    retrieveScriptUDPProperties.period = 5000;
    retrieveScriptUDPProperties.priority = OxCore::TaskPriority::High;
    retrieveScriptUDPProperties.state_and_config = (void *) &machineConfig;

    core.AddTask(&retrieveScriptUDPTask, &retrieveScriptUDPProperties);
  }

  heaterPIDTask.dutyCycleTask = &dutyCycleTask;

  OxCore::Debug<const char *>("Duty Cycle Setup\n");
  OxCore::TaskProperties dutyCycleProperties;
  dutyCycleProperties.name = "dutyCycle";
  dutyCycleProperties.id = 25;
  dutyCycleProperties.period = dutyCycleTask.PERIOD_MS;
  dutyCycleProperties.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties.state_and_config = (void *) &machineConfig;
  bool dutyCycleAdd = core.AddTask(&dutyCycleTask, &dutyCycleProperties);
  if (!dutyCycleAdd) {
    OxCore::Debug<const char *>("dutyCycleAdd Failed\n");
    abort();
  }


  OxCore::TaskProperties HeaterPIDProperties;
  HeaterPIDProperties.name = "HeaterPID";
  HeaterPIDProperties.id = 26;
  HeaterPIDProperties.period = heaterPIDTask.PERIOD_MS;
  HeaterPIDProperties.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties.state_and_config = (void *) &machineConfig;
  bool heaterPIDAdd = core.AddTask(&heaterPIDTask, &HeaterPIDProperties);

  if (!heaterPIDAdd) {
    OxCore::Debug<const char *>("heaterPIDAdd Faild\n");
    abort();
  }

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
