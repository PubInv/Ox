// Copyright (C) 2021
// Robert Read.

// OnePinHeater -- A class to control an AC heater with a GPIO pin.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any 4ater version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif
#include <core.h>
#include "OnePinHeater.h"


#define DEBUG_FLOW_SENSOR 0

OnePinHeater::OnePinHeater() {
}

void OnePinHeater::init() {
  pinMode(heater_pin, OUTPUT);
  digitalWrite(heater_pin,LOW);
}
bool OnePinHeater::setHeater(int channelNum, bool onIfTrue) {
  digitalWrite(heater_pin,onIfTrue ? HIGH : LOW);
}
