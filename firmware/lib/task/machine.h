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



#define HAND_TEST 1

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef RIBBONFISH
// #define RF_FAN 2
#define RF_HEATER 3
#define RF_STACK DAC0
#define MAX31850_DATA_PIN 5
// #define RF_FAN_TACH 5
// This is obsolete
#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER

#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75
// This is the order in which the thermocouples are wired;
// in a perfect world we might use device address
#define POST_STACK_0_IDX 0
#define POST_HEATER_0_IDX 1

#elif STAGE2_HEATER

// WARNING! These values are obsolete.
// There is probably no real dependence on these.
// They should be hunted down and removed.
#define MAX31850_DATA_PIN 5
// This is obsolete
#define THERMOCOUPLE_PIN MAX31850_DATA_PIN //DIFFERENT FOR STAGE2_HEATER
// #define RF_FAN DAC1 //DIFFERENT FOR STAGE2_HEATER
#define RF_STACK DAC0
#define RF_MOSTPLUS_FLOW_PIN A0
#define RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE 1.75

#endif


#include <machine_core_defs.h>

#ifdef STAGE2_HEATER
#include <stage2_config.h>
#endif


class MachineHAL {
public:
  int NUM_HEATERS;
  OnePinHeater **_ac_heaters;
  int DEBUG_HAL = 0;
  Stage2Heater s2heaterToControl = Int1;
  int *HEATER_PINS;
  bool init_heaters();
  virtual bool init() = 0;
};

#define NUM_CRITICAL_ERROR_DEFINITIONS 3
enum CriticalErrorCondition {
  POST_HEATER_TC_BAD,
  POST_GETTER_TC_BAD,
  POST_STACK_TC_BAD
};

class CriticalError {
public:
  bool fault_present;
  unsigned long begin_condition_ms;
  unsigned long toleration_ms;
  MachineState response_state;
};

class MachineConfig {
public:
  MachineConfig();

  CriticalError errors[NUM_CRITICAL_ERROR_DEFINITIONS];

  // TEST CONFIGURATION PARAMETERS
  // ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
  // THEY ARE "HARD_WIRED" HERE.
  // Edit these directly and re-upload to run a different test.
  // This test is designed to abort the processeor when done.


  // These are the so called "5 knob" parameters that
  // can be dynamically changed through the serial port.

  // I am going to have these two move together as one.
  // There is only one Ramp parameter, even though we use
  // two numbers
  float RAMP_UP_TARGET_D_MIN = 0.5; // R (degrees C per minute)
  float RAMP_DN_TARGET_D_MIN = -0.5; // R (degrees C per minute)
  void change_ramp(float ramp);
  float TARGET_TEMP_C = 30.0; // This is the goal target
  float MAX_AMPERAGE = 0.0; // A (Amperes)
  float MAX_STACK_WATTAGE = 0.0; // W (Wattage)
  float FAN_SPEED = 0.0; // F (fraction between 0.0 and 1.0)

  unsigned long BEGIN_DN_TIME_MS = 0;
  unsigned long BEGIN_UP_TIME_MS = 0;

  float SETPOINT_TEMP_C = 30.0; // This is the CURRENT setpoint, which ramps up or down to TARGET_TEMP.

  // These are bounds; we won't let values go outside these.
  // They can only be changed here and forcing a recompilation.
  const float BOUND_MAX_TEMP = 750.0;
  const float BOUND_MIN_TEMP = 25.0;
  const float BOUND_MAX_AMPERAGE_SETTING = 60.0;
  const float BOUND_MAX_WATTAGE = 300.0;
  const float BOUND_MAX_RAMP = 3.0;

  // The beginning temperature of the current warming
  // or cooling cycle.
   float COOL_DOWN_BEGIN_TEMP;
   float WARM_UP_BEGIN_TEMP;


  const unsigned long THERMOCOUPLE_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;

  // TODO: This would better be attached to the statemanager
  // class, as it is used in those task---but also in the
  // separate temp_refresh_task. Until I can refactor
  // temp_refresh_task by placing its funciton in the
  // state manager, I need this gloabl.
  float GLOBAL_RECENT_TEMP = 30.0;

  const float MAX_STACK_VOLTAGE = 12.0;
  static constexpr float IDLE_STACK_VOLTAGE = 1.0;
  static constexpr float MIN_OPERATING_STACK_VOLTAGE = 1.0;
  //
  // Note: The MAX31850, OneWire system, and the MAX31855, both,
  // can not read reliably faster than 100ms.
  // We have tested the TEMP_READ_PERIOD_MS at 100,
  // but see no reason for it to be that fast.
  // At present the code does not really use or log readings
  // that are faster than the heater PID task period, so
  // there is no reason for it to be more than twice as fast as that.
  // Please refer to the documentation here:
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31850-MAX31851.pdf
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31855.pdf
  // Please make sure that the INIT_PID_PERIOD_MS is more than
  // the TEMP_READ_PERIOD_MS.
  static const int TEMP_READ_PERIOD_MS = 225; // this is intentionally a little less than half the PID PERIOD
  static const int INIT_PID_PERIOD_MS = 500;

  static const int INIT_HEARTBEAT_PERIOD_MS = 500;

  static const int INIT_POWERMONITOR_PERIOD_MS = 500;

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
  bool IS_STAGE2_HEATER_CONFIG = false;
  Stage2Heater s2heater;

  MachineScript* script;

  IdleOrOperateSubState idleOrOperate = Operate;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usually 0-255) but this should be
  // the last step.
  float fanDutyCycle = 0.0;

  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};
  int post_getter_indices[1] = {2};

  MachineHAL* hal;
  MachineStatusReport *report;

  bool init();

  void outputReport(MachineStatusReport *msr);
  void createJSONReport(MachineStatusReport *msr, char *buffer);

  // Stage2 specific stuff; this should be handled
  // as a subclass, not a decorator, but I don't have time for that,
  // and it puts the main code at risk, so adding it in here is
  // reasonable - rlr

  void outputStage2Report(Stage2Heater s2h,MachineStatusReport *msr,
                          float target_temp,
                          float setpoint_temp,
                          float measured_temp,
                          float heater_duty_cycle,
                          float ramp_C_per_min);
  void createStage2JSONReport(Stage2Heater s2h,MachineStatusReport *msr, char *buffer);


  // This is currently not in use; we expect to need it
  // when we are making the system more automatic.
  void runComplexAlgolAssertions();
  void clearErrors();

  // This is the number of periods around a point in time we will
  // average to produce a smooth temperature. (Our thermocouples have
  // only 0.25 C resolution, which is low for a 0.5C/minute control
  // situation!) These are always taken to be BACKWARD in time.
  // This IS NOT USED in the current code.
  const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
  // Ddelta is the change in temperature in C per min
  float Ddelta_C_per_min = 0.0;

};


#endif
