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
#include <OnePinHeater.h>

#include <machine_script.h>
#include <stage2_config.h>


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
// This should change to PIN 5 when
// we get the planned control board.
#define MAX31850_DATA_PIN 4
//#define MAX31850_DATA_PIN 5
// #define RF_FAN_TACH 5
#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75

// This is the order in which the thermocouples are wired;
// in a perfect world we might use device address
#define POST_STACK_0_IDX 0
#define POST_HEATER_0_IDX 1

#endif
#endif

#include <machine_core_defs.h>


class MachineHAL {
public:
  int NUM_HEATERS;
  OnePinHeater **_ac_heaters;
  int DEBUG_HAL = 0;
  bool init_heaters();
  virtual bool init() = 0;
};



class MachineConfig {
public:
  MachineConfig();

  //  static const int NUM_STACKS = 1;


  // TEST CONFIGURATION PARAMETERS
  // ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
  // THEY ARE "HARD_WIRED" HERE.
  // Edit these directly and re-upload to run a different test.
  // This test is designed to abort the processeor when done.

  static constexpr float RAMP_UP_TARGET_D_MIN = 0.5; // degrees C per minute
  static constexpr float RAMP_DN_TARGET_D_MIN = -0.5; // degrees C per minute
  unsigned long BEGIN_DN_TIME_MS;
  unsigned long BEGIN_UP_TIME_MS;

  // This is the overall target_temp, which changes over time.

  static constexpr float YELLOW_TEMP = 760.0;
  static constexpr float RED_TEMP = 780.0;
  static constexpr float OPERATING_TEMP = 740.0;
  static constexpr float STOP_TEMP = 27.0;
  static constexpr float MAX_CROSS_STACK_TEMP = 40.0;

  static constexpr float TEMP_REFRESH_LIMIT = 40.0;

  static constexpr float HIGH_TEMP_FAN_SLOW_DOWN_LIMIT = 400.0;

  float COOL_DOWN_BEGIN_TEMP;

  float TARGET_TEMP = 30.0;

  // TODO: This would better be attached to the statemanager
  // class, as it is used in those task---but also in the
  // separate temp_refresh_task. Until I can refactor
  // temp_refresh_task by placing its funciton in the
  // state manager, I need this gloabl.
  float GLOBAL_RECENT_TEMP;

  static const unsigned long HOLD_TIME_MINUTES = 1;
  static const unsigned long HOLD_TIME_SECONDS = 60 * HOLD_TIME_MINUTES;
  static constexpr float STARTING_DUTY_CYCLE_FRACTION = 0.0;

  // AMPERAGE CONTROL
  static constexpr float MAX_AMPERAGE = 60.0;
  float STACK_VOLTAGE = 12.0;
  static constexpr float IDLE_STACK_VOLTAGE = 1.0;
  static constexpr float MIN_OPERATING_STACK_VOLTAGE = 7.0;

  // FAN CONTROL
  static constexpr float FULL_POWER_FOR_FAN = 0.6;
  static constexpr float FAN_SPEED_AT_OPERATING_TEMP = 0.3;
  static constexpr float TEMP_TO_BEGIN_FAN_SLOW_DOWN = 500;
  static constexpr float END_FAN_SLOW_DOWN = OPERATING_TEMP + 25.0;

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
  static const int TEMP_READ_PERIOD_MS = 5000;
  // Duty Cycle Adjustment Period is how often we will adject
  const int DUTY_CYCLE_ADJUSTMENT_PERIOD_MS = 30000;
  // This is the number of periods around a point in time we will
  // average to produce a smooth temperature. (Our thermocouples have
  // only 0.25 C resolution, which is low for a 0.5C/minute control
  // situation!) These are always taken to be BACKWARD in time.
  const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
  // Ddelta is the change in temperature in C per min
  float Ddelta_C_per_min = 0.0;

  int num_duty_cycles = 0;

  // we will perform our duty cycle computation of 30 seconds...
  const int DUTY_CYCLE_COMPUTATION_TIME_MS = 30*1000;

  // These need more study
  float MAXIMUM_STACK_VOLTAGE = 8.0;
  float MAXIMUM_STACK_AMPS = 12.0;
  // These values are useful for testing by hand

  float COOLDOWN_TARGET_C = 27.0;

  //  void _updateFanPWM(float unitInterval);
  void _reportFanSpeed();

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

  constexpr inline static char const *HeaterNames[3] = {
    "Int1",
    "Ext1",
    "Ext2"
  };

  MachineState ms;
  // This is used to make decisions that happen at transition time.
  MachineState previous_ms;
  // This should be an enum, but I am having a lot of problems with it, I am going to
  // try using an int...
  Stage2Heater s2heaterToControl;

  MachineScript* script;

  IdleOrOperateSubState idleOrOperate = Operate;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usuall 0-255) but this should be
  // the last step.
  float fanDutyCycle = 0.0;

  char const* errors[10];
  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};
  int post_getter_indices[1] = {2};

  MachineHAL* hal;
  MachineStatusReport *report;

  void outputReport(MachineStatusReport *msr);
  void createJSONReport(MachineStatusReport *msr, char *buffer);

  // Stage2 specific stuff; this should be handled
  // as a subclass, not a decorator, but I don't have time for that,
  // and it puts the main code at risk, so adding it in here is
  // reasonable - rlr
  Stage2StatusReport *s2sr;
  // This is used by the Serial listener to control which
  // state machine/heater/thermocouple we are controlling

  float STAGE2_OPERATING_TEMP[3];

  unsigned long STAGE2_BEGIN_UP_TIME[3];
  unsigned long STAGE2_BEGIN_DN_TIME[3];

  void outputStage2Report(Stage2StatusReport *msr);
  void createStage2JSONReport(Stage2StatusReport *msr, char *buffer);

};


#endif
