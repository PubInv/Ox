/*
  cog_hal.cpp -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

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

#include <cog_hal.h>
#include <TF800A12K.h>

bool COG_HAL::init() {

  if (DEBUG_HAL > 0) {
      Serial.println("HAL: About to init Fan!");
  }

  pinMode(MAX31850_DATA_PIN, INPUT);
  pinMode(RF_FAN, OUTPUT);
  pinMode(RF_STACK, OUTPUT);


  _fans[0] = SanyoAceB97("FIRST_FAN",0,RF_FAN,1.0);
  _fans[0]._init();

  // TODO: This block of code appears in cog_hal.cpp
  // as well. There should be a way to move this into the
  // superclass, but I can't figure out how to do it.
  if (DEBUG_HAL > 0) {
      Serial.println("About to initialize heaters");
      delay(100);
  }
  _ac_heaters = new OnePinHeater*[NUM_HEATERS];
  for(int i = 0; i < NUM_HEATERS; i++) {
    _ac_heaters[i] = new OnePinHeater();
    _ac_heaters[i]->CHANNEL_0_PIN = HEATER_PINS[i];
    _ac_heaters[i]->init();
  }

  _stacks[0] = new SL_PS("FIRST_STACK",0);
  _stacks[0]->init();

  if (DEBUG_HAL > 0) {
      Serial.println("HAL:About to return!");
  }
  return true;
}

// updateTheFanSpeed to a percentage of the maximum flow.
// We may have the ability to specify flow absolutely in the future,
// but this is genertic.
void COG_HAL::_updateFanPWM(float unitInterval) {
  for (int i = 0; i < NUM_FANS; i++) {
     _fans[i].update(unitInterval);
  }
}
