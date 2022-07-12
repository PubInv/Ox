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

#ifndef MOCK_TEMPERATURE_SENSOR
#define MOCK_TEMPERATURE_SENSOR

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <model.h>

namespace MockTemp {
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

    class MockTemperatureSensor {
        private:
            SensorConfig _config;
            float _temperature;
        public:
      OxApp::Model _m;
      MockTemperatureSensor();
      MockTemperatureSensor(OxApp::Model& m,SensorConfig &config);
            MockTemperatureSensor(SensorConfig &config);
            ~MockTemperatureSensor() {};
            void Config(SensorConfig &config);
            float ReadTemperature();
            float GetTemperature() const;
            SensorConfig GetConfig() const;
    };

}

#endif
