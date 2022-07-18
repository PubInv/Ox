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

#ifndef DS18B20_TEMPERATURE
#define DS18B20_TEMPERATURE

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include "abstract_temperature.h"

namespace Temperature {
  class DS18B20Temperature : public AbstractTemperature {
  private:
    SensorConfig _config;
    float _temperature;
  public:
    DS18B20Temperature();
    DS18B20Temperature(SensorConfig &config);
    void Config(SensorConfig &config);
    float ReadTemperature();
    float GetTemperature() const;
    SensorConfig GetConfig() const;

    //    ~DS18B20Temperature() {};
  };

}

#endif
