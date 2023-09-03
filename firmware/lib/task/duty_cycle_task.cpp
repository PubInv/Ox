/*

  DutyCycle.cpp -- control an AC heater with an SSR

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

#include <duty_cycle_task.h>


DutyCycleTask::DutyCycleTask() {
}

bool DutyCycleTask::_init()
{
  OxCore::Debug<const char *>("DutyCycleTask init\n");
    if (DEBUG_DUTY_CYCLE > 1) {
    Serial.println("DUTY CYCLE RUN!");
  }

  return true;
}

void DutyCycleTask::reset_duty_cycle() {
  recorded_duty_cycle = 0;
  recorded_dc_ms = 0;
  time_of_last_check = millis();
}
// The fundamental purpose of this task is just to
// drive the dynamic from DutyCycle of the heater.
bool DutyCycleTask::_run()
{
  if (DEBUG_DUTY_CYCLE > 1) {
    Serial.println("DUTY CYCLE RUN!");
  }
  // WARNING: This will fail when 2^32 ms are reached, about 28 days I think.
  unsigned long ms = millis();
  unsigned long delta_t = ms - time_of_last_check;
  // now we update the recorded duty_cycle weight
  float old_dc = recorded_duty_cycle;
  float old_ms = recorded_dc_ms;
  recorded_dc_ms += delta_t;
  if (DEBUG_DUTY_CYCLE > 1) {
    Serial.println("DUTY CYCLE MID!");
  }
  if (recorded_dc_ms != 0) {
    recorded_duty_cycle = (old_dc * old_ms + ((isOn ? delta_t : 0))) / recorded_dc_ms;
  } else {
    recorded_duty_cycle = 1.0;
  }

  // now we decided to turn on or off!
  isOn = (recorded_duty_cycle < dutyCycle);
  // now we actually turn the heater on or off!
  if (DEBUG_DUTY_CYCLE > 1) {
    OxCore::Debug<const char *>("DUTY Heater On: ");
    OxCore::DebugLn<int>(isOn);
  }

  time_of_last_check = ms;

  one_pin_heater->setHeater(0,isOn);
  if (DEBUG_DUTY_CYCLE > 1) {
    OxCore::DebugLn<const char *>("DUTY HEATERS SET! ");
  }

  return true;
}
