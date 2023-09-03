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


#ifndef MACHINE_CORE_DEFS_H
#define MACHINE_CORE_DEFS_H

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

// this is which of the Stage2Heaters we operate on.
// This also will control which thermocouple to use.
const int NUM_STAGE2_HEATERS = 3;
enum Stage2Heater {
  Int1,
  Ext1,
  Ext2
};



struct MachineStatusReport {
  MachineState ms;
  float target_temp_C;
  float post_heater_C;
  float post_stack_C;
  float post_getter_C;
  float heater_voltage;
  float stack_voltage;
  float stack_amps;
  float stack_ohms;
  float fan_rpm;
  float fan_pwm;
  float flow_ml_per_s;
  float heater_duty_cycle;
  bool air_flow_sufficient;
};

#endif
