// Copyright (C) 2021
// Lawrence Kincheloe, Robert Read, Ben Coombs.

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
#endif

#include <MAX31855.h>
#include <core.h>
#include <machine.h>
#include <stage2_hal.h>
#include <debug.h>

#define TEMPERATURE_PRECISION 9

namespace Temperature {

  // TODO: Turn this into a Loop to support any number of thermocouples.
  // Print out an ordering so we can match ID against index
  // arrays to hold device Chip Select

  MAX31855Temperature::MAX31855Temperature() {
    // TODO: Either put these in the _config, or
    // remove the usage of _config entirely

    sensors[Int1] =  new Adafruit_MAX31855(MAXCLK, INT1_MAXCS, MAXDO);
    sensors[Ext1] =  new Adafruit_MAX31855(MAXCLK, EXT1_MAXCS, MAXDO);
    sensors[Ext2] =  new Adafruit_MAX31855(MAXCLK, EXT2_MAXCS, MAXDO);
    for (int i = 0; i < 3; i++) {
      sensors[i]->begin();
    }
  }

  float MAX31855Temperature::ReadTemperature() {
    return GetTemperature(0);
  }
  float MAX31855Temperature::GetTemperature() {
    return GetTemperature(0);
  }
  float MAX31855Temperature::GetTemperature(int idx) {

    // TODO: check error here.
    //    float tempC = this->sensors[idx]->readError();
    float tempC = this->sensors[idx]->readCelsius();

    // if (tempC != DEVICE_DISCONNECTED_C)
    //   {
    //   }
    // else
    //   {
    //     Serial.print(F("Error: Could not read temperature data: "));
    //     Serial.println(idx);
    //   }
    return tempC;

  }
  SensorConfig MAX31855Temperature::GetConfig() const {

    return _config;
  }
}
