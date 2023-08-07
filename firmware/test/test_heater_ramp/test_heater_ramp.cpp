/*
  test_heater_ramp -- A program to test the the ability to ramp up, hold, and ramp down.

  Copyright 2023, Robert L. Read

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

#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <debug.h>
#include <GGLabsSSR1.h>

using namespace OxCore;


#include <core_defines.h>
#include <core.h>
#include <machine.h>
#include <SanyoAceB97.h>
#include <Arduino.h>

// We aren't using this, but it brings in declarations we need!
#include <cog_task.h>

using namespace OxCore;
static Core core;

const unsigned long REPORT_PERIOD_MS = 3000;

MachineConfig *machineConfig;
TaskProperties _properties;

void report(MachineConfig *machineConfig) {
  Serial.print("F : ");
  Serial.println(machineConfig->report.fan_speed);
}

unsigned long time_of_last_report = 0;

const float FAN_PER_CENT = 60.0;
const int FAN_PWM = (int) (255.0*60.0/100.0);

// TEST CONFIGURATION PARAMETERS
// ALL OF THESE COULD BE CONFIGURABLE, BUT FOR THIS TEST
// THEY ARE "HARD_WIRED" HERE.
// Edit these directly and re-upload to run a different test.
// This test is designed to abort the processeor when done.

const float RAMP_UP_TARGET_D_MIN = 0.5; // degrees C per minute
const float RAMP_DN_TARGET_D_MIN = 0.5; // degrees C per minute
const int HOLD_TEMPERATURE = 30;
const unsigned long HOLD_TIME_MINUTES = 1;
const unsigned long HOLD_TIME_SECONDS = 60 * HOLD_TIME_MINUTES;
const float STARTING_DUTY_CYCLE_FRACTION = 0.0;

// These parameters are related to our control procedure.
// This is similar to a PID loop, but I don't think any integration
// is needed, and want to have a test that doesn't depend on a PID
// loop. Arguably, that is just implementing a very simple PD loop
// myself; I'll accept that rap.

// Let DutyCycle be changing Duty cycle, a fraction of 1.0.
// In PID termonology, this is the "PLANT VARIABLE"--what we can control.
float DutyCycle = STARTING_DUTY_CYCLE_FRACTION;
// Temperature Read Period is how often we will update the
// Temperature sensor.
const int TEMPERATURE_READ_PERIOD_MS = 1000;
// Duty Cycle Adjustment Period is how often we will adject
const int DUTY_CYCLE_ADJUSTMENT_PERIOD_MS = 30000;
// This is the number of periods around a point in time we will
// average to produce a smooth temperature. (Our thermocouples have
// only 0.25 C resolution, which is low for a 0.5C/minute control
// situation!) These are always taken to be BACKWARD in time.
const int NUMBER_OF_PERIODS_TO_AVERAGE = 4;
// Ddelta is the change in temperature in C per min
float Ddelta_C_per_min = 0.0;
// This is period of time we will use to compute the Ddela_C_per_min.
// Note this does not have to be related to the DUTY_CYCLE_ADJUSTMENT_PERIOD_MS
const int TEMPERATRUE_TIME_DELTA_MS = 60000;
//
const float MAXIMUM_CHANGE_IN_DUTY_CYCLE_PER_MIN = 1.0 / 100.0;

const int NUM_TEMPS_TO_RECORD = ceil((((float) TEMPERATRUE_TIME_DELTA_MS / (float) TEMPERATURE_READ_PERIOD_MS) + NUMBER_OF_PERIODS_TO_AVERAGE));


const long FAKE_NUMBER_OF_DUTY_CYCLES_TO_RUN = 100000;

int num_duty_cycles = 0;

const int DUTY_CYCLE_COMPUTATION_TIME_MS = 30*1000;


// Our AC heater...confusingly named!
const int NUM_HEATERS = 1;

GGLabsSSR1 _ac_heaters[NUM_HEATERS];

const int DEBUG_LEVEL = 1;
using namespace std;

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

  // Once a second we will read the temperature and keep
  // a running record here (back in time) so that we can
  // set the duty cycle based on change.
  class ReadTempsTask : public OxCore::Task
  {
  public:
    const int DEBUG_READ_TEMPS = 1;
    const int PERIOD_MS = TEMPERATURE_READ_PERIOD_MS;
    const int NUM_TEMPS = NUMBER_OF_PERIODS_TO_AVERAGE;
    // This is a ring buffer...
    float temps[NUM_TEMPS_TO_RECORD];
    // we will add one to this
    int next_temp_idx = 0;
    const static int NUM_TEMPERATURE_SENSORS = 3;
    const static int NUM_TEMPERATURE_INDICES = 1;
    Temperature::AbstractTemperature* _temperatureSensors;

    void _readTemperatureSensors();
    void _configTemperatureSensors();
    void updateTemperatures();
    void addTempToQueue(double c);
    float tempFromTime(int t_ms);
    void calculateDdelta();
    void dumpQueue();
    int ringCompuation(int n);
  private:
    bool _init() override;
    bool _run() override;
  };

  void ReadTempsTask::addTempToQueue(double c) {
    this->temps[next_temp_idx] = c;
    this->next_temp_idx = (next_temp_idx + 1) % NUM_TEMPS_TO_RECORD;
  }
  int ReadTempsTask::ringCompuation(int n) {
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
      OxCore::DebugLn<float>(this->temps[ringCompuation(this->next_temp_idx - i)]);
    }
  }

  // compute an average temperature backward in time t
  float ReadTempsTask::tempFromTime(int t_ms) {
    float temp = 0.0;
    const int num_periods_back = (t_ms / TEMPERATURE_READ_PERIOD_MS);
    int num_valid = 0;
    for(int i = 0; i < NUMBER_OF_PERIODS_TO_AVERAGE; i++) {
      float t = temps[ringCompuation(next_temp_idx - (num_periods_back + i))];
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
    Serial.println("computing delta");
    Serial.println(t0,5);

    Serial.println(t1,5);
    float DdeltaRaw = t0 - t1;
    // now compute Ddelta_C_per_min...
    float Ddelta_C_per_min_computed = (DdeltaRaw * TEMPERATRUE_TIME_DELTA_MS) / (60.0*1000.0);
    // now we will set the global so that it is available to the other tasks
    Ddelta_C_per_min = Ddelta_C_per_min_computed;
  }

  void ReadTempsTask::updateTemperatures() {
    _readTemperatureSensors();
    double postHeaterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_heater_indices[0]);
    double postStackTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_stack_indices[0]);
    double postGetterTemp = _temperatureSensors[0].GetTemperature(getConfig()->post_getter_indices[0]);
    getConfig()->report.post_heater_C = postHeaterTemp;
    getConfig()->report.post_stack_C = postStackTemp;
    getConfig()->report.post_getter_C = postGetterTemp;
     addTempToQueue(getConfig()->report.post_heater_C);
     calculateDdelta();
  }
  // I don't fully understand this!
  void ReadTempsTask::_configTemperatureSensors() {
    _temperatureSensors = (Temperature::AbstractTemperature *) new Temperature::MAX31850Temperature[1];
    _temperatureSensors[0]._config = config[0];
  }

  void ReadTempsTask::_readTemperatureSensors() {
    for (int i = 0; i < NUM_TEMPERATURE_INDICES; i++) {
      _temperatureSensors[i].ReadTemperature();
      float temperature = _temperatureSensors[0].GetTemperature(i);
      if (DEBUG_READ_TEMPS > 0) {
        OxCore::Debug<const char *>("Temp : ");
        OxCore::Debug<const char *>(getConfig()->TempLocationNames[i]);
        OxCore::Debug<const char *>(": ");
        OxCore::DebugLn<float>(temperature);
      }
    }
    if (DEBUG_READ_TEMPS > 0) {
      OxCore::Debug<const char *>("Ddelta_C_per_min :");
      //      OxCore::DebugLn<float>(Ddelta_C_per_min);
      Serial.println(Ddelta_C_per_min,5);
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
        for (int i = 0; i < NUM_TEMPERATURE_INDICES; i++) {
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

  enum State {RampingUp, Holding, RampingDn};


  // This task controls the ramp up, hold, and ramp down.
  class SupervisorTask : public OxCore::Task
  {
  public:
    SupervisorTask();
    const int PERIOD_MS = 1000;
    State state = RampingUp;


    unsigned long begin_hold_time;
  private:
    bool _init() override;
    bool _run() override;
  };
  SupervisorTask::SupervisorTask() {
  }

  bool SupervisorTask::_init()
  {
    OxCore::Debug<const char *>("SupervisorTask init\n");
    return true;
  }

  bool SupervisorTask::_run()
  {
    switch (state) {
    case RampingUp: {
      break;
    };
    case Holding: {
      break;
    };
    case RampingDn: {
      break;
    };
    }
    return true;
  }


  // This task controls the ramp up, hold, and ramp down.
  class ControllerTask : public OxCore::Task
  {
  public:
    ControllerTask();
    const int DEBUG_CONTROLLER = 1;
    const int PERIOD_MS = DUTY_CYCLE_ADJUSTMENT_PERIOD_MS;
  private:
    bool _init() override;
    bool _run() override;
  };
  ControllerTask::ControllerTask() {
  }

  bool ControllerTask::_init()
  {
    OxCore::Debug<const char *>("ControllerTask init\n");
    return true;
  }

  // Here is where we do what would be thought of as the PID calculation
  bool ControllerTask::_run()
  {
    float deltaRate = RAMP_UP_TARGET_D_MIN - Ddelta_C_per_min;
    float clamped_deltaRate = constrain(deltaRate,
                                     -MAXIMUM_CHANGE_IN_DUTY_CYCLE_PER_MIN,
                                     MAXIMUM_CHANGE_IN_DUTY_CYCLE_PER_MIN);

    DutyCycle = DutyCycle + clamped_deltaRate;

    if (DEBUG_CONTROLLER > 0) {
      OxCore::Debug<const char *>("Duty Cycle:");
      OxCore::DebugLn<float>(DutyCycle);
      OxCore::DebugLn<const char *>("");
    }
    return true;
  }


  class DutyCycleTask : public OxCore::Task
  {
  public:
    DutyCycleTask();
    const int PERIOD_MS = 500;
    boolean isOn = false;
    float recorded_duty_cycle = 0;
    unsigned long recorded_dc_ms = 0;
    unsigned long time_of_last_check = 0;
  private:
    bool _init() override;
    bool _run() override;
  };
  DutyCycleTask::DutyCycleTask() {
  }

  bool DutyCycleTask::_init()
  {
    OxCore::Debug<const char *>("DutyCycleTask init\n");
    return true;
  }

  // The fundamental purpose of this task is just to
  // drive the stated duty cycle of the heater.
  //
  bool DutyCycleTask::_run()
  {
    // WARNING: This will fail when 2^32 ms are reached, about 28 days I think.
    unsigned long ms = millis();
    unsigned long delta_t = ms - time_of_last_check;
    // now we update the recorded duty_cycle weight
    float old_dc = recorded_duty_cycle;
    float old_ms = recorded_dc_ms;
    recorded_dc_ms += delta_t;
    recorded_duty_cycle = (old_dc * old_ms + ((isOn ? delta_t : 0))) / recorded_dc_ms;
    // now we decided to turn on or off!
    isOn = (recorded_duty_cycle < DutyCycle);
    // now we actually turn the heater on or off!
    for(int i = 0; i < NUM_HEATERS; i++) {
      _ac_heaters[i].setHeater(i,isOn);
    }
  }
}

using namespace OxApp;
DutyCycleTask dutyCycleTask;
ReadTempsTask readTempsTask;
SupervisorTask supervisorTask;
ControllerTask controllerTask;

void setup() {
  OxCore::serialBegin(115200UL);
  delay(500);

  if (core.Boot() == false) {
    ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    // TODO: Output error message
    //return EXIT_FAILURE;
    return;
  }

  machineConfig = new MachineConfig();
  machineConfig->hal = new MachineHAL();


  for(int i = 0; i < NUM_HEATERS; i++) {
    _ac_heaters[i].setHeater(0,LOW);
    _ac_heaters[i].setHeater(1,LOW);
  }

  bool initSuccess  = machineConfig->hal->init();
  Serial.println("about to start!");
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
  }

#ifdef RIBBONFISH
  // TODO: I am not sure where to put this; and it is
  // probably only valid on Due...nonetheless it is
  // absolutely needed byt the TF800A12K.cpp/SL_PS class...
  // it really should be there...
  pinMode(MAX31850_DATA_PIN, INPUT);
  pinMode(RF_FAN, OUTPUT);
  pinMode(RF_HEATER, OUTPUT);
  pinMode(RF_STACK, OUTPUT);
#endif

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.state_and_config = (void *) &machineConfig;
  delay(1000);
  Serial.println("About to run test!");
  delay(100);
  SanyoAceB97 *fan = new SanyoAceB97("FIRST_FAN",0,RF_FAN,1.0);
  Serial.println("Fan Set up");
  fan->_init();
  Serial.println("fan init done!");
  fan->DEBUG_FAN = 0;


  pinMode(fan->PWM_PIN[0], OUTPUT);
  // It is safer to be completely off until we are ready to start!
  analogWrite(fan->PWM_PIN[0],FAN_PWM);

  Serial.println("Fan Set up");
  delay(100);
  OxCore::TaskProperties readTempsProperties;
  Serial.println("A");
  readTempsProperties.name = "readTemps";
  Serial.println("B");
  readTempsProperties.id = 25;
  Serial.println("C");
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  Serial.println("D");
  readTempsProperties.priority = OxCore::TaskPriority::High;
  Serial.println("E");
  readTempsProperties.state_and_config = (void *) &machineConfig;
  Serial.println("About to add readTemps task");
  delay(300);
  core.AddTask(&readTempsTask, &readTempsProperties);
  Serial.println("Task Added");
  delay(100);

  // OxCore::TaskProperties supervisorProperties;
  // supervisorProperties.name = "supervisor";
  // supervisorProperties.id = 26;
  // supervisorProperties.period = supervisorTask.PERIOD_MS;
  // supervisorProperties.priority = OxCore::TaskPriority::High;
  // supervisorProperties.state_and_config = (void *) &machineConfig;
  // core.AddTask(&supervisorTask, &supervisorProperties);


  OxCore::TaskProperties controllerProperties;
  controllerProperties.name = "controller";
  controllerProperties.id = 27;
  controllerProperties.period = controllerTask.PERIOD_MS;
  controllerProperties.priority = OxCore::TaskPriority::High;
  controllerProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&controllerTask, &controllerProperties);


  OxCore::TaskProperties dutyCycleProperties;
  dutyCycleProperties.name = "dutyCycle";
  dutyCycleProperties.id = 28;
  dutyCycleProperties.period = dutyCycleTask.PERIOD_MS;
  dutyCycleProperties.priority = OxCore::TaskPriority::High;
  dutyCycleProperties.state_and_config = (void *) &machineConfig;
  core.AddTask(&dutyCycleTask, &dutyCycleProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  Serial.println("Setup Done!");
}



void loop() {
  OxCore::Debug<const char *>("Loop starting...\n");

  // Blocking call
  if (core.Run() == false) {
    OxCore::ErrorHandler::Log(OxCore::ErrorLevel::Critical, OxCore::ErrorCode::CoreFailedToRun);
#ifdef ARDUINO
    // Loop endlessly to stop the program from running
    OxCore::Debug<const char *>("Aborting! TEST OVER\n");
    delay(300);
    abort();
    // while (true) {}
#endif
    return;
  }

}

#ifndef ARDUINO
int main(int argc, char **argv)
{
  setup();
  loop();
  abort();
}
#endif
