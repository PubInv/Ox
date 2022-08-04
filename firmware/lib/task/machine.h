#ifndef MACHINE_H
#define MACHINE_H


#define HAND_TEST 1

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
    "Idle",
    "Cooldown",
    "CriticalFault",
    "EmergencyShutdown",
    "OffUserAck"
  };
  MachineState ms;
  IdleOrOperateSubState idleOrOperate = Operate;
  float MAXIMUM_HEATER_VOLTAGE = 12.0;
  char const* errors[10];
  int heater_indices[2] = {0,1};

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
