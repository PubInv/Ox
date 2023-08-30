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

#ifndef STAGE2_HEATER_H
#define STAGE2_HEATER_H

#include <cog_task.h>

namespace OxApp
{
    class Stage2HeaterTask : public StateMachineManager
    {
    public:
      TempRefreshTask* tempRefreshTask;
      // Thee Stage2 problem controls 3 heaters.
      HeaterPIDTask* heaterPIDTask[3];
      int DEBUG_LEVEL = 0;

      const static int NUM_TEMPERATURE_SENSORS = 3;
      const static int NUM_TEMPERATURE_INDICES = 2;
      const static int NUM_FANS = 1;
      const static int NUM_STACKS = 1;

      unsigned long begin_down_time = 0;

      void _updatePowerComponentsVoltage(float voltage);
      void _configTemperatureSensors();


       MachineState _updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) override;
       MachineState _updatePowerComponentsOff() override;
       MachineState _updatePowerComponentsWarmup() override;
       MachineState _updatePowerComponentsIdle() override;
       MachineState _updatePowerComponentsCooldown() override;
       MachineState _updatePowerComponentsCritialFault() override;
       MachineState _updatePowerComponentsEmergencyShutdown() override;
       MachineState _updatePowerComponentsOffUserAck() override;

    private:
      bool _init() override;
      bool _run() override;

    };
}

#endif
