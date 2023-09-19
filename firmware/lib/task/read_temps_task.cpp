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

float ReadTempsTask::evaluateThermocoupleRead(int idx,CriticalErrorCondition ec,int &rv) {
  float temp = _temperatureSensors[0].GetTemperature(idx);

#ifndef ALLOW_BAD_THERMOCOUPLES_FOR_TESTING
#ifdef USE_MAX31850_THERMOCOUPLES
  // we'd like to use the corret sentinels, but they don't seem to work...
  //  if (temp == DEVICE_DISCONNECTED_C) {
  if (temp < 0.0) {
    if (!getConfig()->errors[ec].fault_present) {
      getConfig()->errors[ec].fault_present = true;
      getConfig()->errors[ec].begin_condition_ms = millis();
      Serial.println("THERMOCOUPLE DISCONNECTED!!!!!!!!!");
    }
  } else {
    getConfig()->errors[ec].fault_present = false;
  }

  return temp;
#else
  // probably the SPI based MAX31855_THERMOCOUPLES
#endif
#endif
}

void ReadTempsTask::updateTemperatures() {

    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("About to _readTemperatureSensors");
      delay(30);
    }

  _readTemperatureSensors();

    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("Done with _readTemperatureSensors");
      delay(30);
    }

  // These are added just to test if reading quickly causes an error,
  // which might induce us to add power to the Dallas One-Wire board, for example.
    //  float postHeaterTemp = _temperatureSensors[0].GetTemperature(0);
  // Sometimes we get a data read error, that comes across
  // as -127.00. In that case, we will leave the
  // value unchanged from the last read.
  int post_rv;
  float postHeaterTemp = evaluateThermocoupleRead(0,POST_HEATER_TC_BAD,post_rv);
  // The sentinel values are all less than this, so in addtion
  // to critical errors, we will leave this.
  if (postHeaterTemp > -100.0) {
    getConfig()->report->post_heater_C = postHeaterTemp;
//    good_temp_reads++;
    good_temp_reads_heater++;
  } else {
    OxCore::Debug<const char *>("Bad post_heater_C\n");
    //bad_temp_reads++;
    bad_temp_reads_heater++;
  }

  float postGetterTemp = evaluateThermocoupleRead(2,POST_GETTER_TC_BAD,post_rv);
  //_temperatureSensors[0].GetTemperature(2);
  if (postGetterTemp > -100.0) {
    getConfig()->report->post_getter_C = postGetterTemp;
//    good_temp_reads++;
    good_temp_reads_getter++;
  } else {
    OxCore::Debug<const char *>("Bad post_getter_C\n");
    //bad_temp_reads++;
    bad_temp_reads_getter++;
  }

  float postStackTemp = evaluateThermocoupleRead(1,POST_STACK_TC_BAD,post_rv);
  // _temperatureSensors[0].GetTemperature(1);
  if (postStackTemp > -100.0) {
    getConfig()->report->post_stack_C = postStackTemp;
//    good_temp_reads++;
    good_temp_reads_stack++;
  } else {
    OxCore::Debug<const char *>("Bad post_stack_C\n");
    //bad_temp_reads++;
    bad_temp_reads_stack++;
  }

  // WARNING! This needs to be done for all configs if we are
  // a 2-stage heater; this is handled by the subclass.
  getConfig()->report->target_temp_C = getConfig()->TARGET_TEMP_C;
  getConfig()->report->setpoint_temp_C = getConfig()->SETPOINT_TEMP_C;
  getConfig()->report->target_ramp_C = getConfig()->RAMP_UP_TARGET_D_MIN;

  if (DEBUG_READ_TEMPS > 0) {
    getConfig()->outputReport(getConfig()->report);
  }
  // Notice we are keeping the queue only for the post_heater thermocouple,
  // which is what we are using as a control variable.
  addTempToQueue(getConfig()->report->post_heater_C);
  calculateDdelta();
  if (DEBUG_READ_TEMPS > 0) {
    OxCore::Debug<const char *>("Good Temp Reads:");
//    OxCore::Debug<unsigned long>(good_temp_reads);
    OxCore::Debug<unsigned long>(good_temp_reads_heater);
    OxCore::Debug<const char *>(", ");
OxCore::Debug<unsigned long>(good_temp_reads_getter);
    OxCore::Debug<const char *>(", ");
OxCore::Debug<unsigned long>(good_temp_reads_stack);
    OxCore::DebugLn<const char *>("");


    OxCore::Debug<const char *>("Bad  Temp Reads:");
//    OxCore::Debug<unsigned long>(bad_temp_reads);
    OxCore::Debug<unsigned long>(bad_temp_reads_heater);
    OxCore::Debug<const char *>(", ");
    OxCore::Debug<unsigned long>(bad_temp_reads_getter);
    OxCore::Debug<const char *>(", ");
    OxCore::Debug<unsigned long>(bad_temp_reads_stack);
    OxCore::DebugLn<const char *>("");
  }
}

void stage2_ReadTempsTask::updateTemperatures() {

  Serial.println("YYYYYYYYYYYYYYY");
  ReadTempsTask::updateTemperatures();

  // note: This is confugsing; we are naming the temperatures
  // wrongly on a Stage2 system, and then putting into individual
  // machine_configs.
  mcs[0]->report->post_heater_C = getConfig()->report->post_heater_C;
  mcs[1]->report->post_heater_C = getConfig()->report->post_getter_C;
  mcs[2]->report->post_heater_C = getConfig()->report->post_stack_C;
  // The TARGET_TEMP is not computed here, this is just a reporting function!
  for(int i = 0; i < 3; i++) {
    Serial.println((unsigned long) mcs[i]);
    Serial.println((unsigned long) mcs[i]->report);

    mcs[i]->report->target_temp_C = mcs[i]->TARGET_TEMP_C;
    mcs[i]->report->target_ramp_C = mcs[i]->RAMP_UP_TARGET_D_MIN;
    mcs[i]->report->setpoint_temp_C = mcs[i]->SETPOINT_TEMP_C;
    mcs[i]->report->ms = mcs[i]->ms;
  }

}
void ReadTempsTask::_configTemperatureSensors() {

#ifdef USE_MAX31850_THERMOCOUPLES
  _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MAX31850Temperature[1];
#elif USE_MAX31855_THERMOCOUPLES
  _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MAX31855Temperature[1];
#else
  Serial.println("MAJOR INTERNAL ERROR, THERMOCOUPLE PREPROCESSOR DIRECTIVES NOT DEFINED!");
#endif

  _temperatureSensors[0]._config = config[0];
  if (DEBUG_READ_TEMPS > 0) {
    OxCore::Debug<const char *>("Read Temp Configuration done!");
    delay(50);
  }
}

void ReadTempsTask::_readTemperatureSensors() {
  for (int i = 0; i < NUM_TEMP_INDICES; i++) {
    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("QQQ\n");
      delay(50);
    }
    _temperatureSensors[i].ReadTemperature();
    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("BBB\n");
      delay(50);
    }
    float temperature = _temperatureSensors[0].GetTemperature(i);
    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("Temp : ");
      OxCore::Debug<const char *>(getConfig()->TempLocationNames[i]);
      OxCore::Debug<const char *>(": ");
      OxCore::DebugLn<float>(temperature);
    }
    // TODO: We should investigate a delay hear to make sure the
    // OneWire system is ready
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


bool stage2_ReadTempsTask::_run()
{
  if (DEBUG_READ_TEMPS > 1) {
    OxCore::Debug<const char *>("Running ReadTemps\n");
  }
  updateTemperatures();
}

ReadTempsTask::ReadTempsTask() {
}
