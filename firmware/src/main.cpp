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

// Program information
#define COMPANY_NAME "pubinv.org "
//#define PROG_NAME "main.cpp"
#define PROG_NAME "OEDCS"
#define VERSION "; Rev: 0.3.6"  // Pathfinder Relase Candidate (PID changes)
#define DEVICE_UNDER_TEST "Hardware: Due"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#ifdef ARDUINO
#include <Arduino.h>
//#include <display.h>
//
#endif

#include <core.h>
#include <cog_hal.h>

#include <flash.h>
#include <network_task.h>
#include <cog_task.h>
#include <serial_input_task.h>
#include <fault_task.h>
#include <duty_cycle_task.h>
#include <heater_pid_task.h>
#include <read_temps_task.h>
#include <serialReportTask.h>
#include <OEDCSNetworkTask.h>

#ifdef TEST_FANS_ONLY
#include <fanTEST_task.h>
#endif

using namespace OxCore;
static Core core;

/***** Declare your tasks here *****/

OxApp::OEDCSNetworkTask OEDCSNetworkTask;
OxApp::CogTask cogTask;
OxApp::OEDCSSerialInputTask oedcsSerialInputTask;
OxApp::FaultTask faultTask;

HeaterPIDTask heaterPIDTask;
DutyCycleTask dutyCycleTask;
ReadTempsTask readTempsTask;
SerialReportTask serialReportTask;

#include <machine.h>

MachineConfig machineConfig;
/***********************************/

#define ETHERNET_BOARD_PRESENT 1
// #define ETHERNET_BOARD_PRESENT 0 //No ethernet.


// This is to allow a code idiom compatible with the way
// the machine config is found  inside the Tasks
MachineConfig *getConfig() {
  return &machineConfig;
}

// TODO: we need to have setups for individual pieces
// of the Hardware Abstraction Layer
// I don't know why this didn't work inside the core.cpp file!!!


