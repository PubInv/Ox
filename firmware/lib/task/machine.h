#ifndef MACHINE_H
#define MACHINE_H



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

struct MachineConfig {
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
  IdleOrOperateSubState idleOrOperate = Operate;
  float MAXIMUM_HEATER_VOLTAGE = 12.0;
  float MAXIMUM_STACK_VOLTAGE = 12.0;
  char const* errors[10];
  // Until we have a good machine model here,
  // we need to separately identify pre- and post-
  // element temperature sensor indices
  int post_heater_indices[1] = {0};
  int post_stack_indices[1] = {1};

  // These values are useful for testing by hand
#ifdef HAND_TEST
  float COOLDOWN_TARGET_C = 26.0;
  float WARMUP_TARGET_C = 28.0;
  float DESIRED_STACK_C = 30.0;
#else
  float COOLDOWN_TARGET_C = 26.0;
  float WARMUP_TARGET_C = 600.0;
  float DESIRED_STACK_C = 700.0;
#endif

};

#endif
