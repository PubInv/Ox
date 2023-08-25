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
#include <cmath>
#include <abstract_temperature.h>
#include <TF800A12K.h>

using namespace std;


#define DEBUG_LEVEL 0

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

        getConfig()->report = new MachineStatusReport();

        //        _configTemperatureSensors();


#ifdef RIBBONFISH


          // Our pre-heater measured 5.8 ohms
        // Our main heater measured 5.6 ohms
        //        Heater v1("PRIMARY_HEATER", 1, RF_HEATER, 0, 5.8);
        //        _heaters[0] = v1;
        getConfig()->fanDutyCycle = 0.0;

        _stacks[0] = new SL_PS("FIRST_STACK",0);

        _stacks[0]->init();

        // We know that in general NASA wants us to implement current
        // control of the stack. However, at present, this code
        // is doing voltage control. We therefore set the maximum
        // current to that specified in the config file, and
        // then control the heat by varying voltage.
        // This strategy can and should be reversed; they would
        // be equivalent except that it is in general more convenient
        // to think of the amperage consumed by the stack because that
        // is proportional (constant unkown) to the number of O2 ions
        // produced!
        _updateStackAmperage(getConfig()->TARGET_STACK_CURRENT_mA/1000.0);

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

  // this is now handled by a separate task!
  // void CogTask::updateTemperatures() {
  //   _readTemperatureSensors();
  //   double postHeaterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[0]);
  //   double postStackTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[0]);
  //   double postGetterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_getter_indices[0]);
  //   getConfig()->report->post_heater_C = postHeaterTemp;
  //   getConfig()->report->post_stack_C = postStackTemp;
  //   getConfig()->report->post_getter_C = postGetterTemp;

  // }
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

      //      updateTemperatures();
      getConfig()->_reportFanSpeed();

      MachineState new_state = _executeBasedOnState(cogConfig->ms);
      // if the state really changes, we want to log that and take some action!
      if (new_state != cogConfig->ms) {
        cogConfig->ms = new_state;
        OxCore::Debug<const char *>("CHANGING STATE TO: ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[cogConfig->ms]);
        OxCore::DebugLn<const char *>("");
      }

      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("FLOW SLM: ");
      }
      outputReport(getConfig()->report);
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
    getConfig()->previous_ms = ms;
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsOff() {
    MachineState new_ms = Off;
    _updatePowerComponentsVoltage(0);

    getConfig()->fanDutyCycle = 0.0;

    getConfig()->_updateFanPWM(getConfig()->fanDutyCycle);

    _updateStackVoltage(0.0);

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

    // Here we get the temperature and decide if the TARGE_TEMP should be changed!
    // This is highly sensitive to the START_TEMPERATURE being correct;
    // I believe a rate-based algorithm will be more robust. We have the
    // machinery for that, but I am doing this simpler algorithm for now.
    if (getConfig()->report->post_heater_C > MachineConfig::HIGH_TEMPERATURE_FAN_SLOW_DOWN_LIMIT) {
      getConfig()->_updateFanPWM(MachineConfig::HIGH_TEMPERATURE_FAN_PWM);
    } else {
      getConfig()->_updateFanPWM(1.0);
    }


      if (postHeaterTemp > MachineConfig::HOLD_TEMPERATURE) {
        OxCore::Debug<const char *>("State Changing to HOLDING\n");
        new_ms = NormalOperation;
        return new_ms;
      } else {
        const unsigned long ms = millis();
        const unsigned long MINUTES_RAMPING_UP = ms / (60 * 1000);
        getConfig()->TARGET_TEMP = MachineConfig::START_TEMPERATURE + MINUTES_RAMPING_UP * MachineConfig::RAMP_UP_TARGET_D_MIN;
        getConfig()->TARGET_TEMP = min(getConfig()->TARGET_TEMP,
                                       MachineConfig::HOLD_TEMPERATURE);
      }



    _updateStackVoltage(getConfig()->MAXIMUM_STACK_VOLTAGE);
    _updateStackAmperage(getConfig()->TARGET_STACK_CURRENT_mA/1000.0);

    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsIdle() {
    OxCore::Debug<const char *>("IN IDLE FUNCTION ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;
    _updateStackVoltage(0.0);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;

    if (getConfig()->previous_ms != Cooldown) {
      getConfig()->COOL_DOWN_BEGIN_TEMPERATURE = getConfig()->report->post_heater_C;
    }

    if (getConfig()->report->post_heater_C <= getConfig()->COOLDOWN_TARGET_C
        &&
        getConfig()->report->post_stack_C <= getConfig()->COOLDOWN_TARGET_C) {
      new_ms = Off;
      return new_ms;
    } else {
      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("State: RAMPING DN\n");
      }
      float postHeaterTemp = getConfig()->report->post_heater_C;
      if (postHeaterTemp < MachineConfig::STOP_TEMPERATURE) {
        OxCore::Debug<const char *>("Stop temperature reached!\n");
        OxCore::Debug<const char *>("=======================\n");
        getConfig()->_updateFanPWM(0.0);
        new_ms = Off;
        return new_ms;
      } else {
        unsigned long ms = millis();
        const unsigned long MINUTES_RAMPING_DN = (ms - begin_down_time) / (60 * 1000);
        // WARNING! FIX THIS
        // This is dangerous. If you cooldown before acheiving
        // the HOLD_TEMPERATURE, this can actually cause you
        // to rapidly warm up!
        getConfig()->TARGET_TEMP =
          getConfig()->COOL_DOWN_BEGIN_TEMPERATURE + MINUTES_RAMPING_DN * MachineConfig::RAMP_DN_TARGET_D_MIN;
        getConfig()->TARGET_TEMP = max(getConfig()->TARGET_TEMP,MachineConfig::STOP_TEMPERATURE);
        // We use full power fan for normal cooldown at present, since
        // we are actively using the heaters to come down slowly
        getConfig()->_updateFanPWM(1.0);
        _updatePowerComponentsVoltage(0.0);
        _updateStackVoltage(0.0);
      }

    }

    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(0.0);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
    _updatePowerComponentsVoltage(0);
    MachineState new_ms = OffUserAck;
    _updateStackVoltage(0.0);
    // In an emergency shutdown, we do NOT run the fan!
    getConfig()->_updateFanPWM(0.0);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(0.0);
    return new_ms;
  }
  // This is use primarily for maximum power at warmup or
  // maximum cooldown when we don't have to compute based on temperature
   void CogTask::_updatePowerComponentsVoltage(float voltage) {
        for (int i = 0; i < MachineConfig::NUM_HEATERS; i++) {
          //        _heaters[i].update(voltage);
          // We have a two-channel AC heater, we will use both
          // for now
          //          getConfig()->hal->_ac_heaters[i]->setHeater(0,(voltage > 0.0));
          //          getConfig()->hal->_ac_heaters[i]->setHeater(1,(voltage > 0.0));
          //        getConfig()->report->heater_voltage = voltage;
        }
    }

  // In theory, this could take an array rather than
  // a fixed voltage; that will have to be added later
   void CogTask::_updateStackVoltage(float voltage) {
        for (int i = 0; i < NUM_STACKS; i++) {
          _stacks[i]->updateVoltage(voltage,getConfig());
        }
    }

   void CogTask::_updateStackAmperage(float amperage) {
        for (int i = 0; i < NUM_STACKS; i++) {
          _stacks[i]->updateAmperage(amperage,getConfig());
        }
    }

#ifdef RIBBONFISH
   MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
     MachineState new_ms = NormalOperation;

    if (getConfig()->report->post_heater_C > MachineConfig::HIGH_TEMPERATURE_FAN_SLOW_DOWN_LIMIT) {
      getConfig()->_updateFanPWM(MachineConfig::HIGH_TEMPERATURE_FAN_PWM);
    } else {
      getConfig()->_updateFanPWM(1.0);
    }

     float desired_C = getConfig()->MAX_POST_HEATER_C;

     for (int i = 0; i < NUM_STACKS; i++) {
       float temperature = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[i]);
       float current_C = temperature;
       getConfig()->report->post_stack_C = current_C;
       float max_post_stack_C = getConfig()->MAX_POST_STACK_C;
       // TODO: In actuality, we need something more controllable
       // than MAX on and zero off!
       float voltage = (current_C >= max_post_stack_C) ? 0.0 : getConfig()->MAXIMUM_STACK_VOLTAGE;
       // This is a little complicated, because we may actually
       // want a tricle charge---cut for now, if we are in Idle
       // mode, we just set the stack voltage to zero
       if (i_or_o == Idle) {
         voltage = 1.0;
       }
       _updateStackVoltage(voltage);
       getConfig()->report->stack_voltage = voltage;

       Serial.print(" YYY  v : ");
       float v = getConfig()->TARGET_STACK_CURRENT_mA/1000.0;
       Serial.println(v);
       _updateStackAmperage(v);
     }

     return new_ms;
    }
#else
  // This is a mock, simulated update
    MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
      MachineState new_ms = NormalOperation;
        OxCore::Debug<const char *>("_updateHeaterPrims\n");
        // For now, i want to do only the first heater to simplify
        for (int i = 0; i < MachineConfig::NUM_HEATERS && i < 1; i++) {
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
    _updateStackVoltage(getConfig()->MAXIMUM_STACK_VOLTAGE);
    _updateStackAmperage(getConfig()->TARGET_STACK_CURRENT_mA/1000.0);
    OxCore::Debug<const char *>("Checking Voltage Set AAA ");
        OxCore::DebugLn<float>(_heaters[0]._voltage);
        return new_ms;
    }
#endif
}
