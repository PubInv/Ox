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
#include <TF800A12K.h>

using namespace std;


#define DEBUG_LEVEL 1

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

  // TODO: Most of this should be moved into the machine definition
    bool CogTask::_init()
    {
        OxCore::Debug<const char *>("CogTask init\n");

        _configTemperatureSensors();


#ifdef RIBBONFISH
        // Our pre-heater measured 5.8 ohms
        // Our main heater measured 5.6 ohms
        Heater v1("PRIMARY_HEATER", 1, RF_HEATER, 0, 5.8);
        _heaters[0] = v1;

        //        Heater v2("SECONDARY_HEATER",2, 5, 0, 5.6);
        //        _heaters[1] = v2;

        //        Fan f("FIRST_FAN",0,RF_FAN,1.0);
        //        _fans[0] = f;
        _fans[0] = DeltaFans("FIRST_FAN",0,RF_FAN,1.0);
        _fans[0]._init();
        getConfig()->fanPWM = 0.0;

        //        _stacks[0] = new Stack("FIRST_STACK",0,RF_STACK,1.0);

        _stacks[0] = new SL_PS("FIRST_STACK",0);

        // This would be better done in the setup, really, but
        // that should remain generic...I'm afraid I need to regularize all
        // of this but am out of time -- RLR
        _stacks[0]->init();
        _flowsensor = new SensirionFlow();

#else
        // Create a one ohm joule heater
        Heater v1("PRIMARY_HEATER", 1, 50, 5.3, 1.0);
        _heaters[0] = v1;
        // Create a two ohm (smaller) heater for the pure O2 stream
        //        Heater v2("SECONDARY_HEATER",2, 51, 3.6, 2.0);
        //        _heaters[1] = v2;

#endif
        return true;
    }

    bool CogTask::_run()
    {


      MachineConfig *cogConfig = getConfig();
      // If we are in the off state there is nothing to do!
      if (cogConfig->ms == OffUserAck) {
          OxCore::DebugLn<const char *>("AN ERROR OCCURED. WILL NOT ENTER OFF STATE ");
          OxCore::DebugLn<const char *>("UNTIL ACKNOWLEDGED. ENTER A SINGLE 'a' TO ACKNOWLEDGE:");
          return true;
      }
      if (cogConfig->ms == Off) {
          OxCore::DebugLn<const char *>("Currrently Off. Enter a single 'w' to warmup: ");
      }
      _readTemperatureSensors();
      MachineState new_state = _executeBasedOnState(cogConfig->ms);
      // if the state really changes, we want to log that and take some action!
      if (new_state != cogConfig->ms) {
        cogConfig->ms = new_state;
        OxCore::Debug<const char *>("CHANGING STATE TO: ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[cogConfig->ms]);
        OxCore::DebugLn<const char *>("");
      }

      OxCore::Debug<const char *>("FLOW SLM: ");
      float flow_slm = _flowsensor->flowInSLM();
      OxCore::DebugLn<float>(flow_slm);


      // Somewhere we have a true clock value, I would have thought
      // it would be an input to this routine....
#ifndef RIBBONFISH
      RunForward(1.0,model);

      OxCore::Debug<const char *>("Exhaust Temperature (C): ");
      OxCore::DebugLn<float>(model.locations[1].temp_C);
#endif

#ifdef RIBBONFISH
      outputReport(getConfig()->report);
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

    if (DEBUG_LEVEL > 0) {
      OxCore::Debug<const char *>("\nMachine State: ");
      OxCore::Debug<const char *>(getConfig()->MachineStateNames[ms]);
      OxCore::Debug<const char *>(" : ");
      OxCore::DebugLn<const char *>(getConfig()->MachineSubStateNames[getConfig()->idleOrOperate]);
    }
    switch(ms) {
    case Off:
        new_ms = _updatePowerComponentsOff();
      break;
    case Warmup:
        new_ms = _updatePowerComponentsWarmup();
      break;
    case NormalOperation:
      new_ms = _updatePowerComponentsOperation(getConfig()->idleOrOperate);
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
    MachineState new_ms = Off;
    _updatePowerComponentsVoltage(0);
    getConfig()->report.heater_voltage = 0.0;

    getConfig()->fanPWM = 0.0;
    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;

    _updateStackVoltage(0.0);
    getConfig()->report.stack_voltage = 0.0;

    bool flowing = _flowsensor->isAirFlowing();
    if (flowing) {
      OxCore::Debug<const char *>("POTENTIAL ERROR, AIR IS STILL FLOWING ");
    }
    getConfig()->report.air_flow_sufficient = flowing;

    delay(10);
    double flow_ml_per_s = _flowsensor->flowIn_ml_per_s();

    if (flow_ml_per_s >= 4550.0) {
      // This is the error condition for this sensor!
    } else {
      getConfig()->report.flow_ml_per_s = flow_ml_per_s;
    }
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    OxCore::Debug<const char *>("Warmup Mode! ");
    // This algorithm will have to be improved, but
    // for now, we check the temperature, and turn the heaters on
    // full-blast until we reach the warmup-target
    // This will be based on the post-heater temperature
    float postHeaterTemp;
    float postStackTemp;

#ifdef RIBBONFISH
    postHeaterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[0]);
    postStackTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[0]);
    getConfig()->report.post_heater_C = postHeaterTemp;
    getConfig()->report.post_stack_C = postStackTemp;
#else
    postHeaterTemp = model.locations[1].temp_C;
#endif
    if ((postHeaterTemp >= getConfig()->WARMUP_TARGET_C) ||
        (postStackTemp >= getConfig()->WARMUP_TARGET_C)) {
      new_ms = NormalOperation;
      getConfig()->idleOrOperate = Operate;
      _updatePowerComponentsOperation(getConfig()->idleOrOperate);
    } else {
      _updatePowerComponentsVoltage(getConfig()->MAXIMUM_HEATER_VOLTAGE);
    }
    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;


    _updateStackVoltage(getConfig()->MAXIMUM_STACK_VOLTAGE);
    getConfig()->report.stack_voltage = getConfig()->MAXIMUM_STACK_VOLTAGE;
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsIdle() {
    OxCore::Debug<const char *>("IN IDLE FUNCTION ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;

    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;

    _updateStackVoltage(0.0);
    getConfig()->report.stack_voltage = 0.0;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    float postHeaterTemp;
    float postStackTemp;
#ifdef RIBBONFISH
    postHeaterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[0]);
    postStackTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[0]);
    getConfig()->report.post_heater_C = postHeaterTemp;
    getConfig()->report.post_stack_C = postStackTemp;
#else
    postHeaterTemp = model.locations[1].temp_C;
    postStackTemp = model.locations[2].temp_C;
#endif
    if (postHeaterTemp <= getConfig()->COOLDOWN_TARGET_C) {
      new_ms = Off;
    } else {
      _updatePowerComponentsVoltage(0);
    }
    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;


    getConfig()->report.fan_speed = 1.0 ;
    getConfig()->report.stack_voltage = 0.0;

    bool flowing = _flowsensor->isAirFlowing();
    if (!flowing) {
      OxCore::Debug<const char *>("POTENTIAL ERROR, AIR FLOW MAY BE INSUFFICIENT ");
    }
    getConfig()->report.air_flow_sufficient = flowing;

    delay(10);
    double flow_ml_per_s = _flowsensor->flowIn_ml_per_s();

    if (flow_ml_per_s >= 4550.0) {
      // This is the error condition for this sensor!
    } else {
      getConfig()->report.flow_ml_per_s = flow_ml_per_s;
    }

    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;
    _updateStackVoltage(0.0);
    getConfig()->report.fan_speed = 1.0;
    getConfig()->report.stack_voltage = 0.0;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
    _updatePowerComponentsVoltage(0);
    MachineState new_ms = OffUserAck;

    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;
    _updateStackVoltage(0.0);
    getConfig()->report.fan_speed = 1.0;
    getConfig()->report.stack_voltage = 0.0;
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;

    _updateFanSpeed(getConfig()->fanPWM);
    getConfig()->report.fan_speed = getConfig()->fanPWM;
    _updateStackVoltage(0.0);
    getConfig()->report.fan_speed = 1.0;
    getConfig()->report.stack_voltage = 0.0;
    return new_ms;
  }
  // This is use primarily for maximum power at warmup or
  // maximum cooldown when we don't have to compute based on temperature
   void CogTask::_updatePowerComponentsVoltage(float voltage) {
        for (int i = 0; i < NUM_HEATERS; i++) {
        _heaters[i].update(voltage);
        }
    }

  // updateTheFanSpeed to a percentage of the maximum flow.
  // We may have the ability to specify flow absolutely in the future,
  // but this is genertic.
   void CogTask::_updateFanSpeed(float percentage) {
        for (int i = 0; i < NUM_FANS; i++) {
        _fans[i].update(percentage);
        }
    }

  // In theory, this could take an array rather than
  // a fixed voltage; that will have to be added later
   void CogTask::_updateStackVoltage(float voltage) {
        for (int i = 0; i < NUM_STACKS; i++) {
          _stacks[i]->updateVoltage(voltage);
        }
    }

