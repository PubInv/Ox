/*
  Stage2_heater -- A program to control 3 heaters

  Copyright 2023, Robert L. Read

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

#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <debug.h>
#include <OnePinHeater.h>
using namespace OxCore;


#include <core_defines.h>
#include <core.h>
#include <machine.h>
#include <stage2_config.h>
#include <stage2_hal.h>

#include <Arduino.h>

#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <stage2_heater_task.h>
#include <stage2SerialReportTask.h>
// #include <stage2_serial_task.h>
#include <serial_input_task.h>
// #include <temp_refresh_task.h>
#include <stage2_network_task.h>

using namespace OxCore;
static Core core;

const unsigned long REPORT_PERIOD_MS = 5000;
TaskProperties _properties;
unsigned long time_of_last_report = 0;
MachineConfig *machineConfig[3];

// This is a key parameter, which should perhaps be moved to a specific
// default file to make it clearer!
const float STAGE2_DEFAULT_TEMP = 25.0;
const float STAGE2_OPERATING_TEMP = 750.0;
using namespace std;


using namespace OxApp;
stage2_ReadTempsTask stage2_readTempsTask;

HeaterPIDTask heaterPIDTask[3];
DutyCycleTask dutyCycleTask[3];
Stage2HeaterTask stage2HeaterTask[3];

Stage2SerialReportTask stage2SerialReportTask[3];
TempRefreshTask tempRefreshTask[3];
Stage2NetworkTask stage2NetworkTask;

// Stage2SerialTask stage2SerialTask;

Stage2SerialInputTask stage2SerialInputTask;


//#define ETHERNET_BOARD_PRESENT 1
#define ETHERNET_BOARD_PRESENT 0 //No ethernet.


MachineConfig *getConfig(int i) {
  return machineConfig[i];
}

void setup() {

  OxCore::serialBegin(115200UL);
  delay(500);

  if (core.Boot() == false) {
    ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    // TODO: Output error message
    //return EXIT_FAILURE;
    return;
  }
  Stage2HAL *s2hal = new Stage2HAL();
  bool initSuccess  = s2hal->init();
  Serial.println("about to start!");
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
  }

  for(int i = 0; i < 3; i++) {
    machineConfig[i] = new MachineConfig();
    getConfig(i)->hal = s2hal;
    getConfig(i)->hal->DEBUG_HAL = 0;
    getConfig(i)->ms = Off;

    // Now this is problematic and probably should be undone
    getConfig(i)->ms = Off;
    getConfig(i)->ms = Off;
    getConfig(i)->ms = Off;

  }

  Serial.println("About to run test!");

  // Now we have a problem; we want 3 configurations for 3 different
  // heaters, but not everything can be triplicated.
  // we will use the "0" config as a special config;
  // this is the one that will be used for reporting.

  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 19;
  readTempsProperties.period = stage2_readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) getConfig(0);
  delay(300);
  core.AddTask(&stage2_readTempsTask, &readTempsProperties);
  delay(100);
  // For this class, we give it all configs, so it can place
  // the temperatures in them
  for (int i = 0; i < 3; i++) {
    stage2_readTempsTask.mcs[i] = getConfig(i);
  }

  if (ETHERNET_BOARD_PRESENT) {
    OxCore::TaskProperties Stage2NetworkProperties;
    Stage2NetworkProperties.name = "Stage2Network";
    Stage2NetworkProperties.id = 20;
    Stage2NetworkProperties.period = stage2NetworkTask.PERIOD_MS;
    Stage2NetworkProperties.priority = OxCore::TaskPriority::Low;
    // note we must be cautious here, since there is only one Network interface
    Stage2NetworkProperties.state_and_config = (void *) getConfig(0);
    bool stage2Network = core.AddTask(&stage2NetworkTask, &Stage2NetworkProperties);
    if (!stage2Network) {
      OxCore::Debug<const char *>("Stage2Network add failed\n");
      delay(100);
      abort();
    }

    for (int i = 0; i < 3; i++) {
      stage2NetworkTask.mcs[i] = getConfig(i);
    }
    stage2NetworkTask.DEBUG_UDP = 0;
  }

  for(int i = 0; i < 3; i++) {
    OxCore::TaskProperties stage2SerialReportProperties;
    stage2SerialReportProperties.name = "stage2SerialReportTemps";
    stage2SerialReportProperties.id = 21+i;
    stage2SerialReportProperties.period = stage2SerialReportTask[i].PERIOD_MS;
    stage2SerialReportProperties.priority = OxCore::TaskPriority::High;
    stage2SerialReportProperties.state_and_config = (void *) getConfig(i);
    bool stage2SerialReportAdd = core.AddTask(&stage2SerialReportTask[i], &stage2SerialReportProperties);
    if (!stage2SerialReportAdd) {
      OxCore::Debug<const char *>("stage2SerialReport Task add failed\n");
      abort();
    }

    OxCore::TaskProperties dutyCycleProperties;
    dutyCycleProperties.name = "dutyCycle";
    dutyCycleProperties.id = 24+i;
    dutyCycleProperties.period = dutyCycleTask[i].PERIOD_MS;
    dutyCycleProperties.priority = OxCore::TaskPriority::Low;
    dutyCycleProperties.state_and_config = (void *) getConfig(i);
    core.AddTask(&dutyCycleTask[i], &dutyCycleProperties);
    dutyCycleTask[i].whichHeater = (Stage2Heater) i;

    OxCore::TaskProperties HeaterPIDProperties;
    HeaterPIDProperties.name = "HeaterPID";
    HeaterPIDProperties.id = 27+i;
    HeaterPIDProperties.period = heaterPIDTask[i].PERIOD_MS;
    HeaterPIDProperties.priority = OxCore::TaskPriority::High;
    HeaterPIDProperties.state_and_config = (void *) getConfig(i);
    core.AddTask(&heaterPIDTask[i], &HeaterPIDProperties);
    heaterPIDTask[i].whichHeater = (Stage2Heater) i;

    dutyCycleTask[i].one_pin_heater = getConfig(i)->hal->_ac_heaters[i];

    OxCore::TaskProperties stage2HeaterProperties ;
    stage2HeaterProperties.name = "stage2HeaterTask";
    stage2HeaterProperties.id = 30+i;
    stage2HeaterProperties.period = stage2HeaterTask[i].PERIOD_MS;
    stage2HeaterProperties.priority = OxCore::TaskPriority::High;
    stage2HeaterProperties.state_and_config = (void *) getConfig(i);
    bool stage2HeaterTaskAdded = core.AddTask(&stage2HeaterTask[i], &stage2HeaterProperties);
    if (!stage2HeaterTaskAdded) {
      OxCore::Debug<const char *>("stage add Failed\n");
      delay(100);
      abort();
    }
    stage2HeaterTask[i].whichHeater = (Stage2Heater) i;
    stage2HeaterTask[i].heaterPIDTask = &heaterPIDTask[i];

    getConfig(i)->ms = Off;

    heaterPIDTask[i].dutyCycleTask = &dutyCycleTask[i];

    // OxCore::TaskProperties TempRefreshProperties;
    // TempRefreshProperties.name = "TempRefresh";
    // TempRefreshProperties.id = 33+i;
    // TempRefreshProperties.period = tempRefreshTask[i].PERIOD_MS;
    // TempRefreshProperties.priority = OxCore::TaskPriority::Low;
    // TempRefreshProperties.state_and_config = (void *) getConfig(i);
    //   bool tempRefresh = core.AddTask(&tempRefreshTask[i], &TempRefreshProperties);
    // if (!tempRefresh) {
    //   OxCore::Debug<const char *>("Temp Refresh add failed\n");
    //   delay(100);
    //   abort();
    // }
    // stage2HeaterTask[i].tempRefreshTask = &tempRefreshTask[i];

    if (ETHERNET_BOARD_PRESENT) {
      OxCore::TaskProperties Stage2NetworkProperties;
      Stage2NetworkProperties.name = "Stage2Network";
      Stage2NetworkProperties.id = 36+i;
      Stage2NetworkProperties.period = stage2NetworkTask[i].PERIOD_MS;
      Stage2NetworkProperties.priority = OxCore::TaskPriority::Low;
      Stage2NetworkProperties.state_and_config = (void *) getConfig(i);
      bool stage2Network = core.AddTask(&stage2NetworkTask[i], &Stage2NetworkProperties);
      if (!stage2Network) {
        OxCore::Debug<const char *>("Stage2Network add failed\n");
        delay(100);
        abort();
      }
    }
  }

  // Let's put our DEBUG_LEVELS here...
  for(int i = 0; i < 3; i++) {
    heaterPIDTask[i].DEBUG_PID = 0;
    stage2HeaterTask[i].DEBUG_LEVEL = 0;
    dutyCycleTask[i].DEBUG_DUTY_CYCLE = 0;
    //    tempRefreshTask[i].DEBUG = 0;
    stage2NetworkTask[i].DEBUG_UDP = 0;
  }

    core.DEBUG_CORE = 0;
    core._scheduler.DEBUG_SCHEDULER = 0;
    stage2_readTempsTask.DEBUG_READ_TEMPS = 0;

  // This is used to determine which machine will
  // be set by keyboard commands; you switch between them
  // with simple commands
  getConfig(0)->s2heater = Int1;
  getConfig(1)->s2heater = Ext1;
  getConfig(2)->s2heater = Ext2;
  for(int i = 0; i < 3; i++) {
    getConfig(i)->IS_STAGE2_HEATER_CONFIG = true;
  }

  // OxCore::TaskProperties serialProperties;
  // serialProperties.name = "serial";
  // serialProperties.id = 36;
  // serialProperties.period = 250;
  // serialProperties.priority = OxCore::TaskPriority::High;
  // serialProperties.state_and_config = (void *) getConfig(0);
  // bool serialAdd = core.AddTask(&stage2SerialTask, &serialProperties);
  // if (!serialAdd) {
  //   OxCore::Debug<const char *>("SerialProperties add failed\n");
  //   delay(100);
  //   abort();
  // }
  // stage2SerialTask.DEBUG_LEVEL = 2;
  // stage2SerialTask.hal = s2hal;


  OxCore::TaskProperties serialProperties;
  serialProperties.name = "stage2SerialInput";
  serialProperties.id = 39;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) getConfig(0);
  bool serialAdd = core.AddTask(&stage2SerialInputTask, &serialProperties);
  if (!serialAdd) {
    OxCore::Debug<const char *>("stage2SerialInputTask add failed\n");
    delay(100);
    abort();
  }
  stage2SerialInputTask.DEBUG_SERIAL = 0;
  for (int i = 0; i < 3; i++) {
    stage2SerialInputTask.mcs[i] = getConfig(i);
    stage2SerialInputTask.stage2HeaterTasks[i] = &stage2HeaterTask[i];
  }

  s2hal->s2heaterToControl = Int1;

  core.DEBUG_CORE = 0;

  OxCore::Debug<const char *>("Added tasks\n");
}

void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

  delay(100);
  // Blocking call
  if (core.Run() == false) {
    OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
    OxCore::Debug<const char *>("Aborting! TEST OVER\n");
    delay(300);
#ifdef ARDUINO
    // Loop endlessly to stop the program from running
    OxCore::Debug<const char *>("Aborting! TEST OVER\n");
    delay(300);
    abort();
    // while (true) {}
#endif
    return;
  } else {
    OxCore::Debug<const char *>("Run Completely\n");
  }

}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  loop();
  abort();
}
#endif
