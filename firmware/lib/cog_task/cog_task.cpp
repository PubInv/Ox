/*
This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "cog_task.h"

namespace OxApp
{

    MockTemp::SensorConfig config[3] = {
        {
            0,
            MockTemp::SensorMode::SPI,
            MockTemp::TemperatureUnits::C,
            45,
            1,
            0,
            30
        },
        {
            1,
            MockTemp::SensorMode::SPI,
            MockTemp::TemperatureUnits::C,
            47,
            1,
            0,
            30
        },
        {
            2,
            MockTemp::SensorMode::SPI,
            MockTemp::TemperatureUnits::C,
            49,
            1,
            0,
            30
        }
    };

    bool CogTask::_init()
    {
        OxCore::Debug<const char *>("CogTask init\n");

        _configTemperatureSensors();

        // Create a one ohm joule heater
        Heater v1("PRIMARY_HEATER", 1, 50, 5.3, 1.0);
        _heaters[0] = v1;
        // Create a two ohm (smaller) heater for the pure O2 stream
        Heater v2("SECONDARY_HEATER",2, 51, 3.6, 2.0);
        _heaters[1] = v2;

        return true;
    }

    bool CogTask::_run()
    {
        OxCore::Debug<const char *>("CogTask run\n");

        _readTemperatureSensors();

        _updatePowerComponents();

        // Somewhere we have a true clock value, I would have thought
        // it would be an input to this routine....
        model.RunForward(1.0,this);

        OxCore::Debug<const char *>("Exhaust Temperature (C): ");
        OxCore::DebugLn<float>(model.locations[1].temp_C);

        return true;
    }

    void CogTask::_updatePowerComponents() {
        OxCore::Debug<const char *>("_updateHeaterPrims\n");
        for (int i = 0; i < NUM_HEATERS; i++) {
            Heater heater = _heaters[i];
            OxCore::Debug<const char *>("Checking resistance ");
            OxCore::DebugLn<float>(_heaters[i]._resistance);
            OxCore::DebugLn<float>(_heaters[i]._voltage);
            // actually we will compute the new voltage based on
            // whether we are hitting our thermostatic set point or not
            float voltage = 13.2;

            _heaters[i].update(voltage);
            OxCore::Debug<const char *>("Checking Voltage Set ");
            OxCore::DebugLn<float>(_heaters[i]._voltage);

        }
        OxCore::Debug<const char *>("Checking Voltage Set AAA ");
        OxCore::DebugLn<float>(_heaters[0]._voltage);

    }

    void CogTask::_configTemperatureSensors() {
        OxCore::Debug<const char *>("_configPressureSensors\n");
        MockTemp::MockTemperatureSensor sensor1(config[0]);
        _temperatureSensors[0] = sensor1;

        MockTemp::MockTemperatureSensor sensor2(config[1]);
        _temperatureSensors[1] = sensor2;

        MockTemp::MockTemperatureSensor sensor3(config[2]);
        _temperatureSensors[2] = sensor3;

    }

    void CogTask::_readTemperatureSensors() {
        for (int i = 0; i < NUM_TEMPERATURE_SENSORS; i++) {
            float temperature = _temperatureSensors[i].ReadTemperature();
            OxCore::Debug<const char *>("Temperature: ");
            OxCore::DebugLn<float>(temperature);
        }
    }
}
