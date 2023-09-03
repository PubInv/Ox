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

// #include <MAX31850.h>
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

HeaterPIDTask heaterPIDTask[3];
DutyCycleTask dutyCycleTask[3];
Stage2HeaterTask stage2HeaterTask[3];

Stage2SerialReportTask stage2SerialReportTask;

Stage2SerialTask stage2SerialTask;

MachineConfig *getConfig() {
  return machineConfig;
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

  machineConfig = new MachineConfig();

  getConfig()->hal = new Stage2HAL();

  getConfig()->ms = Off;
  getConfig()->s2sr->ms[Int1] = Off;
  getConfig()->s2sr->ms[Ext1] = Off;
  getConfig()->s2sr->ms[Ext2] = Off;

  bool initSuccess  = getConfig()->hal->init();
  Serial.println("about to start!");
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
  }

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.state_and_config = (void *) getConfig();
  delay(1000);
  Serial.println("About to run test!");

  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 20;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) getConfig();
  delay(300);
  core.AddTask(&readTempsTask, &readTempsProperties);
  delay(100);

  OxCore::TaskProperties stage2SerialReportProperties;
  stage2SerialReportProperties.name = "stage2SerialReportTemps";
  stage2SerialReportProperties.id = 21;
  stage2SerialReportProperties.period = stage2SerialReportTask.PERIOD_MS;
  stage2SerialReportProperties.priority = OxCore::TaskPriority::High;
  stage2SerialReportProperties.state_and_config = (void *) getConfig();


  bool stage2SerialReportAdd = core.AddTask(&stage2SerialReportTask, &stage2SerialReportProperties);
  if (!stage2SerialReportAdd) {
     OxCore::Debug<const char *>("stage2SerialReport Task add failed\n");
     abort();
  }

  for(int i = 0; i < 3; i++) {
    OxCore::TaskProperties dutyCycleProperties;
    dutyCycleProperties.name = "dutyCycle";
    dutyCycleProperties.id = 23+i;
    dutyCycleProperties.period = dutyCycleTask[i].PERIOD_MS;
    dutyCycleProperties.priority = OxCore::TaskPriority::Low;
    dutyCycleProperties.state_and_config = (void *) getConfig();
    core.AddTask(&dutyCycleTask[i], &dutyCycleProperties);
    dutyCycleTask[i].whichHeater = (Stage2Heater) i;

    OxCore::TaskProperties HeaterPIDProperties;
    HeaterPIDProperties.name = "HeaterPID";
    HeaterPIDProperties.id = 26+i;
    HeaterPIDProperties.period = heaterPIDTask[i].PERIOD_MS;
    HeaterPIDProperties.priority = OxCore::TaskPriority::High;
    HeaterPIDProperties.state_and_config = (void *) getConfig();
    core.AddTask(&heaterPIDTask[i], &HeaterPIDProperties);
    heaterPIDTask[i].whichHeater = (Stage2Heater) i;

    dutyCycleTask[i].one_pin_heater = getConfig()->hal->_ac_heaters[i];

    OxCore::TaskProperties stage2HeaterProperties ;
    // I really need to do a concatenation here to make the name complete.
    stage2HeaterProperties.name = getConfig()->HeaterNames[i];
    stage2HeaterProperties.id = 29+i;
    stage2HeaterProperties.period = stage2HeaterTask[i].PERIOD_MS;
    stage2HeaterProperties.priority = OxCore::TaskPriority::High;
    stage2HeaterProperties.state_and_config = (void *) getConfig();
    bool stage2HeaterTaskAdded = core.AddTask(&stage2HeaterTask[i], &stage2HeaterProperties);
    if (!stage2HeaterTaskAdded) {
      OxCore::Debug<const char *>("stage add Failed\n");
      abort();
    }
    stage2HeaterTask[i].whichHeater = (Stage2Heater) i;

    stage2HeaterTask[i].STAGE2_TARGET_TEMP = getConfig()->TARGET_TEMP;
    stage2HeaterTask[i].STAGE2_OPERATING_TEMP = getConfig()->STAGE2_OPERATING_TEMP[i];
    getConfig()->s2sr->ms[i] = Off;

    heaterPIDTask[i].dutyCycleTask = &dutyCycleTask[i];
  }

  // Let's put our DEBUG_LEVELS here...
  for(int i = 0; i < 3; i++) {
    stage2HeaterTask[i].DEBUG_LEVEL = 1;
  }


  // This is used to determine which machine will
  // be set by keyboard commands; you switch between them
  // with simple commands
  getConfig()->s2heaterToControl = Int1;

  Serial.println("S2 Heater To Control:");
  Serial.println(getConfig()->s2heaterToControl);
  OxCore::TaskProperties serialProperties;
  serialProperties.name = "serial";
  serialProperties.id = 32;
  serialProperties.period = 250;
  serialProperties.priority = OxCore::TaskPriority::High;
  serialProperties.state_and_config = (void *) getConfig();
  bool serialAdd = core.AddTask(&stage2SerialTask, &serialProperties);
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
