/*
 Copyright (C) 2022 Robert L. Read

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

#ifndef COG_TASK_H
#define COG_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "heater.h"
#include "stack.h"
#include "abstract_ps.h"
// #include "mostplus_flow.h"
#include <machine_core_defs.h>
#include <machine.h>
#include <cog_hal.h>

#include <abstract_temperature.h>

#include <OnePinHeater.h>
#include <MAX31850.h>

#include <temp_refresh_task.h>
#include <heater_pid_task.h>
#include <state_machine_manager.h>


namespace OxApp
{
    class CogTask : public StateMachineManager
    {
    public:
      int PERIOD_MS = 10000;
      int DEBUG_LEVEL = 0;


      // TODO: This should probably be done dynamically, not here...

      // There are really several senosrs, but they are indexed!
      const static int NUM_TEMP_SENSORS = 3;
      const static int NUM_TEMP_INDICES = 2;
      const static int NUM_FANS = 1;

     float getTemperatureReading();
     COG_HAL* getHAL();


      void _updatePowerComponentsVoltage(float voltage);
      void _configTemperatureSensors();

      void _updateFanSpeed(float percentage);
      void _updateStackVoltage(float voltage);
      void _updateStackAmperage(float amperage);


       MachineState _updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) override;
       MachineState _updatePowerComponentsOff() override;
       MachineState _updatePowerComponentsWarmup() override;
       MachineState _updatePowerComponentsIdle() override;
       MachineState _updatePowerComponentsCooldown() override;
       MachineState _updatePowerComponentsCritialFault() override;
       MachineState _updatePowerComponentsEmergencyShutdown() override;
       MachineState _updatePowerComponentsOffUserAck() override;

    private:
      bool _run() override;
      bool _init() override;

    };


}

#endif