#ifdef RIBBONFISH

  // The RibbonFish has two heaters. We just want to turn them on and off as a simple
  // thermostate problem, using the PWMs of our dues to power transistors.
  // However, the process should be slow enough, we can just use "full on" and "full off"
  // as a first cut.
   MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
     MachineState new_ms = NormalOperation;
     float desired_C = getConfig()->MAX_POST_HEATER_C;
     // For now, i want to do only the first heater to simplify
     for (int i = 0; i < NUM_HEATERS; i++) {
       float temperature = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[i]);
       float current_C = temperature;
       float voltage = (current_C >= desired_C) ? 0.0 : getConfig()->MAXIMUM_HEATER_VOLTAGE;

       // These two statements are really atomic, but I don't what the devices to become dependent on the machine!!
       _heaters[i].update(voltage);
       getConfig()->report.heater_voltage = voltage;

     }
     for (int i = 0; i < NUM_STACKS; i++) {
       float temperature = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[i]);
       float current_C = temperature;
       getConfig()->report.post_stack_C = current_C;
       float max_post_stack_C = getConfig()->MAX_POST_STACK_C;
       // TODO: In actuality, we need something more controllable
       // than MAX on and zero off!
       float voltage = (current_C >= max_post_stack_C) ? 0.0 : getConfig()->MAXIMUM_STACK_VOLTAGE;
       // This is a little complicated, because we may actually
       // want a tricle charge---cut for now, if we are in Idle
       // mode, we just set the stack voltage to zero
       if (i_or_o == Idle) {
         voltage = 0.0;
       }
       _updateStackVoltage(voltage);
       getConfig()->report.stack_voltage = voltage;
     }
     float postHeaterTemp;
     float postStackTemp;
     postHeaterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[0]);
    postStackTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[0]);
    getConfig()->report.post_heater_C = postHeaterTemp;
    getConfig()->report.post_stack_C = postStackTemp;


     if (postHeaterTemp < getConfig()->WARMUP_TARGET_C) {
       return Warmup;
     }
     _updateFanSpeed(getConfig()->fanPWM);
     getConfig()->report.fan_speed = getConfig()->fanPWM;

     bool flowing = _flowsensor->isAirFlowing();
    if (!flowing) {
      OxCore::Debug<const char *>("POTENTIAL ERROR, AIR FLOW MAY BE INSUFFICIENT ");
    }
    getConfig()->report.air_flow_sufficient = flowing;
    delay(10);
    double flow_ml_per_s = _flowsensor->flowIn_ml_per_s();

    if (flow_ml_per_s >= 4550.0) {
      // This is the error condition for this sensor!
    } else {
      getConfig()->report.flow_ml_per_s = flow_ml_per_s;
    }
     return new_ms;
    }
