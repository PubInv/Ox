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
#include <stage2_hal.h>

using namespace std;


namespace OxApp
{

  // TODO: Most of this should be moved into the machine definition
  bool Stage2HeaterTask::_init()
  {
    OxCore::Debug<const char *>("Stage2HeaterTask init\n");
    return true;
  }
  // There has to be a better way to do this in C++
  bool Stage2HeaterTask::_run()
    {
      this->run_generic();
    }

  void Stage2HeaterTask::printGenericInstructions() {
    // to avoid this being printed too often, we will only run it for the int1 machine.
    if (getConfig()->s2heater == Int1) {
      Serial.print("Current Heater to command:");
      Serial.println(MachineConfig::HeaterNames[getConfig()->hal->s2heaterToControl]);
      Serial.println("Enter 1, 2, or 3 to switch the machine your commands change.");
      Serial.println("Use h:700 to set target temp; use r:0.3 to set ramp rate.");
    }
  }

  // We override this here, because it is too annyoing to have
  // it printed in triplicate.
  void Stage2HeaterTask::printOffWarnings(MachineState ms) {
  }

  float Stage2HeaterTask::getTemperatureReading() {
    Stage2HAL* s2h = (Stage2HAL *)(getConfig()->hal);
    return s2h->
      getTemperatureReading(getConfig()->s2heater,
                            getConfig());
  }

  MachineState Stage2HeaterTask::_updatePowerComponentsOff() {
    return Off;
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsWarmup() {
    return StateMachineManager::_updatePowerComponentsWarmup();
  }
  MachineState Stage2HeaterTask::_updatePowerComponentsCooldown() {
    return StateMachineManager::_updatePowerComponentsCooldown();
  }

  // Warning!! these states are essentially unused in the
  // 5-knob protocol
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
  void Stage2HeaterTask::turnOff() {
    StateMachineManager::turnOff();
  }
}
