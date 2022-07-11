/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#include "psa_task.h"

namespace OxApp
{    

    OxSensor::SensorConfig config[3] = {
        {
            0,
            OxSensor::SensorMode::SPI,
            OxSensor::PressureUnits::PSI,
            45,
            1,
            0,
            30
        },
        {
            1,
            OxSensor::SensorMode::SPI,
            OxSensor::PressureUnits::PSI,
            47,
            1,
            0,
            30
        },
        {
            2,
            OxSensor::SensorMode::SPI,
            OxSensor::PressureUnits::PSI,
            49,
            1,
            0,
            30
        }
    };

    bool PsaTask::_init()
    {
        OxCore::Debug<const char *>("PsaTask init\n");
        
        _configPressureSensors();

        Valve v1("Fill_A", 1, 50, 500, 1000);    
        _valves.add(v1);
        Valve v2("Exhaust_A",2, 51, 1000, 1500);
        _valves.add(v2);
        Valve v3("Fill_B",3, 52, 1500, 2000);
        _valves.add(v3);
        Valve v4("Exhaust_B",4, 53, 2000, 2500);
        _valves.add(v4);
        Valve v5("Balance",5, 54, 2500, 2600);
        _valves.add(v5);

        _valveCycleTimer.Init(OxCore::Timer::TimeSinceEpochMs());

        
        return true;
    }

    bool PsaTask::_run() 
    {
        OxCore::Debug<const char *>("PsaTask run\n");
        
        _readPressureSensors();

        _updateValves();

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

    void PsaTask::_updateValves() {
        OxCore::Debug<const char *>("_updateValves\n");
        uint32_t elapsed = _valveCycleTimer.Update();
        OxCore::DebugLn<uint32_t>(elapsed);
        for (int i = 0; i < _valves.size(); i++) {
            Valve valve = _valves.get(i);
            valve.update(elapsed);
        }
        if (elapsed > 2600) {
            _valveCycleTimer.Reset();
        }
    }

    void PsaTask::_configPressureSensors() {
        OxCore::Debug<const char *>("_configPressureSensors\n");
        OxSensor::AbpPressureSensor sensor1(config[0]);
        _pressureSensors.add(sensor1);
        
        OxSensor::AbpPressureSensor sensor2(config[1]);
        _pressureSensors.add(sensor2);
        
        OxSensor::AbpPressureSensor sensor3(config[2]);
        _pressureSensors.add(sensor3);
        
        // for (int i = 0; i < _pressureSensors.size(); i++) {
        //     OxCore::Debug<const char *>("A\n");
        //     OxSensor::AbpPressureSensor sensor = _pressureSensors.get(i);
        //     sensor.Config(config[i]);
        // }
    }

    void PsaTask::_readPressureSensors() {
        OxCore::Debug<const char *>("_readPressureSensors\n");
        OxCore::DebugLn<int>(_pressureSensors.size());
        for (int i = 0; i < _pressureSensors.size(); i++) {
            OxSensor::AbpPressureSensor sensor = _pressureSensors.get(i);
            float pressure = sensor.ReadPressure();
            OxCore::Debug<const char *>("Pressure: ");
            OxCore::DebugLn<float>(pressure);
        }
    }
}
