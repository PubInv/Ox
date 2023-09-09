/*

  read_temps_task.cpp -- read temperatures sensors

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

#include <read_temps_task.h>


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

void ReadTempsTask::addTempToQueue(float c) {
  this->temps[next_temp_idx] = c;
  this->next_temp_idx = (next_temp_idx + 1) % NUM_TEMPS_TO_RECORD;
}
int ReadTempsTask::ringComputation(int n) {
  if (n >0)
    return n % NUM_TEMPS_TO_RECORD;
  else
    return (n+NUM_TEMPS_TO_RECORD) % NUM_TEMPS_TO_RECORD;
}
void ReadTempsTask::dumpQueue() {
  OxCore::DebugLn<const char *>("All Temps, going backward in ms:");
  for(int i = 0; i < NUM_TEMPS_TO_RECORD; i++) {
    OxCore::Debug<int>(i*1000);
    OxCore::Debug<const char *>(" : ");
    OxCore::DebugLn<float>(this->temps[ringComputation(this->next_temp_idx - i)]);
  }
}

// compute an average temperature backward in time t
float ReadTempsTask::tempFromTime(int t_ms) {
  float temp = 0.0;
  const int num_periods_back = (t_ms / MachineConfig::TEMP_READ_PERIOD_MS);
  int num_valid = 0;
  for(int i = 0; i < NUMBER_OF_PERIODS_TO_AVERAGE; i++) {
    float t = temps[ringComputation(next_temp_idx - (num_periods_back + i))];
    if (t != 0) {
      temp += t;
      num_valid++;
    }
  }
  if (num_valid > 0)
    return (temp / (float) num_valid);
  else
    return 0.0;
}

// this function is based on parameters
void ReadTempsTask::calculateDdelta() {
  float t0 = tempFromTime(0);
  float t1 = tempFromTime(TEMPERATRUE_TIME_DELTA_MS);
  // If our data is not complete yet, we will wait.
  if ((t0 == 0) || (t1 == 0))
    return;
  float DdeltaRaw = t0 - t1;
  // now compute Ddelta_C_per_min...
  float Ddelta_C_per_min_computed = (DdeltaRaw * TEMPERATRUE_TIME_DELTA_MS) / (60.0*1000.0);
  // now we will set the global so that it is available to the other tasks
  Ddelta_C_per_min = Ddelta_C_per_min_computed;
}

void ReadTempsTask::updateTemperatures() {
  _readTemperatureSensors();

  // These are added just to test if reading quickly causes an error,
  // which might induce us to add power to the Dallas One-Wire board, for example.
  float postHeaterTemp = _temperatureSensors[0].GetTemperature(0);
  float postStackTemp = _temperatureSensors[0].GetTemperature(1);
  float postGetterTemp = _temperatureSensors[0].GetTemperature(2);
  // Sometimes we get a data read error, that comes across
  // as -127.00. In that case, we will leave the
  // value unchanged from the last read.
  if (postHeaterTemp > -100.0) {
    getConfig()->report->post_heater_C = postHeaterTemp;
    good_temp_reads++;
  } else {
    OxCore::Debug<const char *>("Bad post_heater_C\n");
    bad_temp_reads++;
  }
  if (postGetterTemp > -100.0) {
    getConfig()->report->post_getter_C = postGetterTemp;
    good_temp_reads++;
  } else {
    OxCore::Debug<const char *>("Bad post_getter_C\n");
    bad_temp_reads++;
  }
  if (postStackTemp > -100.0) {
    getConfig()->report->post_stack_C = postStackTemp;
    good_temp_reads++;
  } else {
    OxCore::Debug<const char *>("Bad post_stack_C\n");
    bad_temp_reads++;
  }

  // WARNING! This needs to be done for all configs if we are
  // a 2-stage heater; this is handled by the subclass.
  getConfig()->report->target_temp_C = getConfig()->TARGET_TEMP;

  if (DEBUG_READ_TEMPS > 0) {
    getConfig()->outputReport(getConfig()->report);
    OxCore::Debug<const char *>("Target Temp : ");
    OxCore::DebugLn<float>(getConfig()->TARGET_TEMP);
  }
  // Notice we are keeping the queue only for the post_heater thermocouple,
  // which is what we are using as a control variable.
  addTempToQueue(getConfig()->report->post_heater_C);
  calculateDdelta();
  if (DEBUG_READ_TEMPS > 0) {
    OxCore::Debug<const char *>("Good Temp Reads:");
    OxCore::Debug<unsigned long>(good_temp_reads);
    OxCore::DebugLn<const char *>("");
    OxCore::Debug<const char *>("Bad  Temp Reads:");
    OxCore::Debug<unsigned long>(bad_temp_reads);
    OxCore::DebugLn<const char *>("");
  }
}

void stage2_ReadTempsTask::updateTemperatures() {
  ReadTempsTask::updateTemperatures();
  mcs[0]->report->post_heater_C = getConfig()->report->post_heater_C;
  mcs[1]->report->post_getter_C = getConfig()->report->post_getter_C;
  mcs[2]->report->post_stack_C = getConfig()->report->post_stack_C;
  // The TARGET_TEMP is not computed here, this is just a reporting function!
  for(int i = 0; i < 3; i++) {
    mcs[i]->report->target_temp_C = mcs[i]->TARGET_TEMP;
  }

}
// I don't fully understand this!
void ReadTempsTask::_configTemperatureSensors() {
  _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MAX31850Temperature[1];
  _temperatureSensors[0]._config = config[0];
}

void ReadTempsTask::_readTemperatureSensors() {
  for (int i = 0; i < NUM_TEMP_INDICES; i++) {
    _temperatureSensors[i].ReadTemperature();
    float temperature = _temperatureSensors[0].GetTemperature(i);
    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("Temp : ");
      OxCore::Debug<const char *>(getConfig()->TempLocationNames[i]);
      OxCore::Debug<const char *>(": ");
      OxCore::DebugLn<float>(temperature);
    }
  }
  if (DEBUG_READ_TEMPS > 1) {
    dumpQueue();
  }
}

bool ReadTempsTask::_init()
{
  OxCore::Debug<const char *>("ReadTempsTask init\n");
  _configTemperatureSensors();
  OxCore::Debug<const char *>("Config of temperature sensors done\n");
  for (int i = 0; i < NUM_TEMP_INDICES; i++) {
    temps[i] = 0.0;
  }
  return true;
}

bool ReadTempsTask::_run()
{
  if (DEBUG_READ_TEMPS > 1) {
    OxCore::Debug<const char *>("Running ReadTemps\n");
  }
  updateTemperatures();
}

ReadTempsTask::ReadTempsTask() {
}
