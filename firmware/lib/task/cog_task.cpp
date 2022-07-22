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


#ifdef RIBBONFISH
        // Our pre-heater measured 5.8 ohms
        // Our main heater measured 5.6 ohms
        // We wil use DUE pins 4 and 5 for these heaters
        // (3 will be for the fan)
        // These are PWM pins if we need them...
        Heater v1("PRIMARY_HEATER", 1, 4, 0, 5.8);
        _heaters[0] = v1;

        Heater v2("SECONDARY_HEATER",2, 5, 0, 5.6);
        _heaters[1] = v2;
#else
        // Create a one ohm joule heater
        Heater v1("PRIMARY_HEATER", 1, 50, 5.3, 1.0);
        _heaters[0] = v1;
        // Create a two ohm (smaller) heater for the pure O2 stream
        Heater v2("SECONDARY_HEATER",2, 51, 3.6, 2.0);
        _heaters[1] = v2;

#endif

        return true;
    }

    bool CogTask::_run()
    {
        _readTemperatureSensors();
        MachineState *ms = (MachineState *) _properties.state_and_config;

        MachineState new_state = _executeBasedOnState(*ms);
        // if the state really changes, we want to log that and take some action!

        // Somewhere we have a true clock value, I would have thought
        // it would be an input to this routine....
#ifndef RIBBONFISH
        RunForward(1.0,model);

        OxCore::Debug<const char *>("Exhaust Temperature (C): ");
        OxCore::DebugLn<float>(model.locations[1].temp_C);
#endif
        return true;
    }

  static float compute_change_in_voltage(float current_C,float current_V,float desired_C) {
  }

  // There is significant COG dependent logic here.
  // At the expense of extra lines of code, I'm
  // going to keep this mostly simple by making it look
  // "table-driven"
  MachineState CogTask::_executeBasedOnState(MachineState ms) {
    MachineState new_ms;
    switch(ms) {
    case Off:
        new_ms = _updatePowerComponentsOff();
      break;
    case Warmup:
        new_ms = _updatePowerComponentsWarmup();
      break;
    case Operation:
        new_ms = _updatePowerComponentsOperation();
      break;
    case Idle:
        new_ms = _updatePowerComponentsIdle();
      break;
    case Cooldown:
        new_ms = _updatePowerComponentsCooldown();
      break;
    case CriticalFault:
        new_ms = _updatePowerComponentsCritialFault();
      break;
    case EmergencyShutdown:
        new_ms = _updatePowerComponentsEmergencyShutdown();
      break;
    case OffUserAck:
        new_ms = _updatePowerComponentsOffUserAck();
        break;
    default:
      OxCore::Debug<const char *>("INTERNAL ERROR: UNKOWN MACHINE STATE\n");
      // This is not really enough information; we need a way to
      // record what the fault is, but it will do for now.
      new_ms = CriticalFault;
    }
    return new_ms;

  }
  MachineState CogTask::_updatePowerComponentsOff() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsIdle() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    return new_ms;
  }

#ifdef RIBBONFISH

  // The RibbonFish has two heaters. We just want to turn them on and off as a simple
  // thermostate problem, using the PWMs of our dues to power transistors.
  // However, the process should be slow enough, we can just use "full on" and "full off"
  // as a first cut.
   MachineState CogTask::_updatePowerComponentsOperation() {
        // For now, i want to do only the first heater to simplify
        int heater_indices[2];
        heater_indices[0] = 0;
        heater_indices[1] = 1;
        for (int i = 0; i < NUM_HEATERS && i < 2; i++) {

        float temperature = _temperatureSensors[0].GetTemperature(heater_indices[i]);
        // It would be nice to tie this temperature
        // to the model location, but that will have to wait!
        // float current_C = model.locations[1].temp_C;
        //            float current_V = _heaters[i]._voltage;
        float current_C = temperature;
        float desired_C = 30;
        // We could compute the actual voltage, but
        // we will use the simpler on/off algorithm here...
        float voltage = (current_C >= desired_C) ? 0.0 : 12.0;
        _heaters[i].update(voltage);
        }

    }
#else
  // This is a mock, simulated update
    MachineState CogTask::_updatePowerComponentsOperation() {
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
#endif

  void CogTask::RunForward(float t,Model& m) {
    // This math only works for 1 second, I think
    float watts = pow(_heaters[0]._voltage,2) / _heaters[0]._resistance;
    float degrees_delta = watts / m.watts_per_degree;
    // now really need to know the airflow

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
#ifdef RIBBONFISH
      float temperature = _temperatureSensors[0].ReadTemperature();
      for (int i = 0; i < NUM_TEMPERATURE_INDICES; i++) {
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
