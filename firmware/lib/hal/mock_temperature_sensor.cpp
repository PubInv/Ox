// Copyright (C) 2021
// Robert Read, Ben Coombs.

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
#include <mock_temperature_sensor.h>
#include <core.h>


namespace Temperature {
  MockTemperatureSensor::MockTemperatureSensor() {
    }

  MockTemperatureSensor::MockTemperatureSensor(OxApp::Model& m,SensorConfig &config) {
    _m = m;
        _config = config;
    }
  MockTemperatureSensor::MockTemperatureSensor(SensorConfig &config) {
        _config = config;
    }

  float MockTemperatureSensor::ReadTemperature() {
        // Mock sensor
        _temperature = (_config.temperatureMax - _config.temperatureMin)/2;
        return _temperature;
    }
  float MockTemperatureSensor::GetTemperature(int idx) {
    return GetTemperature(0);
    }

  float MockTemperatureSensor::GetTemperature() {
        return _temperature;
    }

  SensorConfig MockTemperatureSensor::GetConfig() const {
        return _config;
    }
}
