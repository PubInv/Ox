/*

  read_temps_task.h -- read temperatures sensors

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

#ifndef READ_TEMP_TASK_H
#define READ_TEMP_TASK_H

#include <core_defines.h>
#include <core.h>

#ifdef USE_MAX31850_THERMOCOUPLES
#include <MAX31850.h>
#else
#include <DS18B20_temperature.h>
#endif

using namespace OxCore;

class ReadTempsTask : public OxCore::Task
{
public:
  ReadTempsTask();
  int DEBUG_READ_TEMPS = 0;
  static const int PERIOD_MS = MachineConfig::TEMP_READ_PERIOD_MS;
  // This is a ring buffer...

  // Ddelta is the change in temperature in C per min
  float Ddelta_C_per_min = 0.0;
// This is the number of periods around a point in time we will
// average to produce a smooth temperature. (Our thermocouples have
// only 0.25 C resolution, which is low for a 0.5C/minute control
// situation!) These are always taken to be BACKWARD in time.
  static const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
  static const int NUM_TEMPS = NUMBER_OF_PERIODS_TO_AVERAGE;
  static const int TEMPERATRUE_TIME_DELTA_MS = 60000;
  static constexpr float MAXIMUM_CHANGE_IN_DUTY_CYCLE_PER_MIN = 1.0 / 100.0;

  static constexpr int NUM_TEMPS_TO_RECORD = ceil((((float) TEMPERATRUE_TIME_DELTA_MS / (float) MachineConfig::TEMP_READ_PERIOD_MS) + NUMBER_OF_PERIODS_TO_AVERAGE));

  float temps[NUM_TEMPS_TO_RECORD];

  // This is period of time we will use to compute the Ddela_C_per_min.
  // Note this does not have to be related to the DUTY_CYCLE_ADJUSTMENT_PERIOD_MS

  // we will add one to this
  int next_temp_idx = 0;
  const static int NUM_TEMP_SENSORS = 3;
  const static int NUM_TEMP_INDICES = 1;
  Temperature::AbstractTemperature* _temperatureSensors;

  void _readTemperatureSensors();
  void _configTemperatureSensors();
  void updateTemperatures();
  void addTempToQueue(float c);
  float tempFromTime(int t_ms);
  void calculateDdelta();
  void dumpQueue();
  int ringCompuation(int n);
private:
  bool _init() override;
  bool _run() override;
};


#endif
