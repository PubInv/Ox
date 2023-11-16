/*
  state_machine_manager.cpp

  Copyright 2023, Robert L. Read

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

#include <state_machine_manager.h>

using namespace std;

namespace OxApp
{

  bool StateMachineManager::run_generic()
  {
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("starting run generic: ");
    }

    MachineState ms = getConfig()->ms;

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("ms : ");
      OxCore::DebugLn<int>(ms);
    }
    printOffWarnings(ms);

    printGenericInstructions();

    MachineState new_state = _executeBasedOnState(ms);
    if (DEBUG_LEVEL > 0) {
      OxCore::DebugLn<const char *>("finished execute");
    }
    // if the state really changes, we want to log that and take some action!
    if (new_state != ms) {
      getConfig()->ms = new_state;
      OxCore::Debug<const char *>("CHANGING STATE TO: ");
      OxCore::DebugLn<const char *>(getConfig()->MachineStateNames[getConfig()->ms]);
      OxCore::DebugLn<const char *>("");
    }
    return true;
  }

  void StateMachineManager::printOffWarnings(MachineState ms) {
    // If we are in the off state there is nothing to do!
    if (ms == OffUserAck) {
      OxCore::DebugLn<const char *>("AN ERROR OCCURED. WILL NOT ENTER OFF STATE ");
      OxCore::DebugLn<const char *>("UNTIL ACKNOWLEDGED. ENTER A SINGLE 'a' TO ACKNOWLEDGE:");
    }
  }

  MachineState StateMachineManager::checkCriticalFaults(MachineState ms) {
    unsigned long now = millis();
    for(int i = 0; i < NUM_CRITICAL_ERROR_DEFINITIONS; i++) {
      if (getConfig()->errors[i].fault_present) {
        if ((((float) now) - ((float) getConfig()->errors[i].begin_condition_ms))
            > (float) getConfig()->errors[i].toleration_ms) {
          if (getConfig()->errors[i].response_state == EmergencyShutdown) {
            return EmergencyShutdown;
          }
        }
      }
    }
    return ms;
  }
  // There is significant COG dependent logic here.
  // At the expense of extra lines of code, I'm
  // going to keep this mostly simple by making it look
  // "table-driven"
  MachineState StateMachineManager::_executeBasedOnState(MachineState ms) {
    MachineState new_ms;

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("\nMachine State: ");
      OxCore::Debug<const char *>(getConfig()->MachineStateNames[ms]);
      OxCore::Debug<const char *>(" : ");
      OxCore::DebugLn<const char *>(getConfig()->MachineSubStateNames[getConfig()->idleOrOperate]);
    }

    new_ms = checkCriticalFaults(ms);

    switch(new_ms) {
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
    getConfig()->previous_ms = ms;
    getConfig()->ms = new_ms;
    getConfig()->report->ms = new_ms;
    return new_ms;
  }


  float StateMachineManager::computeRampUpSetpointTemp(float t,float recent_t,unsigned long begin_up_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_UP = (ms - begin_up_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_UP * getConfig()->RAMP_UP_TARGET_D_MIN;
    tt = min(tt,getConfig()->TARGET_TEMP_C);
    tt = min(tt,getConfig()->BOUND_MAX_TEMP);
    return tt;
  }
  float StateMachineManager::computeRampDnSetpointTemp(float t,float recent_t,unsigned long begin_dn_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_DN = (ms - begin_dn_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_DN * getConfig()->RAMP_DN_TARGET_D_MIN;
    tt = max(tt,getConfig()->TARGET_TEMP_C);
    tt = max(tt,getConfig()->BOUND_MIN_TEMP);
    return tt;
  }

  void StateMachineManager::turnOff() {
    heaterPIDTask->shutHeaterDown();
  }

  // if we change the targetTemp, we will enter either
  // Warmup or Cooldown, with new values.
  void StateMachineManager::transitionToWarmup(float recent) {
    getConfig()->previous_ms = getConfig()->ms;
    getConfig()->ms = Warmup;
    getConfig()->WARM_UP_BEGIN_TEMP = recent;
    getConfig()->SETPOINT_TEMP_C = recent;
    getConfig()->BEGIN_UP_TIME_MS = millis();
  }

  void StateMachineManager::transitionToCooldown(float recent) {
    getConfig()->previous_ms = getConfig()->ms;
    getConfig()->ms = Cooldown;
    getConfig()->COOL_DOWN_BEGIN_TEMP = recent;
    getConfig()->SETPOINT_TEMP_C = recent;
    getConfig()->BEGIN_DN_TIME_MS = millis();
  }

  void StateMachineManager::changeTargetTemp(float t) {
    MachineConfig *mc = getConfig();

    float tt = min(mc->BOUND_MAX_TEMP,t);
    tt = max(mc->BOUND_MIN_TEMP,tt);

    mc->TARGET_TEMP_C = tt;
    mc->report->target_temp_C = tt;
    float current = mc->GLOBAL_RECENT_TEMP;
    if (tt > current) {
      transitionToWarmup(current);
    } else if (tt < current) {
      transitionToCooldown(current);
    } else {
      // no change needed
    }
  }



  MachineState StateMachineManager::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Warmup Mode!\n");
    }

    float t = getTemperatureReadingA_C();
    getConfig()->GLOBAL_RECENT_TEMP = t;

    // if we've reached operating temperature, we switch
    // states
    if (t >= getConfig()->TARGET_TEMP_C) {
      new_ms = NormalOperation;
      return new_ms;
    }


    if (USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {
      // These also are dependent on which heater we are using
      float tt = computeRampUpSetpointTemp(t,
                                           getConfig()->WARM_UP_BEGIN_TEMP,
                                           getConfig()->BEGIN_UP_TIME_MS);
      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("Warmup tt for :");
        Serial.println(getConfig()->s2heater);
        OxCore::DebugLn<float>(tt);
        OxCore::Debug<const char *>("Global Recent temp\n");
        OxCore::DebugLn<float>(getConfig()->GLOBAL_RECENT_TEMP);
        OxCore::DebugLn<float>(getConfig()->BEGIN_UP_TIME_MS);
      }

      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;
    }
    return new_ms;
  }

  MachineState StateMachineManager::_updatePowerComponentsCritialFault() {
	logRecorderTask->SetPeriod(MachineConfig::INIT_LOG_RECORDER_SHORT_PERIOD_MS);
  }
  MachineState StateMachineManager::_updatePowerComponentsEmergencyShutdown() {
	logRecorderTask->SetPeriod(MachineConfig::INIT_LOG_RECORDER_SHORT_PERIOD_MS);
  }
  
  MachineState StateMachineManager::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Cooldown Mode!\n");
    }
    float t = getTemperatureReadingA_C();
    getConfig()->GLOBAL_RECENT_TEMP = t;

    if (t <= getConfig()->TARGET_TEMP_C) {
      new_ms = NormalOperation;
      return new_ms;
    }

    if (USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {

      float tt = computeRampDnSetpointTemp(t,
                                           getConfig()->COOL_DOWN_BEGIN_TEMP,
                                           getConfig()->BEGIN_DN_TIME_MS);
      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;

      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("CoolDown tt for :");
        Serial.println((unsigned long) heaterPIDTask);
        OxCore::DebugLn<float>(tt);
        OxCore::Debug<const char *>("Global Recent temp\n");
        OxCore::DebugLn<float>(getConfig()->GLOBAL_RECENT_TEMP);
        OxCore::DebugLn<float>(getConfig()->BEGIN_UP_TIME_MS);
      }
    }
    return new_ms;
  }


  MachineState StateMachineManager::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
    MachineState new_ms = NormalOperation;
    if (USE_ONE_BUTTON) {
      runOneButtonAlgorithm();
    } else {

      float tt = getConfig()->TARGET_TEMP_C;
      getConfig()->SETPOINT_TEMP_C = tt;
      heaterPIDTask->HeaterSetPoint_C = tt;
    }
    return new_ms;
  }

}
