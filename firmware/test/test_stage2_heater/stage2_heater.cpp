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

#include <Arduino.h>

#include <MAX31850.h>
#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <stage2_heater_task.h>
#include <stage2SerialReportTask.h>
#include <serial_task.h>


using namespace OxCore;
static Core core;

const unsigned long REPORT_PERIOD_MS = 5000;
TaskProperties _properties;
unsigned long time_of_last_report = 0;
MachineConfig *machineConfig;

// This is a key parameter, which should perhaps be moved to a specific
// default file to make it clearer!
const float STAGE2_DEFAULT_TEMP = 25.0;
const float STAGE2_OPERATING_TEMP = 750.0;
using namespace std;


using namespace OxApp;
ReadTempsTask readTempsTask;

HeaterPIDTask heaterPIDTask_ext1;
HeaterPIDTask heaterPIDTask_ext2;
HeaterPIDTask heaterPIDTask_int1;

DutyCycleTask dutyCycleTask_ext1;
DutyCycleTask dutyCycleTask_ext2;
DutyCycleTask dutyCycleTask_int1;

Stage2HeaterTask stage2HeaterTask_int1;
Stage2HeaterTask stage2HeaterTask_ext1;
Stage2HeaterTask stage2HeaterTask_ext2;

Stage2SerialReportTask stage2SerialReportTask;

SerialTask serialTask;

