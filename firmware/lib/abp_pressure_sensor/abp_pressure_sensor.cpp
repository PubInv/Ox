// Public Invention's Ox Project is an open source hardware design for an oxygen
// concentrator for use by field hospitals around the world. This team aims to
// design an oxygen concentrator that can be manufactured locally while overcoming
// challenges posed by human resources, hospital location (geographically),
// infrastructure and logistics; in addition, this project attempts the minimum
// documentation expected of their design for international approval whilst
// tackling regulatory requirements for medical devices. Copyright (C) 2021
// Robert Read, Ben Coombs, and Darío Hereñú.

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
#include <abp_pressure_sensor.h>
#include <core.h>

namespace OxSensor {
    AbpPressureSensor::AbpPressureSensor(SensorConfig &config) {
        _config = config;
#ifdef ARDUINO
        pinMode(config.pin, OUTPUT);
        //SPI.begin();
#endif
    }

    float AbpPressureSensor::ReadPressure() {
#ifdef ARDUINO
        OxCore::Debug<const char *>("Selecting sensor: ");
        OxCore::DebugLn<uint8_t>(_config.pin);
        digitalWrite(_config.pin, LOW);
        //SPI.beginTransaction(SPISettings(100000, LSBFIRST, SPI_MODE1));
        SPI.begin();
        uint8_t val1 = SPI.transfer(0);
        uint8_t val2 = SPI.transfer(0);
        uint8_t val3 = SPI.transfer(0);
        uint8_t val4 = SPI.transfer(0);
        
        OxCore::Debug<const char *>("SPI transfered.\n");
        
        //SPI.endTransaction();
        digitalWrite(_config.pin, HIGH);
        
        OxCore::Debug<const char *>("Count: ");
        OxCore::DebugLn<uint8_t>(val1);
        OxCore::DebugLn<uint8_t>(val2);
        OxCore::DebugLn<uint8_t>(val3);
        OxCore::DebugLn<uint8_t>(val4);
        
        // OxCore::Debug<const char *>("boop4\n");
        // From table 6 of the ABP datasheet
        // const uint32_t count_at_10_percent = 1638; // 0x0666
        // const uint32_t count_at_80_percent = 13106;
        // const uint32_t count_at_90_percent = 14745; // 0x3999
        // const uint32_t count_at_100_percent = 16383; // 2^14
        // uint32_t deltaPressure = _config.pressureMax - _config.pressureMin;
        // float y = stat/count_at_100_percent;
        // _pressure = ((deltaPressure * (y - count_at_10_percent))/count_at_80_percent) + _config.pressureMax;

        _pressure = (((0x2777 - 0x0666) * (_config.pressureMax - _config.pressureMin))/(0x3999 - 0x0666)) + _config.pressureMin;
        return _pressure;
#else
        // Mock sensor
        _pressure = (_config.pressureMax - _config.pressureMin)/2;
        return _pressure;
#endif
    }

    float AbpPressureSensor::GetPressure() const {
        return _pressure;
    }

    SensorConfig AbpPressureSensor::GetConfig() const {
        return _config;
    }
}
