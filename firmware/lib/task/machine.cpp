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

#include <machine.h>
// there is not yet anything for this to do
#include <core.h>
#include <Wire.h>


void outputReport(MachineStatusReport msr) {
          OxCore::DebugLn<const char *>("");
        OxCore::Debug<const char *>("Post Heater C: ");
        OxCore::DebugLn<float>(msr.post_heater_C);
        OxCore::Debug<const char *>("Heater      V: ");
        OxCore::DebugLn<float>(msr.heater_voltage);
        OxCore::Debug<const char *>("Post Stack  C: ");
        OxCore::DebugLn<float>(msr.post_stack_C);
        OxCore::Debug<const char *>("Stack volts V: ");
        OxCore::DebugLn<float>(msr.stack_voltage);
        OxCore::Debug<const char *>("Stack amps  A: ");
        OxCore::DebugLn<float>(msr.stack_amps);
        OxCore::Debug<const char *>("Stack ohms  O: ");
        if (msr.stack_ohms < 0.0) {
          OxCore::DebugLn<const char*>(" N/A");
        } else {
          OxCore::DebugLn<float>(msr.stack_ohms);
        }
        OxCore::Debug<const char *>("Flow (ml / s): ");
        OxCore::DebugLn<float>(msr.flow_ml_per_s);
        OxCore::Debug<const char *>("Fan Speed (non-lin) [0.0 .. 1.0]: ");
        OxCore::DebugLn<float>(msr.fan_speed);
}

bool MachineHAL::init() {
  // we should probably check that we can read this effectively here
  // and return false if not

  Wire.begin();

  _flowsensor = new SensirionFlow();

  if (_flowsensor->flowSensor->serialNumber == 0xFFFFFFFF) {
    Serial.println("FLOW SENSOR NOT AVIALABLE!");
    Serial.println("THIS IS A CRITICAL ERROR!");
    return false;
  }

  _fans[0] = DeltaFans("FIRST_FAN",0,RF_FAN,1.0);



  _fans[0]._init();
#ifdef TEST_FANS_ONLY
  _fans[0].DEBUG_FAN = 1;
#else
  _fans[0].DEBUG_FAN = 0;
#endif

  return true;
}

// updateTheFanSpeed to a percentage of the maximum flow.
// We may have the ability to specify flow absolutely in the future,
// but this is genertic.
void MachineConfig::_updateFanSpeed(float unitInterval) {
  for (int i = 0; i < NUM_FANS; i++) {
    hal->_fans[i].update(unitInterval);
  }
}
