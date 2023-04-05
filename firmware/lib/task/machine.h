#ifndef MACHINE_H
#define MACHINE_H

// Hardware Abstraction Layer
#include "SensirionSFM3X00.h"
#include "DeltaFans.h"

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



enum MachineState {
  // Off is the initial state. It is a zero-power state.
  Off,
  // Attempting to reach operational temperatures.
  Warmup,
  // Operating
  NormalOperation,
  // Attempt to cool down slowly
  Cooldown,
  // A critical fault has occurred or an acknowledgement has not been received
  CriticalFault,
  // Emergency Shutdwon: stop power consumption as quickly as possible
  EmergencyShutdown,
  // Remain Off util a user releases this state
  OffUserAck
};

enum IdleOrOperateSubState {
  // Operate means to produce maximum oxygen. It is the default substates
  Operate,
  // Idle means to produce minimum possible oxygen, but stay warm.
  Idle
};

struct MachineStatusReport {
  float post_heater_C;
  float post_stack_C;
  float heater_voltage;
  float stack_voltage;
  float stack_amps;
  float stack_ohms;
  float fan_speed;
  float flow_ml_per_s;
  boolean air_flow_sufficient;
};

const static int NUM_FANS = 1;

class MachineHAL {
public:
  SensirionFlow *_flowsensor;
  DeltaFans _fans[NUM_FANS];
  bool init();
  //  void _updateFanSpeed(float unitInterval);
};


class MachineConfig {
public:

  void _updateFanSpeed(float unitInterval);

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

  IdleOrOperateSubState idleOrOperate = Operate;
  float MAXIMUM_HEATER_VOLTAGE = 12.0;
  float MAXIMUM_STACK_VOLTAGE = 8.0;
  float MAXIMUM_STACK_AMPS = 1.0;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usuall 0-255) but this should be
  // the last step.
  float fanPWM = 0.5;

  char const* errors[10];
  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};

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
  float TARGET_FLOW_ml_per_S = 1000.0;
#else
  float COOLDOWN_TARGET_C = 26.0;
  float WARMUP_TARGET_C = 600.0;
  float DESIRED_STACK_C = 700.0;
#endif

  MachineStatusReport report;

};

void outputReport(MachineStatusReport msr);



#endif
