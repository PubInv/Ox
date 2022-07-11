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

        Heater v1("PRIMARY_HEATER", 1, 50, 500, 1000);
        _heaters.add(v1);
        Heater v2("SECONDARY_HEATER",2, 51, 1000, 1500);
        _heaters.add(v2);

        _valveCycleTimer.Init(OxCore::Timer::TimeSinceEpochMs());


        return true;
    }

    bool CogTask::_run()
    {
        OxCore::Debug<const char *>("CogTask run\n");

        _readTemperatureSensors();

        _updatePowerComponents();

        // if (elapsed >= TOTAL_CYCLE_TIME)
        // {
        //     valveCycleTimer.Reset();
        //     //vc.resetValves();
        // } else {
        //     //vc.updateController(elapsed);
        //     //_printValveState(vc.getValveBits());
        // }

        return true;
    }

    void CogTask::_updatePowerComponents() {
        OxCore::Debug<const char *>("_updateHeaterPrims\n");
        uint32_t elapsed = _valveCycleTimer.Update();
        OxCore::DebugLn<uint32_t>(elapsed);
        for (int i = 0; i < _heaters.size(); i++) {
            Heater heater = _heaters.get(i);
            heater.update(elapsed);
        }
        if (elapsed > 2600) {
            _valveCycleTimer.Reset();
        }
    }

    void CogTask::_configTemperatureSensors() {
        OxCore::Debug<const char *>("_configPressureSensors\n");
        MockTemp::MockTemperatureSensor sensor1(config[0]);
        _temperatureSensors.add(sensor1);

        MockTemp::MockTemperatureSensor sensor2(config[1]);
        _temperatureSensors.add(sensor2);

        MockTemp::MockTemperatureSensor sensor3(config[2]);
        _temperatureSensors.add(sensor3);

        // for (int i = 0; i < _temperatureSensors.size(); i++) {
        //     OxCore::Debug<const char *>("A\n");
        //     MockTemp::AbpPressureSensor sensor = _temperatureSensors.get(i);
        //     sensor.Config(config[i]);
        // }
    }

    void CogTask::_readTemperatureSensors() {
        OxCore::Debug<const char *>("_readTemperatureSensors\n");
        OxCore::DebugLn<int>(_temperatureSensors.size());
        for (int i = 0; i < _temperatureSensors.size(); i++) {
            MockTemp::MockTemperatureSensor sensor = _temperatureSensors.get(i);
            float temperature = sensor.ReadTemperature();
            OxCore::Debug<const char *>("Temperature: ");
            OxCore::DebugLn<float>(temperature);
        }
    }
}
