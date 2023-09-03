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
#include <cog_hal.h>

#include <retrieve_script_UDP_task.h>
#include <cog_task.h>
#include <serial_task.h>
#include <fault_task.h>
#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <temp_refresh_task.h>
#include <serialReportTask.h>

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
SerialReportTask serialReportTask;

#include <machine.h>

MachineConfig machineConfig;
/***********************************/

#define ETHERNET_BOARD_PRESENT 1


// This is to allow a code idiom compatible with the way
// the machine config is found  inside the Tasks
MachineConfig *getConfig() {
  return &machineConfig;
}

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

  //  Eventually we will migrate all hardware to the COG_HAL..
  machineConfig.hal = new COG_HAL();
  bool initSuccess  = machineConfig.hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
    abort();
  }

  // Now we will set the machine state to "Off"
  machineConfig.ms = Off;

  /***** Configure and add your tasks here *****/

  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 19;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) &machineConfig;
  bool readAdd = core.AddTask(&readTempsTask, &readTempsProperties);
  if (!readAdd) {
    OxCore::Debug<const char *>("ReadTemps Task add failed\n");
    abort();
  }

  OxCore::TaskProperties serialReportProperties;
  serialReportProperties.name = "serialReportTemps";
  serialReportProperties.id = 20;
  serialReportProperties.period = serialReportTask.PERIOD_MS;
  serialReportProperties.priority = OxCore::TaskPriority::High;
  serialReportProperties.state_and_config = (void *) &machineConfig;
  bool serialReportAdd = core.AddTask(&serialReportTask, &serialReportProperties);
  if (!serialReportAdd) {
    OxCore::Debug<const char *>("serialReport Task add failed\n");
    abort();
  }

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 21;
  cogProperties.period = cogTask.PERIOD_MS;
  cogProperties.priority = OxCore::TaskPriority::High;
  cogProperties.state_and_config = (void *) &machineConfig;
  bool cogAdd = core.AddTask(&cogTask, &cogProperties);
  if (!cogAdd) {
    OxCore::Debug<const char *>("Cognitive Task add failed\n");
    abort();
  }

  cogTask.heaterPIDTask = &heaterPIDTask;

  OxCore::TaskProperties serialProperties;
  serialProperties.name = "serial";
  serialProperties.id = 22;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) &machineConfig;
  bool serialAdd = core.AddTask(&serialTask, &serialProperties);
  if (!serialAdd) {
    OxCore::Debug<const char *>("SerialProperties add failed\n");
    abort();
  }


  OxCore::TaskProperties TempRefreshProperties;
  TempRefreshProperties.name = "TempRefresh";
  TempRefreshProperties.id = 23;
  TempRefreshProperties.period = tempRefreshTask.PERIOD_MS;
  TempRefreshProperties.priority = OxCore::TaskPriority::Low;
  TempRefreshProperties.state_and_config = (void *) &machineConfig;
  bool tempRefresh = core.AddTask(&tempRefreshTask, &TempRefreshProperties);
  if (!tempRefresh) {
    OxCore::Debug<const char *>("Temp Refresh add failed\n");
    abort();
  }

  cogTask.tempRefreshTask = &tempRefreshTask;


  if (ETHERNET_BOARD_PRESENT) {
    OxCore::TaskProperties retrieveScriptUDPProperties;
    retrieveScriptUDPProperties.name = "retrieveScriptUDP";
    retrieveScriptUDPProperties.id = 24;
    retrieveScriptUDPProperties.period = 5000;
    retrieveScriptUDPProperties.priority = OxCore::TaskPriority::High;
    retrieveScriptUDPProperties.state_and_config = (void *) &machineConfig;

    bool retrieveScriptUDP = core.AddTask(&retrieveScriptUDPTask, &retrieveScriptUDPProperties);
    if (!retrieveScriptUDP) {
      OxCore::Debug<const char *>("Retrieve Script UDP\n");
      abort();
    }
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
  dutyCycleTask.one_pin_heater = getConfig()->hal->_ac_heaters[0];

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

  heaterPIDTask.DEBUG_PID = 1;

  OxCore::Debug<const char *>("Added tasks\n");

  /*********************************************/
}


void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

  // Blocking call
  if (core.Run() == false) {
      OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
      // make sure we print anything needed!
      delay(100);
      // Loop endlessly to stop the program from running
      while (true) {
        Serial.println("INTERNAL ERROR!");
        delay(100);
      }
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
