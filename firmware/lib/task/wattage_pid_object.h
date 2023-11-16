/*

  wattage_pid_object.h -- control the overall wattage

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

#ifndef WATTAGE_PID_OBJECT_H
#define WATTAGE_PID_OBJECT_H

#include <core_defines.h>
#include <core.h>
#include <PID_v1.h>

  class WattagePIDObject
  {
  public:
    WattagePIDObject();
    PID *pidControllerWattage;
    int DEBUG_PID = 0;

    double totalWattage_Output_W = 0.0;
    double final_totalWattage_W = 0.0;
    double temperatureSetPoint_C = 25.0;
    double input_temperature_C = 25.0;

    // This function sets the above parameters and
    // also sets them into the PID loop.
    void SetTunings(double, double, double);
    void printTunings();

    double GetKp();
    double GetKi();
    double GetKd();
    void shutHeaterDown();
    float compute(float controlTemp);

  private:
    double FKp = 10.0;
    double FKi = 1.0;
    double FKd = 0.5;

  };

#endif
