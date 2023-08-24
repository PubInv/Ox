/*
Copyright (C) 2023 Robert Read.

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

#ifndef MACHINE_H
#define MACHINE_H

// Hardware Abstraction Layer
// #include "SensirionSFM3X00.h"
#include <SanyoAceB97.h>

#include <machine_script.h>

#define HAND_TEST 1

/* Connections:
   RibbonFish
   D2 - the fan
   D3 - the heater
   DAC0 - the stack
   D4 - MAX31850_DATA_PIN
*/

#ifdef ARDUINO
#include <Arduino.h>


#ifdef RIBBONFISH
#define RF_FAN 2
#define RF_HEATER 3
#define RF_STACK DAC0
#define MAX31850_DATA_PIN 4
#define RF_FAN_TACH 5
#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75

// This is the order in which the thermocouples are wired;
// in a perfect world we might use device address
#define POST_STACK_0_IDX 0
#define POST_HEATER_0_IDX 1

#endif
#endif

#include <machine_core_defs.h>

const static int NUM_FANS = 1;

class MachineHAL {
public:
  //  SensirionFlow *_flowsensor;
  SanyoAceB97 _fans[NUM_FANS];
  int DEBUG_HAL = 0;
  bool init();
  //  void _updateFanSpeed(float unitInterval);
};


class MachineConfig {
public:
  MachineConfig() {
    script = new MachineScript();
    report = new MachineStatusReport();
  };


  const float FAN_PER_CENT = 60.0;
  const int FAN_PWM = (int) (255.0*60.0/100.0);

  // TEST CONFIGURATION PARAMETERS
  // ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
  // THEY ARE "HARD_WIRED" HERE.
  // Edit these directly and re-upload to run a different test.
  // This test is designed to abort the processeor when done.

  static constexpr float RAMP_UP_TARGET_D_MIN = 0.5; // degrees C per minute
  static constexpr float RAMP_DN_TARGET_D_MIN = -0.5; // degrees C per minute

  // This is the overall target_temp, which changes over time.

  static constexpr float HOLD_TEMPERATURE = 600.0;
  static constexpr float STOP_TEMPERATURE = 27.0;
  static constexpr float START_TEMPERATURE = 600.0;

  float TARGET_TEMP = 27.0;

  static const unsigned long HOLD_TIME_MINUTES = 1;
  static const unsigned long HOLD_TIME_SECONDS = 60 * HOLD_TIME_MINUTES;
  static constexpr float STARTING_DUTY_CYCLE_FRACTION = 0.0;
  float STACK_VOLTAGE = 12.0;
  float STACK_AMPERAGE = 3.0;

  // These parameters are related to our control procedure.
  // This is similar to a PID loop, but I don't think any integration
  // is needed, and want to have a test that doesn't depend on a PID
  // loop. Arguably, that is just implementing a very simple PD loop
  // myself; I'll accept that rap.

  // Let DutyCycle be changing Duty cycle, a fraction of 1.0.
  // In PID termonology, this is the "PLANT VARIABLE"--what we can control.
  float GlobalDutyCycle = STARTING_DUTY_CYCLE_FRACTION;
  // Temperature Read Period is how often we will update the
  // Temperature sensor.
  static const int TEMPERATURE_READ_PERIOD_MS = 5000;
  // Duty Cycle Adjustment Period is how often we will adject
  const int DUTY_CYCLE_ADJUSTMENT_PERIOD_MS = 30000;
  // This is the number of periods around a point in time we will
  // average to produce a smooth temperature. (Our thermocouples have
  // only 0.25 C resolution, which is low for a 0.5C/minute control
  // situation!) These are always taken to be BACKWARD in time.
  const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
  // Ddelta is the change in temperature in C per min
  float Ddelta_C_per_min = 0.0;
  // This is period of time we will use to compute the Ddela_C_per_min.
  // Note this does not have to be related to the DUTY_CYCLE_ADJUSTMENT_PERIOD_MS


  const long FAKE_NUMBER_OF_DUTY_CYCLES_TO_RUN = 100000;

  int num_duty_cycles = 0;

  const int DUTY_CYCLE_COMPUTATION_TIME_MS = 30*1000;


  // Our AC heater...confusingly named!
  const int NUM_HEATERS = 1;
  const int NUM_STACKS = 1;

  float MAXIMUM_HEATER_VOLTAGE = 12.0;
  float MAXIMUM_STACK_VOLTAGE = 8.0;
  float MAXIMUM_STACK_AMPS = 1.0;
  // These values are useful for testing by hand
#ifdef HAND_TEST
  float COOLDOWN_TARGET_C = 27.0;
  float WARMUP_TARGET_C = 150.0;
  float MAX_POST_HEATER_C = 150.0;
  float TARGET_STACK_C = 150.0;
  float MAX_POST_STACK_C = 180.0;
  float TARGET_STACK_CURRENT_mA = 1.0;

  // I'm not sure what the real value here should be!
  // but to activate the fans we have to make this
  // higher than 500 ml/s, because the fans seem to
  // produce that at idle!
  // float TARGET_FLOW_ml_per_S = 1000.0;
#else
  float COOLDOWN_TARGET_C = 26.0;
  float WARMUP_TARGET_C = 600.0;
  float DESIRED_STACK_C = 700.0;
#endif

  void _updateFanSpeed(float unitInterval);

  static const int NUM_MACHINE_STATES = 8;

  constexpr inline static char const *MachineStateNames[8] = {
    "Off",
    "Warmup",
    "NormalOperation",
    "Cooldown",
    "CriticalFault",
    "EmergencyShutdown",
    "OffUserAck"
  };
  constexpr inline static char const *MachineSubStateNames[2] = {
    "(Not Idling)",
    "(Idling)"
  };
  constexpr inline static char const *TempLocationNames[2] = {
    "Post Heater",
    "Post Stack"
  };
  MachineState ms;
  MachineHAL* hal;
  MachineScript* script;

  IdleOrOperateSubState idleOrOperate = Operate;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usuall 0-255) but this should be
  // the last step.
  float dutyCycle = 0.0;

  char const* errors[10];
  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};
  int post_getter_indices[1] = {2};


  MachineStatusReport *report;

};

void outputReport(MachineStatusReport *msr);
void createJSONReport(MachineStatusReport *msr, char *buffer);


#endif