void setup() {

 OxCore::serialBegin(115200UL);
  delay(500);

  if (core.Boot() == false) {
    ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    // TODO: Output error message
    //return EXIT_FAILURE;
    return;
  }

  machineConfig = new MachineConfig();

  machineConfig->hal = new MachineHAL();

  machineConfig->ms = Off;
  machineConfig->s2sr->ms_int1 = Off;
  machineConfig->s2sr->ms_ext1 = Off;
  machineConfig->s2sr->ms_ext2 = Off;

  bool initSuccess  = machineConfig->hal->init();
  Serial.println("about to start!");
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
  }

  pinMode(RF_HEATER, OUTPUT);

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.state_and_config = (void *) machineConfig;
  delay(1000);
  Serial.println("About to run test!");

  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 20;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) machineConfig;
  delay(300);
  core.AddTask(&readTempsTask, &readTempsProperties);
  delay(100);

  OxCore::TaskProperties stage2SerialReportProperties;
  stage2SerialReportProperties.name = "stage2SerialReportTemps";
  stage2SerialReportProperties.id = 21;
  stage2SerialReportProperties.period = stage2SerialReportTask.PERIOD_MS;
  stage2SerialReportProperties.priority = OxCore::TaskPriority::High;
  stage2SerialReportProperties.state_and_config = (void *) machineConfig;


  bool stage2SerialReportAdd = core.AddTask(&stage2SerialReportTask, &stage2SerialReportProperties);
  if (!stage2SerialReportAdd) {
     OxCore::Debug<const char *>("stage2SerialReport Task add failed\n");
     abort();
  }

  OxCore::DebugLn<long>(machineConfig->s2sr->ms_int1);
  delay(100);
  OxCore::TaskProperties dutyCycleProperties_int1;
  dutyCycleProperties_int1.name = "dutyCycle_int1";
  dutyCycleProperties_int1.id = 23;
  dutyCycleProperties_int1.period = dutyCycleTask_int1.PERIOD_MS;
  dutyCycleProperties_int1.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties_int1.state_and_config = (void *) machineConfig;
  core.AddTask(&dutyCycleTask_int1, &dutyCycleProperties_int1);
  dutyCycleTask_int1.whichHeater = Int1;

  OxCore::TaskProperties dutyCycleProperties_ext1;
  dutyCycleProperties_ext1.name = "dutyCycle_ext1";
  dutyCycleProperties_ext1.id = 24;
  dutyCycleProperties_ext1.period = dutyCycleTask_ext1.PERIOD_MS;
  dutyCycleProperties_ext1.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties_ext1.state_and_config = (void *) machineConfig;
  core.AddTask(&dutyCycleTask_ext1, &dutyCycleProperties_ext1);
  dutyCycleTask_ext1.whichHeater = Ext1;

  OxCore::TaskProperties dutyCycleProperties_ext2;
  dutyCycleProperties_ext2.name = "dutyCycle_ext2";
  dutyCycleProperties_ext2.id = 25;
  dutyCycleProperties_ext2.period = dutyCycleTask_ext2.PERIOD_MS;
  dutyCycleProperties_ext2.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties_ext2.state_and_config = (void *) machineConfig;
  core.AddTask(&dutyCycleTask_ext2, &dutyCycleProperties_ext2);
  dutyCycleTask_ext1.whichHeater = Ext2;

  OxCore::TaskProperties HeaterPIDProperties_int1;
  HeaterPIDProperties_int1.name = "HeaterPID_int1";
  HeaterPIDProperties_int1.id = 26;
  HeaterPIDProperties_int1.period = heaterPIDTask_int1.PERIOD_MS;
  HeaterPIDProperties_int1.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties_int1.state_and_config = (void *) machineConfig;
  core.AddTask(&heaterPIDTask_int1, &HeaterPIDProperties_int1);
  heaterPIDTask_int1.whichHeater = Int1;

  OxCore::TaskProperties HeaterPIDProperties_ext1;
  HeaterPIDProperties_ext1.name = "HeaterPID_ext1";
  HeaterPIDProperties_ext1.id = 27;
  HeaterPIDProperties_ext1.period = heaterPIDTask_ext1.PERIOD_MS;
  HeaterPIDProperties_ext1.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties_ext1.state_and_config = (void *) machineConfig;
  core.AddTask(&heaterPIDTask_ext1, &HeaterPIDProperties_ext1);
  heaterPIDTask_int1.whichHeater = Ext1;

  OxCore::TaskProperties HeaterPIDProperties_ext2;
  HeaterPIDProperties_ext2.name = "HeaterPID_ext2";
  HeaterPIDProperties_ext2.id = 28;
  HeaterPIDProperties_ext2.period = heaterPIDTask_ext2.PERIOD_MS;
  HeaterPIDProperties_ext2.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties_ext2.state_and_config = (void *) machineConfig;
  core.AddTask(&heaterPIDTask_ext2, &HeaterPIDProperties_ext2);
  heaterPIDTask_int1.whichHeater = Ext1;

  heaterPIDTask_int1.dutyCycleTask = &dutyCycleTask_int1;
  heaterPIDTask_ext1.dutyCycleTask = &dutyCycleTask_ext1;
  heaterPIDTask_ext2.dutyCycleTask = &dutyCycleTask_ext2;

  OxCore::TaskProperties stage2HeaterProperties_int1;
  stage2HeaterProperties_int1.name = "stage2_int1";
  stage2HeaterProperties_int1.id = 29;
  stage2HeaterProperties_int1.period = heaterPIDTask_ext2.PERIOD_MS;
  stage2HeaterProperties_int1.priority = OxCore::TaskPriority::High;
  stage2HeaterProperties_int1.state_and_config = (void *) machineConfig;
  bool stage2HeaterTaskAdded = core.AddTask(&stage2HeaterTask_int1, &stage2HeaterProperties_int1);
  if (!stage2HeaterTaskAdded) {
    OxCore::Debug<const char *>("stage2 int1 Failed\n");
    abort();
  }
  stage2HeaterTask_int1.whichHeater = Int1;

  OxCore::TaskProperties stage2HeaterProperties_ext1;
  stage2HeaterProperties_ext1.name = "stage2_ext1";
  stage2HeaterProperties_ext1.id = 30;
  stage2HeaterProperties_ext1.period = heaterPIDTask_ext2.PERIOD_MS;
  stage2HeaterProperties_ext1.priority = OxCore::TaskPriority::High;
  stage2HeaterProperties_ext1.state_and_config = (void *) machineConfig;
  bool stage2HeaterTaskAdded_ext1 = core.AddTask(&stage2HeaterTask_ext1, &stage2HeaterProperties_ext1);
  if (!stage2HeaterTaskAdded_ext1) {
    OxCore::Debug<const char *>("stage2 ext1 Failed\n");
    abort();
  }
  stage2HeaterTask_ext1.whichHeater = Ext1;

  OxCore::TaskProperties stage2HeaterProperties_ext2;
  stage2HeaterProperties_ext2.name = "stage2_ext2";
  stage2HeaterProperties_ext2.id = 31;
  stage2HeaterProperties_ext2.period = heaterPIDTask_ext2.PERIOD_MS;
  stage2HeaterProperties_ext2.priority = OxCore::TaskPriority::High;
  stage2HeaterProperties_ext2.state_and_config = (void *) machineConfig;
  bool stage2HeaterTaskAdded_ext2 = core.AddTask(&stage2HeaterTask_ext2, &stage2HeaterProperties_ext2);
  if (!stage2HeaterTaskAdded_ext2) {
    OxCore::Debug<const char *>("stage2 ext2 Failedd\n");
    abort();
  }
  stage2HeaterTask_ext2.whichHeater = Ext2;

  stage2HeaterTask_int1.DEBUG_LEVEL = 1;
  stage2HeaterTask_ext1.DEBUG_LEVEL = 1;
  stage2HeaterTask_ext2.DEBUG_LEVEL = 1;

  stage2HeaterTask_int1.STAGE2_TARGET_TEMP = machineConfig->STAGE2_DEFAULT_TEMP_INT1;
  stage2HeaterTask_ext1.STAGE2_TARGET_TEMP = machineConfig->STAGE2_DEFAULT_TEMP_EXT1;
  stage2HeaterTask_ext2.STAGE2_TARGET_TEMP = machineConfig->STAGE2_DEFAULT_TEMP_EXT2;

  stage2HeaterTask_int1.STAGE2_OPERATING_TEMP = machineConfig->STAGE2_OPERATING_TEMP_INT1;
  stage2HeaterTask_ext1.STAGE2_OPERATING_TEMP = machineConfig->STAGE2_OPERATING_TEMP_EXT1;
  stage2HeaterTask_ext2.STAGE2_OPERATING_TEMP = machineConfig->STAGE2_OPERATING_TEMP_EXT2;


  OxCore::TaskProperties serialProperties;
  serialProperties.name = "serial";
  serialProperties.id = 32;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) &machineConfig;
  bool serialAdd = core.AddTask(&serialTask, &serialProperties);
  if (!serialAdd) {
    OxCore::Debug<const char *>("SerialProperties add failed\n");
    abort();
  }

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
