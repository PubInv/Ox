#ifndef MACHINE_H
#define MACHINE_H


enum MachineState {
  // Off is the initial state. It is a zero-power state.
  Off,
  // Attempting to reach operational temperatures.
  Warmup,
  // Operating
  Operation,
  // Stay warm but produce minimal oxygen
  Idle,
  // Attempt to cool down slowly
  Cooldown,
  // A critical fault has occurred or an acknowledgement has not been received
  CriticalFault,
  // Emergency Shutdwon: stop power consumption as quickly as possible
  EmergencyShutdown,
  // Remain Off util a user releases this state
  OffUserAck
};

struct COGConfig {
  constexpr inline static char const *MachineStateNames[8] = {
    "Off",
    "Warmup",
    "Operation",
    "Idle",
    "Cooldown",
    "CriticalFault",
    "EmergencyShutdown",
    "OffUserAck"
  };
  MachineState ms;
  float MAXIMUM_HEATER_VOLTAGE = 12.0;
  float COOLDOWN_TARGET_C = 26.0;
  char const* errors[10];
};

#endif
