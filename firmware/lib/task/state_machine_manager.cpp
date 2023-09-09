#include <state_machine_manager.h>

using namespace std;

namespace OxApp
{

  bool StateMachineManager::run_generic()
  {

    if (DEBUG_LEVEL > 0) {
      OxCore::DebugLn<const char *>("starting run generic");
    }
    // To make sure startup has now wild surges,
    // if we have a valid temperature we will make sure the
    // TempRefreshTask has been run...
    float t = getTemperatureReading();

    MachineState ms = getConfig()->ms;
    // This is only run once, to handle a reset without waiting
    // 5 minutes.
    if ((ms == Warmup) || (ms == Cooldown)) {
      if ((abs(getConfig()->TARGET_TEMP - t) > 40.0) ||
          ((tempRefreshTask->time_of_last_refresh == 0) &&
           (t > 0.0))) {
        tempRefreshTask->run();
        heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
      }
    }

    if (DEBUG_LEVEL > 0) {
      OxCore::DebugLn<const char *>("ms");
      OxCore::DebugLn<int>(ms);
    }
    printOffWarnings(ms);

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
    if (ms == Off) {
      OxCore::DebugLn<const char *>("Currrently Off. Enter a single 'w' to warmup: ");
    }
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
    getConfig()->previous_ms = ms;
    getConfig()->ms = new_ms;
    getConfig()->report->ms = new_ms;
    return new_ms;
  }


  float StateMachineManager::computeFanSpeed(float t) {
    float f;
    float p = getConfig()->FULL_POWER_FOR_FAN;
    float s = getConfig()->FAN_SPEED_AT_OPERATING_TEMP;
    float d = getConfig()->TEMP_TO_BEGIN_FAN_SLOW_DOWN;
    float e = getConfig()->END_FAN_SLOW_DOWN;
    float h = getConfig()->OPERATING_TEMP;
    float r = getConfig()->RED_TEMP;
    float y = getConfig()->YELLOW_TEMP;
    if (t < d) {
      f = p;
    } else if (t >= d && t < y) {
      f = p - (p - s) * ((t - d) / (h - d));
    } else  { // t > y
      f = s + ((t - y) / (r - y)) * (1.0 - s);
    }
    return f;
  }
  float StateMachineManager::computeAmperage(float t) {
    return MachineConfig::MAX_AMPERAGE *
      ((t < getConfig()->YELLOW_TEMP)
       ?  1.0
       : MachineConfig::MAX_AMPERAGE * max(0,getConfig()->RED_TEMP - t) /
       (getConfig()->RED_TEMP - getConfig()->YELLOW_TEMP));
  }


  float StateMachineManager::computeRampUpTargetTemp(float t,float recent_t,unsigned long begin_up_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_UP = (ms - begin_up_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_UP * getConfig()->RAMP_UP_TARGET_D_MIN;
    tt = min(tt,getConfig()->OPERATING_TEMP);
    return tt;
  }
  float StateMachineManager::computeRampDnTargetTemp(float t,float recent_t,unsigned long begin_dn_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_DN = (ms - begin_dn_time_ms) / (60 * 1000);

    float tt =
      recent_t - MINUTES_RAMPING_DN * getConfig()->RAMP_DN_TARGET_D_MIN;
    tt = max(tt,getConfig()->STOP_TEMP);
    return t;
  }
}
