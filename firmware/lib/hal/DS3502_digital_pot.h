// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifndef DS3502_DIGITAL_POT
#define DS3502_DIGITAL_POT

// Put guard for Arduino here

#ifdef ARDUINO

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include "abstract_temperature.h"

#include <OneWire.h>
#include <DallasTemperature.h>


// Note: In a real solution, we need to know the addresses for the temperature sensors
// so that we can be definite about which one is where. See:
// https://lastminuteengineers.com/multiple-ds18b20-arduino-tutorial/
// For now, we will rely on "indexing" and if it is wrong just switch the sensors.
// This is a quick-and-dirty approach for the "RibbonFish" POC.
#include <Adafruit_DS3502.h>

// TODO: This could potentially become part of
// an abstract "power supply"
  class DS3502DigitalPot  {
  public:
    DS3502DigitalPot();
    bool foundPot = false;
    Adafruit_DS3502 ds3502;
    void setWiper(float fraction);
  };

#endif

#endif
