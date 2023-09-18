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
    Serial.println("Enter s:1 to Turn On, s:0 to Turn Off.");
    Serial.println("Enter a:XX.X to set (a)mperage, (w)attage, (f)an speed (h)eater set p., and (r)amp rate.");

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
    StateMachineManager::turnOff();
  }

  MachineState CogTask::_updatePowerComponentsOff() {
    turnOff();
    return Off;
  }

  void CogTask::_updateCOGSpecificComponents() {
      float t = getTemperatureReading();
      float fs = computeFanSpeed(t);
      float a = computeAmperage(t);

      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("fan speed, amperage\n");
        OxCore::Debug<float>(fs);
        OxCore::Debug<const char *>(" ");
        OxCore::Debug<float>(a);
      }

      getHAL()->_updateFanPWM(fs);
      getConfig()->report->fan_pwm = fs;
      _updateStackAmperage(a);
      _updateStackVoltage(getConfig()->STACK_VOLTAGE);
  }
  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    new_ms = StateMachineManager::_updatePowerComponentsWarmup();
    if (new_ms == Warmup) {
      _updateCOGSpecificComponents();
    }
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    new_ms = StateMachineManager::_updatePowerComponentsCooldown();
    if (new_ms == Cooldown) {
      _updateCOGSpecificComponents();
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
    StateMachineManager::_updatePowerComponentsOperation(i_or_o);
    _updateCOGSpecificComponents();

    return new_ms;
  }
}
