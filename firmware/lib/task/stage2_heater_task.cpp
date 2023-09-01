/*
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

#include "stage2_heater_task.h"
#include <cmath>
#include <abstract_temperature.h>
#include <stage2_config.h>

using namespace std;


namespace OxApp
{

  // TODO: Most of this should be moved into the machine definition
    bool Stage2HeaterTask::_init()
    {
        OxCore::Debug<const char *>("Stage2HeaterTask init\n");
        return true;
    }

      bool Stage2HeaterTask::_run()
    {
      // put an ms switch here
      if (DEBUG_LEVEL > 0) {
              OxCore::Debug<const char *>("Stage2HeaterTask run\n");
      }

      // To make sure startup has now wild surges,
      // if we have a valid temperature we will make sure the
      // TempRefreshTask has been run...
      float postHeaterTemp = getConfig()->report->post_heater_C;
      if ((tempRefreshTask->time_of_last_refresh == 0) &&
          (postHeaterTemp > 0.0)) {
        tempRefreshTask->run();
      }
      StateMachineManager::_run();
    }


  MachineState Stage2HeaterTask::_updatePowerComponentsOff() {
    MachineState new_ms = Off;
    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Warmup Mode!\n");
    }

    // This needs to be made dendent on which one we are!
    // probably should use and enum and a switch here.

    float t;
    switch(whichHeater) {
    case Int1:
      t = getConfig()->s2sr->int1_temp_C;
      break;
    case Ext1:
      t = getConfig()->s2sr->ext1_temp_C;
      break;
    case Ext2:
      t = getConfig()->s2sr->ext2_temp_C;
      break;
    }
    // These also are dependent on which heater we are using
    float tt = computeRampUpTargetTemp(t,
                                       getConfig()->RECENT_TEMPERATURE,
                                       getConfig()->BEGIN_UP_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("tt\n");
      OxCore::DebugLn<float>(tt);
    }


    STAGE2_TARGET_TEMP = tt;
    // now we will set the setPoint in the heater_pid_task...
    // this requires a dependence on that task, but is
    // better than creating a deeper global dependence.
    heaterPIDTask->HeaterSetPoint_C = STAGE2_TARGET_TEMP;
    switch(whichHeater) {
    case Int1:
      getConfig()->s2sr->target_int1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext1:
      getConfig()->s2sr->target_ext1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext2:
      getConfig()->s2sr->target_ext2_temp_C = STAGE2_TARGET_TEMP;
      break;
    }

    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Cooldown Mode!\n");
    }
    if (getConfig()->previous_ms != Cooldown) {
      // can this be be made RECENT_TEMPERATURE?
      getConfig()->COOL_DOWN_BEGIN_TEMPERATURE = getConfig()->report->post_heater_C;
    }

    if (getConfig()->report->post_heater_C <= getConfig()->COOLDOWN_TARGET_C
        &&
        getConfig()->report->post_stack_C <= getConfig()->COOLDOWN_TARGET_C) {
      new_ms = Off;
      return new_ms;
    }

    float t;
    switch(whichHeater) {
    case Int1:
      t = getConfig()->s2sr->int1_temp_C;
      break;
    case Ext1:
      t = getConfig()->s2sr->ext1_temp_C;
      break;
    case Ext2:
      t = getConfig()->s2sr->ext2_temp_C;
      break;
    }

    float tt = computeRampDnTargetTemp(t,
                                       getConfig()->COOL_DOWN_BEGIN_TEMPERATURE,
                                       getConfig()->BEGIN_DN_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("tt\n");
      OxCore::DebugLn<float>(tt);
    }

    STAGE2_TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = STAGE2_TARGET_TEMP;
    switch(whichHeater) {
    case Int1:
      getConfig()->s2sr->target_int1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext1:
      getConfig()->s2sr->target_ext1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext2:
      getConfig()->s2sr->target_ext2_temp_C = STAGE2_TARGET_TEMP;
      break;
    }


    return new_ms;
  }

  MachineState Stage2HeaterTask::_updatePowerComponentsIdle() {
    OxCore::Debug<const char *>("IN IDLE FUNCTION ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;
    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsEmergencyShutdown() {
    MachineState new_ms = OffUserAck;
    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }

   MachineState Stage2HeaterTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
     MachineState new_ms = NormalOperation;

    float t = getConfig()->report->post_heater_C;
    float tt = STAGE2_OPERATING_TEMP;

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("tt\n");
      OxCore::DebugLn<float>(tt);
    }

    STAGE2_TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = STAGE2_TARGET_TEMP;

    switch(whichHeater) {
    case Int1:
      getConfig()->s2sr->target_int1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext1:
      getConfig()->s2sr->target_ext1_temp_C = STAGE2_TARGET_TEMP;
      break;
    case Ext2:
      getConfig()->s2sr->target_ext2_temp_C = STAGE2_TARGET_TEMP;
      break;
    }

    return new_ms;
   }
}
