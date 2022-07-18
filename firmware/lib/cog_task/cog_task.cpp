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
#include<cmath>
#include <abstract_temperature.h>

using namespace std;


namespace OxApp
{

    Temperature::SensorConfig config[3] = {
        {
            0,
            Temperature::SensorMode::SPI,
            Temperature::TemperatureUnits::C,
            45,
            1,
            0,
            30
        },
        {
            1,
            Temperature::SensorMode::SPI,
            Temperature::TemperatureUnits::C,
            47,
            1,
            0,
            30
        },
        {
            2,
            Temperature::SensorMode::SPI,
            Temperature::TemperatureUnits::C,
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
        RunForward(1.0,model);

        OxCore::Debug<const char *>("Exhaust Temperature (C): ");
        OxCore::DebugLn<float>(model.locations[1].temp_C);

        return true;
    }

  static float compute_change_in_voltage(float current_C,float current_V,float desired_C) {
  }

    void CogTask::_updatePowerComponents() {
        OxCore::Debug<const char *>("_updateHeaterPrims\n");
        // For now, i want to do only the first heater to simplify
        for (int i = 0; i < NUM_HEATERS && i < 1; i++) {
            OxCore::Debug<const char *>("Checking resistance ");
            OxCore::DebugLn<float>(_heaters[i]._resistance);
            OxCore::DebugLn<float>(_heaters[i]._voltage);
            // actually we will compute the new voltage based on
            // whether we are hitting our thermostatic set point or not
            // This will be a really simple model for now...
            float current_C = model.locations[1].temp_C;
            float current_V = _heaters[i]._voltage;
            float desired_C = 650.0;
            float voltage = _heaters[i].
              compute_change_in_voltage(current_C,
                                        current_V,
                                        desired_C,
                                        model.watts_per_degree);

            _heaters[i].update(voltage);
            OxCore::Debug<const char *>("Checking Voltage Set ");
            OxCore::DebugLn<float>(_heaters[i]._voltage);

        }
        OxCore::Debug<const char *>("Checking Voltage Set AAA ");
        OxCore::DebugLn<float>(_heaters[0]._voltage);

    }
  void CogTask::RunForward(float t,Model& m) {
    // This math only works for 1 second, I think
    float watts = pow(_heaters[0]._voltage,2) / _heaters[0]._resistance;
    float degrees_delta = watts / m.watts_per_degree;
    // now really need to know the airflow

    OxCore::Debug<const char *>("Heater 1 air flow degrees delta ");
    OxCore::DebugLn<float>(degrees_delta);

    // We'll assume that the watts added directly increase the
    // the temperature at the output
    m.locations[1].temp_C = m.locations[0].temp_C + degrees_delta;
  }

    void CogTask::_configTemperatureSensors() {
        OxCore::Debug<const char *>("_configPressureSensors\n");


#ifdef RIBBONFISH
        _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::DS18B20Temperature[1];
        _temperatureSensors[0]._config = config[0];
#else
        _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MockTemperatureSensor[NUM_TEMPERATURE_SENSORS];
        for(int i = 0; i < NUM_TEMPERATURE_SENSORS; i++) {
          _temperatureSensors[i]._config = config[i];

        }
#endif

    }

    void CogTask::_readTemperatureSensors() {
      OxCore::Debug<const char *>("AAA: ");
#ifdef RIBBONFISH
      float temperature = _temperatureSensors[0].ReadTemperature();
      OxCore::Debug<const char *>("DDD: ");
      for (int i = 0; i < NUM_TEMPERATURE_SENSORS; i++) {
        float temperature = _temperatureSensors[0].GetTemperature(i);
        OxCore::Debug<const char *>("Temperature: ");
        OxCore::DebugLn<float>(temperature);
      }
#else
      OxCore::Debug<const char *>("DDD: ");
      for (int i = 0; i < NUM_TEMPERATURE_SENSORS; i++) {
         _temperatureSensors[i].ReadTemperature();
        float temperature = _temperatureSensors[i].GetTemperature();
        OxCore::Debug<const char *>("Temperature: ");
        OxCore::DebugLn<float>(temperature);
      }
#endif

    }
}
