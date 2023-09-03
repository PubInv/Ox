/*
  stage2_ -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

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

  init_heaters();

  if (DEBUG_HAL > 0) {
      Serial.println("HAL:About to return!");
  }
  return true;
}
