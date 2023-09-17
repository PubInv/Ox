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
  // TODO: This class should probably be rewritten with specific state transition functions.
  // This would allow us to log transition events elegantly. As it is, we have to be idempotent...
  // in the "off" state we have to keep turning everything off because we don't know if we were
  // just turned off or have been off for 24 hours.

  // TODO: Most of this should be moved into the machine definition
  bool CogTask::_init()
  {
    OxCore::Debug<const char *>("CogTask init\n");

    getConfig()->fanDutyCycle = 0.0;

    return true;
  }

  void CogTask::printGenericInstructions() {
    Serial.println("Enter s:w to Warmup, s:c to cooldown, s:o to turn off.");
    Serial.println("Enter a:XX.X to set (a)mperage, (w)attage, (f)an speed (h)eater temp, and (r)amp rate.");

  }

  COG_HAL* CogTask::getHAL() {
    return (COG_HAL *) (getConfig()->hal);
  }

  float CogTask::getTemperatureReading() {
    return getConfig()->report->post_heater_C;
  }
  bool CogTask::_run()
  {
    // Report fan speed
    getConfig()->report->fan_rpm =
      getHAL()->_fans[0]._calcRPM(0);
    this->StateMachineManager::run_generic();
  }

  void CogTask::turnOff() {
    float fs = 0.0;
    getConfig()->fanDutyCycle = fs;
    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    _updateStackVoltage(getConfig()->MIN_OPERATING_STACK_VOLTAGE);
    // Although after a minute this should turn off, we want
    // to do it immediately
    heaterPIDTask->shutHeaterDown();
  }

  MachineState CogTask::_updatePowerComponentsOff() {
    MachineState new_ms = Off;
    turnOff();
    return new_ms;
  }


  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Warmup Mode!\n");
    }

    float t = getTemperatureReading();

    // if we've reached operating temperature, we switch
    // states
    // This is now obsolete in the "5 knob" protocol
    // if (t > getConfig()->OPERATING_TEMP) {
    //   new_ms = NormalOperation;
    //   return new_ms;
    // }


    if (getConfig()->previous_ms != Warmup) {

      // Although this is wise, I think it is will add confusion to the
      // user, so I am removing it for now.
      //   getConfig()->GLOBAL_RECENT_TEMP = t;

      getConfig()->BEGIN_UP_TIME_MS = millis();
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

    // This action is taken out in the "5 knob" protocoln
    // if (cross_stack_temp > getConfig()->MAX_CROSS_STACK_TEMP) {
    //   OxCore::Debug<const char *>("PAUSING INCREASED DUE TO CROSS STACK TEMP: ");
    //   OxCore::DebugLn<float>(cross_stack_temp);
    //   // here now we will not change the TARGET_TEMP.
    //   // in order to be prepared when this condition is
    //   // releived, we need to recent the time and temp
    //   // so that we can smoothly been operating.
    //   getConfig()->GLOBAL_RECENT_TEMP = t;
    //   getConfig()->BEGIN_UP_TIME_MS = millis();
    // } else {
      getConfig()->TARGET_TEMP = tt;
      // now we will set the setPoint in the heater_pid_task...
      // this requires a dependence on that task, but is
      // better than creating a deeper global dependence.
      heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
      //    }

    _updateStackVoltage(getConfig()->STACK_VOLTAGE);

    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;

    _updateStackVoltage(getConfig()->IDLE_STACK_VOLTAGE);
    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Cooldown Mode!\n");
    }

    float t = getTemperatureReading();

          // This would be better done in a transition function!
    if (getConfig()->previous_ms != Cooldown) {

      // Although this is wise, I think it is will add confusion to the
      // user, so I am removing it for now.
      //   getConfig()->GLOBAL_RECENT_TEMP = t;


      getConfig()->BEGIN_DN_TIME_MS = millis();
    }

    // I believe this is obsolete in the "5 knobs" protocol
    // if (t <= getConfig()->COOLDOWN_TARGET_C) {
    //   new_ms = Off;
    //   return new_ms;
    // }


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
    float tt = getConfig()->TARGET_TEMP;

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
    heaterPIDTask->HeaterSetPoint_C = tt;

    _updateStackVoltage(getConfig()->STACK_VOLTAGE);
    return new_ms;
  }
}