#else
  // This is a mock, simulated update
    MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
      MachineState new_ms = NormalOperation;
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
            float desired_C = getConfig()->MAX_POST_HEATER_C;
            float voltage = _heaters[i].
              compute_change_in_voltage(current_C,
                                        current_V,
                                        desired_C,
                                        model.watts_per_degree);

            _heaters[i].update(voltage);
            OxCore::Debug<const char *>("Checking Voltage Set ");
            OxCore::DebugLn<float>(_heaters[i]._voltage);

        }
     _updateFanSpeed(getConfig()->fanPWM);
     getConfig()->report.fan_speed = getConfig()->fanPWM;
    _updateStackVoltage(getConfig()->MAXIMUM_STACK_VOLTAGE);
    OxCore::Debug<const char *>("Checking Voltage Set AAA ");
        OxCore::DebugLn<float>(_heaters[0]._voltage);
        return new_ms;
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
#ifdef USE_MAX31850_THERMOCOUPLES
        _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MAX31850Temperature[1];
#else
        _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::DS18B20Temperature[1];
#endif
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
      _temperatureSensors[0].ReadTemperature();
      for (int i = 0; i < NUM_TEMPERATURE_INDICES; i++) {
        float temperature = _temperatureSensors[0].GetTemperature(i);
        OxCore::Debug<const char *>("Temp : ");
        OxCore::Debug<const char *>(getConfig()->TempLocationNames[i]);
        OxCore::Debug<const char *>(": ");
        OxCore::DebugLn<float>(temperature);
      }
#else
      OxCore::Debug<const char *>("DDD: ");
      for (int i = 0; i < NUM_TEMPERATURE_SENSORS; i++) {
         _temperatureSensors[i].ReadTemperature();
        float temperature = _temperatureSensors[i].GetTemperature();
        OxCore::Debug<const char *>("Temp: ");
        OxCore::Debug<const char *>(getConfig()->TempLocationNames[i]);
        OxCore::Debug<const char *>(": ");
        OxCore::DebugLn<float>(temperature);
      }
#endif

    }
}
