/*
 Copyright (C) 2022 Robert L. Read

 state_machine_manager_task -- An abstract superclass for state machines

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


#ifndef STATE_MACHINE_MANAGER_H
#define STATE_MACHINE_MANAGER_H

#include <machine_core_defs.h>
#include <core.h>
#include <heater_pid_task.h>
#include <log_recorder_task.h>
// #include <temp_refresh_task.h>   


namespace OxApp
{

  class StateMachineManager : public OxCore::Task {
  public:
    int DEBUG_LEVEL = 0;
    bool USE_ONE_BUTTON = false;
    HeaterPIDTask* heaterPIDTask;
    Log_Recorder_Task* logRecorderTask;

    MachineState _executeBasedOnState(MachineState ms);
    virtual MachineState _updatePowerComponentsOperation(IdleOrOperateSubState i_or_o);
    virtual MachineState _updatePowerComponentsOff() = 0 ;
    virtual MachineState _updatePowerComponentsWarmup() = 0;
    virtual MachineState _updatePowerComponentsIdle() = 0;
    virtual MachineState _updatePowerComponentsCooldown() = 0;
    virtual MachineState _updatePowerComponentsCritialFault() = 0;
    virtual MachineState _updatePowerComponentsEmergencyShutdown() = 0;
    virtual MachineState _updatePowerComponentsOffUserAck() = 0;
    virtual float getTemperatureReadingA_C() = 0;
    virtual float getTemperatureReadingB_C() = 0;
    virtual float getTemperatureReadingC_C() = 0;
    virtual void runOneButtonAlgorithm() = 0;
    bool run_generic();

    MachineState checkCriticalFaults(MachineState ms);
    void transitionToWarmup(float tt);
    void transitionToCooldown(float tt);
    void changeTargetTemp(float t);
    virtual void turnOff();
    virtual void printGenericInstructions();
    virtual void printOffWarnings(MachineState ms);

    // These code in theory be made static
    //      float  computeFanSpeed(float t);
    float  computeRampUpSetpointTemp(float t,float recent_t,unsigned long begin_up_time_ms);
    float  computeRampDnSetpointTemp(float t,float recent_t,unsigned long begin_dn_time_ms);

    bool _run() override;
  };
}

#endif
