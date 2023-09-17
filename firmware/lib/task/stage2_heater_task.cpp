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

  void Stage2HeaterTask::printGenericInstructions() {
    // to avoid this being printed too often, we will only run it for the int1 machine.
    if (getConfig()->s2heater == Int1) {
      Serial.print("Current Heater to command:");
      Serial.println(MachineConfig::HeaterNames[getConfig()->hal->s2heaterToControl]);
      Serial.println("Enter 1, 2, or 3 to switch the machine your commands change.");
      Serial.println("Use h:700 to set target temp; use r:0.3 to set ramp rate.");
    }
  }

  float Stage2HeaterTask::getTemperatureReading() {
    Stage2HAL* s2h = (Stage2HAL *)(getConfig()->hal);
    return s2h->
      getTemperatureReading(getConfig()->s2heater,
                            getConfig());
  }

  // We override this here, because it is too annyoing to have
  // it printed in triplicate.
  void Stage2HeaterTask::printOffWarnings(MachineState ms) {
  }
  // There has to be a better way to do this in C++
  bool Stage2HeaterTask::_run()
    {
      this->run_generic();
    }

    void Stage2HeaterTask::turnOff() {
      heaterPIDTask->shutHeaterDown();
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
    // We believe this is now obsolete in the "5 knob"
    // protocol...
    //    if (t >= getConfig()->OPERATING_TEMP) {
    //      return NormalOperation;
    //    }

    // These also are dependent on which heater we are using
    float tt = computeRampUpTargetTemp(t,
                                       getConfig()->GLOBAL_RECENT_TEMP,
                                       getConfig()->BEGIN_UP_TIME_MS);

    if (!isnormal(tt)) {
      OxCore::Debug<const char *>("XXXXXXXXXXX non-normal target value tt\n");
      OxCore::DebugLn<float>(tt);
      OxCore::DebugLn<float>(getConfig()->BEGIN_UP_TIME_MS);
      OxCore::DebugLn<int>(getConfig()->s2heater);

    }

     getConfig()->GLOBAL_RECENT_TEMP = t;
     //     getConfig()->BEGIN_UP_TIME_MS = millis();

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("Warmup tt for :");
      Serial.println(getConfig()->s2heater);
      OxCore::DebugLn<float>(tt);
      OxCore::Debug<const char *>("Global Recent temp\n");
      OxCore::DebugLn<float>(getConfig()->GLOBAL_RECENT_TEMP);
      OxCore::DebugLn<float>(getConfig()->BEGIN_UP_TIME_MS);

    }

    getConfig()->TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;

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

    float tt = computeRampDnTargetTemp(t,
                                       getConfig()->COOL_DOWN_BEGIN_TEMP,
                                       getConfig()->BEGIN_DN_TIME_MS);

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("CoolDown tt for :");
      Serial.println((unsigned long) heaterPIDTask);
      OxCore::DebugLn<float>(tt);
      OxCore::Debug<const char *>("Global Recent temp\n");
      OxCore::DebugLn<float>(getConfig()->GLOBAL_RECENT_TEMP);
      OxCore::DebugLn<float>(getConfig()->BEGIN_UP_TIME_MS);
    }

    getConfig()->TARGET_TEMP = tt;
    heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;

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

    getConfig()->TARGET_TEMP;
    heaterPIDTask->HeaterSetPoint_C = getConfig()->TARGET_TEMP;
    return new_ms;
  }
}
