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

#include "cog_task.h"
#include <cmath>
#include <abstract_temperature.h>
#include <TF800A12K.h>

using namespace std;


namespace OxApp
{


  // TODO: Most of this should be moved into the machine definition
    bool CogTask::_init()
    {
        OxCore::Debug<const char *>("CogTask init\n");

        getConfig()->fanDutyCycle = 0.0;

        return true;
    }

  COG_HAL* CogTask::getHAL() {
    (COG_HAL *) getConfig()->hal;
  }


  float CogTask::getTemperatureReading() {
    return getConfig()->report->post_heater_C;
  }
    bool CogTask::_run()
    {
      // Report fan speed
      getConfig()->report->fan_rpm =
        getHAL()->_fans[0]._calcRPM(0);
      // To make sure startup has now wild surges,
      // if we have a valid temperature we will make sure the
      // TempRefreshTask has been run...
      float t = getTemperatureReading();

      // This is only run once, to handle a reset without waiting
      // 5 minutes.
      if ((abs(getConfig()->TARGET_TEMP - t) > 40.0) ||
          ((tempRefreshTask->time_of_last_refresh == 0) &&
           (t > 0.0))) {
        tempRefreshTask->run();
        heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
      }
      MachineState ms = getConfig()->ms;
      printOffWarnings(ms);

      MachineState new_state = _executeBasedOnState(ms);
      // if the state really changes, we want to log that and take some action!
      if (new_state != ms) {
        getConfig()->ms = new_state;
        OxCore::Debug<const char *>("CHANGING STATE TO: ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[getConfig()->ms]);
        OxCore::DebugLn<const char *>("");
      }
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

    bool StateMachineManager::_run()
    {
      return true;
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

  MachineState CogTask::_updatePowerComponentsOff() {
    MachineState new_ms = Off;

    float fs = 0.0;
    getConfig()->fanDutyCycle = fs;
    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);

    return new_ms;
  }

  float StateMachineManager::computeFanSpeed(float t) {
    float f;
    float p = MachineConfig::FULL_POWER_FOR_FAN;
    float s = MachineConfig::FAN_SPEED_AT_OPERATING_TEMP;
    float d = MachineConfig::TEMP_TO_BEGIN_FAN_SLOW_DOWN;
    float e = MachineConfig::END_FAN_SLOW_DOWN;
    float h = MachineConfig::OPERATING_TEMP;
    float r = MachineConfig::RED_TEMP;
    float y = MachineConfig::YELLOW_TEMP;
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
      ((t < MachineConfig::YELLOW_TEMP)
       ?  1.0
       : MachineConfig::MAX_AMPERAGE * max(0,MachineConfig::RED_TEMP - t) /
       (MachineConfig::RED_TEMP - MachineConfig::YELLOW_TEMP));
  }


  float StateMachineManager::computeRampUpTargetTemp(float t,float recent_t,unsigned long begin_up_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_UP = (ms - begin_up_time_ms) / (60 * 1000);
    float tt = recent_t + MINUTES_RAMPING_UP * MachineConfig::RAMP_UP_TARGET_D_MIN;
    tt = min(tt,MachineConfig::OPERATING_TEMP);
    return tt;
  }
  float StateMachineManager::computeRampDnTargetTemp(float t,float recent_t,unsigned long begin_dn_time_ms) {
    unsigned long ms = millis();
    const unsigned long MINUTES_RAMPING_DN = (ms - begin_dn_time_ms) / (60 * 1000);

    float tt =
     recent_t + MINUTES_RAMPING_DN * MachineConfig::RAMP_DN_TARGET_D_MIN;
    tt = max(tt,MachineConfig::STOP_TEMP);
    return t;
  }

  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Warmup Mode!\n");
    }

    float t = getTemperatureReading();

    // if we've reached operating temperature, we switch
    // states
    if (t >= getConfig()->OPERATING_TEMP) {
      return NormalOperation;
    }

    float fs = computeFanSpeed(t);
    float a = computeAmperage(t);
    float tt = computeRampUpTargetTemp(t,
                                       getConfig()->GLOBAL_RECENT_TEMP,
                                       getConfig()->BEGIN_UP_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("fan speed, amperage, tt\n");
      OxCore::Debug<float>(fs);
      OxCore::Debug<const char *>(" ");
      OxCore::Debug<float>(a);
      OxCore::Debug<const char *>(" ");
      OxCore::DebugLn<float>(tt);
    }

    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    _updateStackAmperage(a);

    // This will be used by the HeaterPID task.
    float cross_stack_temp =  abs(getConfig()->report->post_getter_C -  getConfig()->report->post_stack_C);

    if (cross_stack_temp > MachineConfig::MAX_CROSS_STACK_TEMP) {
      OxCore::Debug<const char *>("PAUSING INCREASED DUE TO CROSS STACK TEMP: ");
      OxCore::DebugLn<float>(cross_stack_temp);
      // here now we will not change the TARGET_TEMP.
      // in order to be prepared when this condition is
      // releived, we need to recent the time and temp
      // so that we can smoothly been operating.
      getConfig()->GLOBAL_RECENT_TEMP = t;
      getConfig()->BEGIN_UP_TIME_MS = millis();
    } else {
      getConfig()->TARGET_TEMP = tt;
      // now we will set the setPoint in the heater_pid_task...
      // this requires a dependence on that task, but is
      // better than creating a deeper global dependence.
      heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
    }

    _updateStackVoltage(getConfig()->STACK_VOLTAGE);

    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Cooldown Mode!\n");
    }

