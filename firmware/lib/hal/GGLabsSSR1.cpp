// Copyright (C) 2021
// Robert Read.

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
#include "GGLabsSSR1.h"


#define DEBUG_FLOW_SENSOR 0

GGLabsSSR1::GGLabsSSR1() {
  init();
}

void GGLabsSSR1::init() {
  const int ACTUAL_CHANNELS = 2;
  // we initialize the Actual channels even if we are only using
  // one because we don't want to have a stray 110VAC line energized
  for(int i = 0; i < ACTUAL_CHANNELS; i++) {
    pinMode(channel_pins[i], OUTPUT);
    digitalWrite(channel_pins[i],LOW);
  }
}
bool GGLabsSSR1::setHeater(int channelNum, bool onIfTrue) {
  if (channelNum < NUM_CHANNELS)
    digitalWrite(channel_pins[channelNum],onIfTrue ? HIGH : LOW);
}
