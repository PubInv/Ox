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
#include "abstract_ps.h"
#include <machine_core_defs.h>
#include <machine.h>
#include <cog_hal.h>

#include <abstract_temperature.h>

#include <OnePinHeater.h>
#include <MAX31850.h>

#include <heater_pid_task.h>
#include <state_machine_manager.h>
#include <PID_v1.h>
#include <wattage_pid_object.h>
#include <duty_cycle_task.h>

namespace OxApp
{
  class CogTask : public StateMachineManager
  {
  public:
    int PERIOD_MS = 10000;
    int DEBUG_LEVEL = 0;

    DutyCycleTask *dutyCycleTask;
    WattagePIDObject *wattagePIDObject;

    // There are really several sensors, but they are indexed!
    const static int NUM_TEMP_SENSORS = 3;
    const static int NUM_TEMP_INDICES = 2;
    const static int NUM_FANS = 1;

    float getTemperatureReadingA_C();
    float getTemperatureReadingB_C();
    float getTemperatureReadingC_C();
    bool updatePowerMonitor();

    // "OneButton Routines"
    float computeHeaterDutyCycleFromWattage(float heaterWattage_w);
    float computeTotalWattage(float controlTemp);
    float computeTargetStackWattage(float targetTotalWattage, float heaterWatts, float currentTemp, float B, float C, float targetStackWatts);
    float computeFanSpeedTargetFromSchedule(float temp);
    float computeFanSpeedTarget(float currentTargetTemp,float temp, float heaterWatts);
    bool heaterWattsAtFullPowerPred(float watts);
    void oneButtonAlgorithm(float &totalWattage_w,float &stackWattage_w,float &heaterWattage_w,float &fanSpeed_p);
    void runOneButtonAlgorithm();

    // The PID controller for OneButton Routine
    PID *pidControllerWattage;
    double totalWattage_Output_W = 0.0;
    double final_totalWattage_W = 0.0;
    double temperatureSetPoint_C = 25.0;
    double input_temperature_C = 25.0;



    COG_HAL* getHAL();

    void turnOff() override;
    void printGenericInstructions() override;

    float getFanSpeed(float t);
    float computeAmperage(float t);

    void _updateCOGSpecificComponents();
    void _updatePowerComponentsVoltage(float voltage);
    void _configTemperatureSensors();

    void _updateFanSpeed(float percentage);
    void _updateStackVoltage(float voltage);
    void _updateStackAmperage(float amperage);
    void _updateStackWattage(float wattage);

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