    float t = getTemperatureReading();
    if (getConfig()->previous_ms != Cooldown) {
      getConfig()->GLOBAL_RECENT_TEMP = t;
    }

    if (t <= getConfig()->COOLDOWN_TARGET_C) {
      new_ms = Off;
      return new_ms;
    }


    float fs = computeFanSpeed(t);
    float a = computeAmperage(t);
    float tt = computeRampDnTargetTemp(t,
                                       getConfig()->COOL_DOWN_BEGIN_TEMP,
                                       getConfig()->BEGIN_DN_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("fan speed, amperage, tt\n");
      OxCore::Debug<float>(fs);
      OxCore::Debug<const char *>(" ");
      OxCore::Debug<float>(a);
      OxCore::Debug<const char *>(" ");
      OxCore::DebugLn<float>(tt);
    }

    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    _updateStackAmperage(a);

    // Note that Tom Taylor said not to do this check
    // when cooling down, because it might let the system
    // get stuck cooling. I'm leaving it commented here
    // to be symmetric with warmp - rlr
    // float cross_stack_temp =  abs(getConfig()->report->post_getter_C -  getConfig()->report->post_stack_C);

    // if (cross_stack_temp > MachineConfig::MAX_CROSS_STACK_TEMP) {

    //   if (DEBUG_LEVEL > 0) {
    //   OxCore::Debug<const char *>("PAUSING DUE TO CROSS STACK TEMP\n");
    //   }
    //   // here now we will not change the TARGET_TEMP.
    //   // in order to be prepared when this condition is
    //   // releived, we need to recent the time and temp
    //   // so that we can smoothly been operating.
    //   getConfig()->COOL_DOWN_BEGIN_TEMP = t;
    //   getConfig()->BEGIN_DN_TIME_MS = millis();
    // } else

      {
      getConfig()->TARGET_TEMP = tt;
      heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
    }

    _updateStackVoltage(getConfig()->STACK_VOLTAGE);

    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsIdle() {
    OxCore::Debug<const char *>("IN IDLE FUNCTION ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;
    _updateStackVoltage(MachineConfig::IDLE_STACK_VOLTAGE);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
    MachineState new_ms = OffUserAck;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    // In an emergency shutdown, we do NOT run the fan!
    getHAL()->_updateFanPWM(0.0);
    getConfig()->report->fan_pwm = 0.0;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    return new_ms;
  }
  // TODO: This would go better on the HAL
   void CogTask::_updateStackVoltage(float voltage) {
     for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
       getHAL()->_stacks[i]->updateVoltage(voltage,getConfig());
     }
   }

   void CogTask::_updateStackAmperage(float amperage) {
        for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
          getHAL()->_stacks[i]->updateAmperage(amperage,getConfig());
        }
    }

   MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
     MachineState new_ms = NormalOperation;

    float t = getTemperatureReading();;
    float fs = computeFanSpeed(t);
    float a = computeAmperage(t);
    float tt = MachineConfig::OPERATING_TEMP;

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("fan speed, amperage, tt\n");
      OxCore::Debug<float>(fs);
      OxCore::Debug<const char *>(" ");
      OxCore::Debug<float>(a);
      OxCore::Debug<const char *>(" ");
      OxCore::DebugLn<float>(tt);
    }

    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;

    _updateStackAmperage(a);
    getConfig()->TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;

    _updateStackVoltage(getConfig()->STACK_VOLTAGE);
     return new_ms;
   }
}
