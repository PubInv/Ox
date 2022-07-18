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

#ifndef ABSTRACT_TEMPERATURE
#define ABSTRACT_TEMPERATURE

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <model.h>

namespace Temperature {
  // TODO: these enums are universal, should be in a separate file
    enum class SensorMode {
        I2C,
        SPI,
        Analog
    };

    enum class TemperatureUnits {
        K,
        C,
        F
    };

    struct SensorConfig {
        uint32_t id;
        SensorMode mode;
        TemperatureUnits units;
        uint8_t pin;
        uint32_t timeout;
        uint32_t temperatureMin;
        uint32_t temperatureMax;
    };

  // Note: Our Temperature Model allows indexing;
  // That is, there may be several temperature sensors
  // implemented by an AbstractTemperature, discovered by
  // an integer index.
    class AbstractTemperature {
        public:
            SensorConfig _config;
      // This actually needs to be an array!
            float _temperature;
        public:
      AbstractTemperature();
      AbstractTemperature(SensorConfig &config);

      // Note: The "ReadTemperature" function may do
      // whatever is necessary to object (possibly hardware)
      // to obtain values. It therefore may mutate the object.
      // This presumably gets ALL temperatures.
      virtual float ReadTemperature();

      // "Get" is intended to be a const which which does not
      // modify the object. Ben made this a const, but that
      // makes it imposssible for us to call functions on
      // attached objects, so I'm giving that up... - rlr
      virtual float GetTemperature();
      virtual float GetTemperature(int idx);
      SensorConfig GetConfig() const;

    virtual ~AbstractTemperature() {};
   };

}

#endif
