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

#ifndef MAX31850_TEMPERATURE
#define MAX31850_TEMPERATURE

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

#include <machine.h>
// Note: In a real solution, we need to know the addresses for the temperature sensors
// so that we can be definite about which one is where. See:
// https://lastminuteengineers.com/multiple-ds18b20-arduino-tutorial/
// For now, we will rely on "indexing" and if it is wrong just switch the sensors.
// This is a quick-and-dirty approach for the "RibbonFish" POC.
// #define POST_STACK_0_IDX 0
//#define POST_HEATER_0_IDX 1
// #define MAX31850_DATA_PIN 2

namespace Temperature {
  class MAX31850Temperature : public AbstractTemperature {
  public:
    SensorConfig _config;
    float _temperature;
    OneWire oneWire;
    DallasTemperature sensors;
    // Pass our oneWire reference to Dallas Temperature.
  public:
    MAX31850Temperature();
    MAX31850Temperature(SensorConfig &config);

// function to print a device address
    void printAddress(DeviceAddress deviceAddress);
    void Config(SensorConfig &config);
    float ReadTemperature();
    float GetTemperature();
    float GetTemperature(int idx);
    SensorConfig GetConfig() const;

    //    ~MAX31850Temperature() {};
  };

}

#endif

#endif
