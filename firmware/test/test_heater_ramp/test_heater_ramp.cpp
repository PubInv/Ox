/*
  test_heater_ramp -- A program to test the the ability to ramp up, hold, and ramp down.

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
#include <GGLabsSSR1.h>
#include <TF800A12K.h>

using namespace OxCore;


#include <PID_v1.h>

#include <core_defines.h>
#include <core.h>
#include <machine.h>

#include <SanyoAceB97.h>
#include <Arduino.h>

#ifdef USE_MAX31850_THERMOCOUPLES
#include <MAX31850.h>
#else
#include <DS18B20_temperature.h>
#endif

// We aren't using this, but it brings in declarations we need!
#include <cog_task.h>

#include <duty_cycle_task.h>

#include <heater_pid_task.h>

#include <read_temps_task.h>


using namespace OxCore;
static Core core;

const unsigned long REPORT_PERIOD_MS = 3000;

MachineConfig *machineConfig;
TaskProperties _properties;
SanyoAceB97 *fan;

// This exists purely to be compatible with the code
// in the make task library
MachineConfig *localGetConfig() {
  return machineConfig;
}

unsigned long time_of_last_report = 0;

//const float FAN_PER_CENT = 60.0;
// const int FAN_PWM = (int) (255.0*60.0/100.0);

// TEST CONFIGURATION PARAMETERS
// ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
// THEY ARE "HARD_WIRED" HERE.
// Edit these directly and re-upload to run a different test.
// This test is designed to abort the processeor when done.

// const float RAMP_UP_TARGET_D_MIN = 0.5; // degrees C per minute
// const float RAMP_DN_TARGET_D_MIN = -0.5; // degrees C per minute

// // This is the overall target_temp, which changes over time.

// const float HOLD_TEMPERATURE = 600.0;
// const float STOP_TEMPERATURE = 27.0;
// const float START_TEMPERATURE = 600.0;

// float TARGET_TEMP = 27.0;

// const unsigned long HOLD_TIME_MINUTES = 1;
// const unsigned long HOLD_TIME_SECONDS = 60 * HOLD_TIME_MINUTES;
// const float STARTING_DUTY_CYCLE_FRACTION = 0.0;
// const float STACK_VOLTAGE = 12.0;
// const float STACK_AMPERAGE = 3.0;

// These parameters are related to our control procedure.
// This is similar to a PID loop, but I don't think any integration
// is needed, and want to have a test that doesn't depend on a PID
// loop. Arguably, that is just implementing a very simple PD loop
// myself; I'll accept that rap.

// Let DutyCycle be changing Duty cycle, a fraction of 1.0.
// In PID termonology, this is the "PLANT VARIABLE"--what we can control.
//float GlobalDutyCycle = MachineConfig::STARTING_DUTY_CYCLE_FRACTION;
// Temperature Read Period is how often we will update the
// Temperature sensor.
//const int TEMPERATURE_READ_PERIOD_MS = 5000;
// Duty Cycle Adjustment Period is how often we will adject
//const int DUTY_CYCLE_ADJUSTMENT_PERIOD_MS = 30000;
// This is the number of periods around a point in time we will
// average to produce a smooth temperature. (Our thermocouples have
// only 0.25 C resolution, which is low for a 0.5C/minute control
// situation!) These are always taken to be BACKWARD in time.
// const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
// const int TEMPERATRUE_TIME_DELTA_MS = 60000;
//
// const float MAXIMUM_CHANGE_IN_DUTY_CYCLE_PER_MIN = 1.0 / 100.0;

// const int NUM_TEMPS_TO_RECORD = ceil((((float) MachineConfig::TEMPERATRUE_TIME_DELTA_MS / (float) TEMPERATURE_READ_PERIOD_MS) +MachineConfig::NUMBER_OF_PERIODS_TO_AVERAGE));


//const long FAKE_NUMBER_OF_DUTY_CYCLES_TO_RUN = 100000;

// int num_duty_cycles = 0;

// const int DUTY_CYCLE_COMPUTATION_TIME_MS = 30*1000;


// GGLabsSSR1* _ac_heaters[NUM_HEATERS];


AbstractPS* _stacks[MachineConfig::NUM_STACKS];

// This is copied directly from cog_task.cpp...
void _updateStackVoltage(float voltage,MachineConfig *machineConfig) {
  for (int i = 0; i < MachineConfig::NUM_STACKS; i++) {
    _stacks[i]->updateVoltage(voltage,machineConfig);
  }
}
void _updateStackAmperage(float amperage,MachineConfig *machineConfig) {
  for (int i = 0; i < MachineConfig::NUM_STACKS; i++) {
    _stacks[i]->updateAmperage(amperage,machineConfig);
  }
}


// SpudTask *spudTask;

const int DEBUG_LEVEL = 2;
using namespace std;

namespace OxApp
{

  enum State {RampingUp, Holding, RampingDn};
  State global_state = RampingUp;

  class FanReportTask : public OxCore::Task
  {
  public:
    FanReportTask();
    const int PERIOD_MS = 3000;
    const int DEBUG_FAN_TASK = 1;
    SanyoAceB97 *fan;
  private:
    bool _init() override;
    bool _run() override;
  };
  FanReportTask::FanReportTask() {
  }

  bool FanReportTask::_init()
  {
    OxCore::Debug<const char *>("FanReportTask init\n");
    return true;
  }

  bool FanReportTask::_run()
  {
    if (DEBUG_FAN_TASK > 0) {
      fan->update(0.5);
    }
  }
  // This task controls the ramp up, hold, and ramp down.
  class SupervisorTask : public OxCore::Task
  {
  public:
    SupervisorTask();
    const int DEBUG_ID = 0;
    const int PERIOD_MS = 10000;
    //    Global_State global_state = RampingUp;
    unsigned long begin_hold_time;
    unsigned long begin_down_time;
  private:
    bool _init() override;
    bool _run() override;
  };
  SupervisorTask::SupervisorTask() {
  }

  bool SupervisorTask::_init()
  {
    OxCore::Debug<const char *>("SupervisorTask init\n");
    return true;
  }

  bool SupervisorTask::_run()
  {
    outputReport(getConfig()->report);
    // This should not be necessary here, it should be required only once...
    _updateStackVoltage(machineConfig->STACK_VOLTAGE,machineConfig);
    _updateStackAmperage(machineConfig->STACK_AMPERAGE,machineConfig);

    const unsigned long ms = millis();
    switch (global_state) {
    case RampingUp: {
      analogWrite(fan->PWM_PIN[0],153);
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: RAMPING UP\n");
      }
      float postHeaterTemp = localGetConfig()->report->post_heater_C;
      if (postHeaterTemp > MachineConfig::HOLD_TEMPERATURE) {
        global_state = Holding;
        OxCore::Debug<const char *>("State Changing to HOLDING\n");
        begin_hold_time = millis();
      } else {
        const unsigned long MINUTES_RAMPING_UP = ms / (60 * 1000);
        localGetConfig()->TARGET_TEMP = MachineConfig::START_TEMPERATURE + MINUTES_RAMPING_UP * MachineConfig::RAMP_UP_TARGET_D_MIN;
        localGetConfig()->TARGET_TEMP = min(localGetConfig()->TARGET_TEMP, MachineConfig::HOLD_TEMPERATURE);
      }
      break;
    };
    case Holding: {
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: HOLDING\n");
      }
      unsigned long ms = millis();
      if (((ms - begin_hold_time) / 1000) > MachineConfig::HOLD_TIME_SECONDS) {
        global_state = RampingDn;
        begin_down_time = ms;
        OxCore::Debug<const char *>("State: Changing to RAMPING DN\n");
      } else {
        const unsigned long MINUTES_HOLDING = ms / (60 * 1000);
      }
      break;
    };
    case RampingDn: {
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: RAMPING DN\n");
      }
      float postHeaterTemp = localGetConfig()->report->post_heater_C;
      if (postHeaterTemp < MachineConfig::STOP_TEMPERATURE) {
        analogWrite(fan->PWM_PIN[0],5);
        OxCore::Debug<const char *>("Stop temperature reached!\n");
        OxCore::Debug<const char *>("=======================\n");
        delay(1000);
        while(1);
      } else {
        const unsigned long MINUTES_RAMPING_DN = (ms - begin_down_time) / (60 * 1000);
        localGetConfig()->TARGET_TEMP =
          MachineConfig::HOLD_TEMPERATURE + MINUTES_RAMPING_DN * MachineConfig::RAMP_DN_TARGET_D_MIN;
        localGetConfig()->TARGET_TEMP = max(localGetConfig()->TARGET_TEMP,MachineConfig::STOP_TEMPERATURE);
      }
      break;
    };
    }
    return true;
  }
}

using namespace OxApp;
FanReportTask fanReportTask;
ReadTempsTask readTempsTask;
SupervisorTask supervisorTask;
// ControllerTask controllerTask;
HeaterPIDTask heaterPIDTask;


void setup() {

 OxCore::serialBegin(115200UL);
  delay(500);

  if (core.Boot() == false) {
    ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    // TODO: Output error message
    //return EXIT_FAILURE;
    return;
  }
  Serial.println("AAA!");
  machineConfig = new MachineConfig();
  machineConfig->hal = new MachineHAL();
  Serial.println("BBB!");

  heaterPIDTask.HeaterSetPoint_C = localGetConfig()->TARGET_TEMP;



   Serial.println("CCC!");

  // bool initSuccess  = localGetConfig()->hal->init();
  // Serial.println("about to start!");
  // if (!initSuccess) {
  //   Serial.println("Could not init Hardware Abastraction Layer Properly!");
  // }

  _stacks[0] = new SL_PS("FIRST_STACK",0);

  _stacks[0]->init();


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


  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.state_and_config = (void *) localGetConfig();
  delay(1000);
  Serial.println("About to run test!");
  delay(100);
  fan = new SanyoAceB97("FIRST_FAN",0,RF_FAN,1.0);
  Serial.println("Fan Set up");
  fan->_init();
  Serial.println("fan init done!");
  fan->DEBUG_FAN = 0;


  pinMode(fan->PWM_PIN[0], OUTPUT);
  // It is safer to be completely off until we are ready to start!
  analogWrite(fan->PWM_PIN[0],153);

  Serial.println("Fan Set up");
  delay(100);

  fanReportTask.fan = fan;
  OxCore::TaskProperties fanReportProperties;
  fanReportProperties.name = "fanReport";
  fanReportProperties.id = 19;
  fanReportProperties.period = fanReportTask.PERIOD_MS;
  fanReportProperties.priority = OxCore::TaskPriority::High;
  fanReportProperties.state_and_config = (void *) localGetConfig();
  delay(300);
  core.AddTask(&fanReportTask, &fanReportProperties);
  delay(100);


  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 21;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) localGetConfig();
  delay(300);
  core.AddTask(&readTempsTask, &readTempsProperties);
  delay(100);

  OxCore::TaskProperties supervisorProperties;
  supervisorProperties.name = "supervisor";
  supervisorProperties.id = 22;
  supervisorProperties.period = supervisorTask.PERIOD_MS;
  supervisorProperties.priority = OxCore::TaskPriority::High;
  supervisorProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(&supervisorTask, &supervisorProperties);



  DutyCycleTask dutyCycleTask;
  //  dutyCycleTask = new DutyCycleTask();
  heaterPIDTask.dutyCycleTask = &dutyCycleTask;

  OxCore::Debug<const char *>("DDD\n");

  OxCore::TaskProperties dutyCycleProperties;
  dutyCycleProperties.name = "dutyCycle";
  dutyCycleProperties.id = 23;
  OxCore::Debug<const char *>("period:");
  OxCore::Debug<int>(dutyCycleTask.PERIOD_MS);
  OxCore::Debug<const char *>("\n");
  dutyCycleProperties.period = dutyCycleTask.PERIOD_MS;
  dutyCycleProperties.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(&dutyCycleTask, &dutyCycleProperties);

  OxCore::TaskProperties HeaterPIDProperties;
  HeaterPIDProperties.name = "HeaterPID";
  HeaterPIDProperties.id = 24;
  HeaterPIDProperties.period = heaterPIDTask.PERIOD_MS;
  HeaterPIDProperties.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(&heaterPIDTask, &HeaterPIDProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  _updateStackVoltage(machineConfig->STACK_VOLTAGE,machineConfig);
  _updateStackAmperage(machineConfig->STACK_AMPERAGE,machineConfig);
  analogWrite(fan->PWM_PIN[0],153);

  Serial.println("Setup Done!");

  OxCore::Debug<const char *>("AAA NUM_HEATERS: ");
  OxCore::DebugLn<int>(MachineConfig::NUM_HEATERS);

}



void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

  // Blocking call
  if (core.Run() == false) {
    OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
    // Loop endlessly to stop the program from running
    OxCore::Debug<const char *>("Aborting! TEST OVER\n");
    delay(300);
    abort();
    // while (true) {}
#endif
    return;
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
