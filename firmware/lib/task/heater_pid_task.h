/*

  heater_pid_task.h -- control an AC heater with an SSR

  Copyright (C) 2023 Robert Read.

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

#ifndef HEATER_PID_TASK_H
#define HEATER_PID_TASK_H

#include <core_defines.h>
#include <core.h>
#include <PID_v1.h>
#include <duty_cycle_task.h>

// Runs the Pressure Swing Adsorption cycle
  class HeaterPIDTask : public OxCore::Task
  {
  public:
    HeaterPIDTask();
    Stage2Heater whichHeater;
    PID *pidControllerHeater;
    int DEBUG_PID = 0;
    // If you change this, you probably want to change the
    // PID constants, otherwise you are changing your gain.
    // int PERIOD_MS = 60*1000;
    int PERIOD_MS = 5*1000;

    // These are on a scale of 1.0;
    double dutyCycle_Output = 0.0;
    double final_dutyCycle = 0.0;
    double HeaterSetPoint_C = 25.0;
    double Input_temperature_C = 25.0;
    DutyCycleTask *dutyCycleTask;

    // This function sets the above parameters and
    // also sets them into the PID loop.
    void SetTunings(double, double, double);
    double GetKp();
    double GetKi();
    double GetKd();
    void shutHeaterDown();
  private:
    // These are initial default values...
    // other code may change these by calling SetTunings
    double FKp = 0.005;
    // I want zero overshoot, so I am setting this to zero!
    double FKi = 0.0;
    double FKd = 0.0;

    bool _init() override;
    bool _run() override;
    // This would go into the abstract class.
  };

#endif