void setup()
{
  OxCore::serialBegin(115200UL);
  delay(500);

   // WARNING! need 5 second delay for pio compiler it seems
  // DO NOT REMOVE THIS STATEMENT!
  delay(5000);


  // We're doing this here because the Core may not be initialized
  watchdogReset();


  // TODO: consider doing this....
    // Serial.begin(BAUDRATE);
    // while (!Serial) {
    //   watchdogReset();
    // }
    // Serial.println(F("starting"));

  //Debug<const char *>("Starting Ox...\n");
  Debug<const char *>("Starting ");
  Debug<const char *>(PROG_NAME);
  Debug<const char *>(VERSION);
  Debug<const char *>("\n");

  Debug<const char *>("Build: ");
  Debug<const char *>((__DATE__ " " __TIME__));
  Debug<const char *>("\n");

   //Print out the reset reason
  Serial.println("=================");
  Serial.print("ResetCause: ");
  switch(getResetCause()) {
  case 0: Serial.println("general"); break;
  case 1: Serial.println("backup"); break;
  case 2: Serial.println("watchdog"); break;
  case 3: Serial.println("software"); break;
  case 4: Serial.println("user"); break;
  }
  Serial.println("=================");

  // TODO: consider doing this....
    // Serial.begin(BAUDRATE);
    // while (!Serial) {
    //   watchdogReset();
    // }
    // Serial.println(F("starting"));


  delay(100);
  if (core.Boot() == false) {
      ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
      // TODO: Output error message
      //return EXIT_FAILURE;
      return;
  }
  Debug<const char *>("Core booted...\n");
  delay(100);

  core.ResetHardwareWatchdog();

  machineConfig.init();
  //  Eventually we will migrate all hardware to the COG_HAL..
  COG_HAL* hal = new COG_HAL();
  machineConfig.hal = hal;

  machineConfig.hal->DEBUG_HAL = 0;
  bool initSuccess  = machineConfig.hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
    delay(50);
    abort();
  } else {
    Serial.println("Successful init of Hardware Abastraction Layer!");
  }

  // Now we will set the machine state to "Off"
  getConfig()->ms = Off;

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

  getConfig()->ms = Off;
  cogTask.heaterPIDTask = &heaterPIDTask;


  OxCore::TaskProperties oedcsSerialProperties;
  oedcsSerialProperties.name = "oedcsSerial";
  oedcsSerialProperties.id = 22;
  oedcsSerialProperties.period = oedcsSerialInputTask.PERIOD_MS;
  oedcsSerialProperties.priority = OxCore::TaskPriority::High;
  oedcsSerialProperties.state_and_config = (void *) &machineConfig;
   bool oedcsSerialAdd = core.AddTask(&oedcsSerialInputTask, &oedcsSerialProperties);
  if (!oedcsSerialAdd) {
    OxCore::Debug<const char *>("SerialInputProperties add failed\n");
    abort();
  }
  oedcsSerialInputTask.cogTask = &cogTask;

  if (ETHERNET_BOARD_PRESENT) {
    OxCore::TaskProperties OEDCSNetworkProperties;
    OEDCSNetworkProperties.name = "OEDCSNetwork";
    OEDCSNetworkProperties.id = 24;
    OEDCSNetworkProperties.period = 5000;
    OEDCSNetworkProperties.priority = OxCore::TaskPriority::High;
    OEDCSNetworkProperties.state_and_config = (void *) &machineConfig;

    bool OEDCSNetwork = core.AddTask(&OEDCSNetworkTask, &OEDCSNetworkProperties);
    if (!OEDCSNetwork) {
      OxCore::Debug<const char *>("Retrieve Script UDP\n");
      abort();
    }
  }

  dutyCycleTask.whichHeater = (Stage2Heater) 0;

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
  HeaterPIDProperties.period = MachineConfig::INIT_PID_PERIOD_MS;
  HeaterPIDProperties.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties.state_and_config = (void *) &machineConfig;
  bool heaterPIDAdd = core.AddTask(&heaterPIDTask, &HeaterPIDProperties);

  if (!heaterPIDAdd) {
    OxCore::Debug<const char *>("heaterPIDAdd Faild\n");
    abort();
  }

  core.ResetHardwareWatchdog();


  heaterPIDTask.whichHeater = (Stage2Heater) 0;

  heaterPIDTask.dutyCycleTask = &dutyCycleTask;

  cogTask.heaterPIDTask = &heaterPIDTask;

  core.DEBUG_CORE = 0;
  core._scheduler.DEBUG_SCHEDULER = 0;
  dutyCycleTask.DEBUG_DUTY_CYCLE = 0;
  heaterPIDTask.DEBUG_PID = 0;
  cogTask.DEBUG_LEVEL = 0;
  OEDCSNetworkTask.DEBUG_UDP = 0;
  OEDCSNetworkTask.net_udp.DEBUG_UDP = 0;
  readTempsTask.DEBUG_READ_TEMPS = 0;
  oedcsSerialInputTask.DEBUG_SERIAL = 0;
  getConfig()->script->DEBUG_MS = 0;
  OxCore::Debug<const char *>("Added tasks\n");

  // Now we will set the initial tunings for the heater_pid tasks
  // This is a place where one could change the settings for
  // one of the heaters but not another.

  heaterPIDTask.SetTunings(hal->INIT_Kp, hal->INIT_Ki, hal->INIT_Kd);


  // We want to make sure we have run the temps before we start up.

  readTempsTask._run();
  readTempsTask._run();
  readTempsTask._run();
  getConfig()->GLOBAL_RECENT_TEMP = getConfig()->report->post_heater_C;
  Serial.print("starting temp is: ");
  Serial.println(getConfig()->GLOBAL_RECENT_TEMP);
  OxCore::Debug<const char *>("Starting\n");
  /*********************************************/
}


void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

  // Blocking call
  if (core.Run() == false) {
      OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
      // make sure we print anything needed!
      Serial.println("Critical error!");
      delay(100);
      // Loop endlessly to stop the program from running
      Serial.println("INTERNAL ERROR (CORE RETURNED)!");
      delay(1000);
      abort();
#endif
      return;
  } else {
    Serial.println("INTERNAL ERROR (CORE DID NOT START)!");
    delay(300000);
    abort();
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
