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

#ifndef MACHINE_SCRIPT_H
#define MACHINE_SCRIPT_H

#include <machine_core_defs.h>


/*
  The MachineScript class depends strongly on the MachineConfig.
  The purpose of a MachineScript is to manage an "experiment"
  from warmup up to operating to cooldown to off. This may
  someday also script full, long duration operational modes.
  Our basic principles are:
  1) There is one (or zero) MachineScripts operating, and this
  changes atomically (via a pointer.)
  2) The MachineScripts do not change the basic FSA defining
  the legal states (though it may internally keep sub-states,
  for exmaple, to warm up with different regimes.)
  3) The MachineScript can induce state transitions, in
  particular at particular points in time. It cannot, however,
  interrupt safety-critical shutdown states.
  4) Although our initial intention is taht the MachineScript
  will be ready from a fixed serverd via UDP, the
  MachineScript class is independent of this.

 */


// At first, I am just copying what Geoff did,
// which should be adequate for a lot of testing.
// Later we may change the structure of this data
// to better reflect all of the things that we want to
// control.

// This type has internal types as defined;
// the standard in the configuration file read by
// UDP may be different and in fact we want to study this much.
class MachinePhase {
  // The Offset is the time at which this phase should be enterred, measured in
  // seconds since the beginning of the script.  There may be a lot of room
  // for making this more flexible in the future.
  long offSet;
  // Note that the two fan parameters are likely incompatible.
  // The system is free to use the non-zero one. If they are
  // both zero, then the fan speed is definitely zero.
  float fanSpeed; // [0.0..1.0] -- this is fan speed as fraction of full power
  float fanFlow; // this is standard liters per minute (spm)
  float heaterTemp; // This is the desired-post heater temperature (this is probably the pres-stack temperature)
  /* This is a limit on the number of amps for the heater.
     This may be inconvenient to compute, but in can be estimated by simply measuring the ohmage of the heater and dividing
     110V by that. In likelihood this amperage will be
     implemented as an on-off duty cycle create with an SSR.
   */
  float heaterCurrent;

  /* This the maximum change in post-heater temperature in
     degrees C per minute. The system should decrease
     the heater duty cycle until this is acheived.
  */
  float heaterRamp;

  /*
    The stack may be the most important feature.
    The temperature is the "post-stack" temperature maximum.
    The current is the maximum current we want.
    The Ramp is the change in temperature in degrees C
    per minute.
   */
  float stackTemp;
  float stackCurrent;
  float stackRamp;
};
struct phase_t {
  int duration;
  int fan_speed;
  int fan_flow;
  int preheat_temp;
  int preheat_current;
  int preheat_ramp;
  int stack_temp;
  int stack_current;
  int stack_ramp;
};

//

class MachineScript {
 public:
  static const int MAX_NUM_PHASES = 100;
  int DEBUG_MS = 0;
  char *name;
  long timeStamp;
  bool dryRun;
  int numPhases = 0;

  // This is a magic number here which should really be shared, but it would
  // require extracting the machine.h enumeration to a new file
  MachinePhase *phasesInState[8];
  bool AppendPhase(MachinePhase p);
  void DeleteAllPhases();
  MachineScript *parse_buffer_into_new_script(char *buffer,int debug);
  void DeleteAllPhases(struct phase_t *p);
};


int parse_param(char *buffer, const char *value, char **rvalue);
int parse_param(char *buffer, const char *value, int *rvalue);
int parse_param_state(char *buffer, const char *value, char **rvalue);
int parse_state(char *state, struct phase_t *phase_list[]);

void parse_phases_from_state_script(MachineState m_state,char *srcipt,MachineScript *ms);

#endif
