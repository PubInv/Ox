/*
  stage2_hal.cpp -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

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

#include <stage2_hal.h>

bool Stage2HAL::init() {
  pinMode(MAX31850_DATA_PIN, INPUT);

  // TODO: This block of code appears in cog_hal.cpp
  // as well. There should be a way to move this into the
  // superclass, but I can't figure out how to do it.
  if (DEBUG_HAL > 0) {
      Serial.println("About to initialize heaters");
      Serial.println(NUM_HEATERS);
      delay(100);
  }
  _ac_heaters = new OnePinHeater*[NUM_HEATERS];
  for(int i = 0; i < NUM_HEATERS; i++) {
      Serial.println("000");
      delay(100);
    _ac_heaters[i] = new OnePinHeater();
      Serial.println("AAA");
      delay(100);
    _ac_heaters[i]->CHANNEL_0_PIN = HEATER_PINS[i];
      Serial.println("BBB");
      delay(100);
    _ac_heaters[i]->init();
      Serial.println("CCC");
      delay(100);
  }

  if (DEBUG_HAL > 0) {
      Serial.println("HAL:About to return!");
      delay(100);
  }
  return true;
}
