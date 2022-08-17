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
#include "mostplus_flow.h"


#define DEBUG_FLOW_SENSOR 0

MostPlusFlow::MostPlusFlow() {
  pinMode(RF_MOSTPLUS_FLOW_PIN, INPUT);
}

// Unfortunately, in the current RibbonFish the
// voltage drops enough when the heater is own to make this
// reading problematic for low flows.
boolean MostPlusFlow::isAirFlowing() {
  int digitizedVoltage = analogRead(RF_MOSTPLUS_FLOW_PIN);
  float trueVoltage = digitizedVoltage * 3.3 / 1023;
  if (DEBUG_FLOW_SENSOR > 0) {
    OxCore::Debug<const char *>("FlowSensor V: ");
    OxCore::DebugLn<float>(trueVoltage);
  }
  return trueVoltage > RF_MOSTPLUS_FLOW_LOW_CUTOFF_VOLTAGE;
}
