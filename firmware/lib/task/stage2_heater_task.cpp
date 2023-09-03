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

  // TODO --- I would like to make this a static function so that it can be moved into
  // the parent task.
  void Stage2HeaterTask::tempRefresh(float t,float RECENT_TEMP,MachineState ms) {
    if (ms == Warmup) {
      time_of_last_refresh = millis();

      if (abs(t - RECENT_TEMP) > getConfig()->TEMP_REFRESH_LIMIT) {
        getConfig()->STAGE2_BEGIN_UP_TIME[whichHeater] = time_of_last_refresh;
        RECENT_TEMP = t;
        STAGE2_TARGET_TEMP = t;
      }
    } else if (ms == Cooldown) {
      time_of_last_refresh = millis();
      if (abs(t - RECENT_TEMP) > getConfig()->TEMP_REFRESH_LIMIT) {
        getConfig()->STAGE2_BEGIN_DN_TIME[whichHeater] = time_of_last_refresh;
        RECENT_TEMP = t;
        STAGE2_TARGET_TEMP = t;
      }
    }
  }


  float Stage2HeaterTask::getTemperatureReading() {
    float t;
    switch (getConfig()->s2heaterToControl) {
    case 0:
      t = getConfig()->report->post_heater_C;
      break;
    case 1:
      t = getConfig()->report->post_getter_C;
      break;
    case 2:
      t = getConfig()->report->post_stack_C;
      break;
    default:
      Serial.println("s2heater_task HeaterControl not set!");
      break;
    }
    return t;
  }

  bool Stage2HeaterTask::_run()
  {
    // put an ms switch here
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Stage2HeaterTask run\n");
    }

    MachineState ms = getConfig()->s2sr->ms[whichHeater];
    printOffWarnings(ms);

    MachineState new_state = _executeBasedOnState(ms);
    // if the state really changes, we want to log that and take some action!
    if (new_state != getConfig()->ms) {
      getConfig()->ms = new_state;
      OxCore::Debug<const char *>("CHANGING STATE TO: ");
      OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[getConfig()->ms]);
      OxCore::DebugLn<const char *>("");
    }

    return true;

  }

  // TODO: This code is too similar to the
  // same function in cog_task.cpp, it should be
  // lamba-lifted.
  MachineState Stage2HeaterTask::_executeBasedOnState(MachineState ms) {
    MachineState new_ms;

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("\nMachine State: ");
      OxCore::Debug<const char *>(getConfig()->MachineStateNames[ms]);
      OxCore::Debug<const char *>(" : ");
      OxCore::DebugLn<const char *>(getConfig()->MachineSubStateNames[getConfig()->idleOrOperate]);
    }


    switch(ms) {
    case Off:
      new_ms = _updatePowerComponentsOff();
      break;
    case Warmup:
      new_ms = _updatePowerComponentsWarmup();
      break;
    case NormalOperation:
      new_ms = _updatePowerComponentsOperation(getConfig()->idleOrOperate);
      break;
    case Cooldown:
      new_ms = _updatePowerComponentsCooldown();
      break;
    case CriticalFault:
      new_ms = _updatePowerComponentsCritialFault();
      break;
    case EmergencyShutdown:
      new_ms = _updatePowerComponentsEmergencyShutdown();
      break;
    case OffUserAck:
      new_ms = _updatePowerComponentsOffUserAck();
      break;
    default:
      OxCore::Debug<const char *>("INTERNAL ERROR: UNKOWN MACHINE STATE\n");
      // This is not really enough information; we need a way to
      // record what the fault is, but it will do for now.
      new_ms = CriticalFault;
    }
    getConfig()->s2sr->ms[whichHeater] = new_ms;
    return new_ms;
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
    float t = getTemperatureReading();

    // if we've reached operating temperature, we switch
    // states
    if (t >= getConfig()->STAGE2_OPERATING_TEMP[whichHeater]) {
      return NormalOperation;
    }

    // This is an important "sanity check"
    // for surviving restarts...
    tempRefresh(t,getConfig()->GLOBAL_RECENT_TEMP,Warmup);

    // These also are dependent on which heater we are using
    float tt = computeRampUpTargetTemp(t,
                                       getConfig()->GLOBAL_RECENT_TEMP,
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

    getConfig()->s2sr->target_temp_C[whichHeater];

    return new_ms;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Cooldown Mode!\n");
    }
    float t = getTemperatureReading();

    if (getConfig()->previous_ms != Cooldown) {
      getConfig()->COOL_DOWN_BEGIN_TEMP = t;
    }

    if (t <= getConfig()->COOLDOWN_TARGET_C) {
      new_ms = Off;
      return new_ms;
    }

    tempRefresh(t,getConfig()->GLOBAL_RECENT_TEMP,Cooldown);

    float tt = computeRampDnTargetTemp(t,
                                       getConfig()->COOL_DOWN_BEGIN_TEMP,
                                       getConfig()->BEGIN_DN_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("tt\n");
      OxCore::DebugLn<float>(tt);
    }

    STAGE2_TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = STAGE2_TARGET_TEMP;

    getConfig()->s2sr->target_temp_C[whichHeater];

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

    float tt = getConfig()->STAGE2_OPERATING_TEMP[whichHeater];
    STAGE2_TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = STAGE2_TARGET_TEMP;
    getConfig()->s2sr->target_temp_C[whichHeater] = STAGE2_TARGET_TEMP;

    return new_ms;
  }
}
