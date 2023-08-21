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
#include <TF800A12K.h>

using namespace OxCore;


#include <PID_v1.h>

#include <core_defines.h>
#include <core.h>
#include <machine.h>

#include <SanyoAceB97.h>
#include <Arduino.h>

#ifdef USE_MAX31850_THERMOCOUPLES
#include <MAX31850.h>
#else
#include <DS18B20_temperature.h>
#endif

// We aren't using this, but it brings in declarations we need!
#include <cog_task.h>

#include <duty_cycle_task.h>
// #include <spud_task.h>



using namespace OxCore;
static Core core;

const unsigned long REPORT_PERIOD_MS = 3000;

MachineConfig *machineConfig;
TaskProperties _properties;
SanyoAceB97 *fan;

// This exists purely to be compatible with the code
// in the make task library
MachineConfig *localGetConfig() {
  return machineConfig;
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
const float RAMP_DN_TARGET_D_MIN = -0.5; // degrees C per minute

// This is the overall target_temp, which changes over time.

const float HOLD_TEMPERATURE = 600.0;
const float STOP_TEMPERATURE = 27.0;
const float START_TEMPERATURE = 600.0;

float TARGET_TEMP = 27.0;

const unsigned long HOLD_TIME_MINUTES = 1;
const unsigned long HOLD_TIME_SECONDS = 60 * HOLD_TIME_MINUTES;
const float STARTING_DUTY_CYCLE_FRACTION = 0.0;
const float STACK_VOLTAGE = 12.0;
const float STACK_AMPERAGE = 3.0;

// These parameters are related to our control procedure.
// This is similar to a PID loop, but I don't think any integration
// is needed, and want to have a test that doesn't depend on a PID
// loop. Arguably, that is just implementing a very simple PD loop
// myself; I'll accept that rap.

// Let DutyCycle be changing Duty cycle, a fraction of 1.0.
// In PID termonology, this is the "PLANT VARIABLE"--what we can control.
float GlobalDutyCycle = STARTING_DUTY_CYCLE_FRACTION;
// Temperature Read Period is how often we will update the
// Temperature sensor.
const int TEMPERATURE_READ_PERIOD_MS = 5000;
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
const int NUM_STACKS = 1;

GGLabsSSR1 _ac_heaters[NUM_HEATERS];

AbstractPS* _stacks[NUM_STACKS];

// This is copied directly from cog_task.cpp...
void _updateStackVoltage(float voltage,MachineConfig *machineConfig) {
  for (int i = 0; i < NUM_STACKS; i++) {
    _stacks[i]->updateVoltage(voltage,machineConfig);
  }
}
void _updateStackAmperage(float amperage,MachineConfig *machineConfig) {
  for (int i = 0; i < NUM_STACKS; i++) {
    _stacks[i]->updateAmperage(amperage,machineConfig);
  }
}


// SpudTask *spudTask;

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

  // Runs the Pressure Swing Adsorption cycle
  class HeaterPIDTask : public OxCore::Task
  {
  public:
    HeaterPIDTask();
    PID *pidControllerHeater;
    int DEBUG_PID = 1;
    const int PERIOD_MS = 60*1000;

    // These are on a scale of 1.0;
    double dutyCycle_Output = 0.0;
    double final_dutyCycle = 0.0;
    double HeaterSetPoint_C = 0.0;
    double Input_temperature_C = 0.0;
    DutyCycleTask *dutyCycleTask;
  private:
    bool _init() override;
    bool _run() override;
    // This would go into the abstract class.
  };


  HeaterPIDTask::HeaterPIDTask() {


    /* Setting the initial parameters is always weird for PID controller..
     */

    // These initial values are from Tom Taylor, scaled by the fact
    // that I am using a plant variable of duty cycle on a [0.0..1.0] scale.
    // double FKp = 0.03;
    // double FKi = 0.003;
    // double FKd = 0.0;

    // This "AJUSTMEMNT" was found by experiment
    double ADJUSTMENT = 15.0;
    double FKp = 0.03/ADJUSTMENT;
    // I want zero overshoot, so I am setting this to zero!
    //    double FKi = 0.003/ADJUSTMENT;
    double FKi = 0.0;
    double FKd = 0.0/ADJUSTMENT;

    // Upon after testing, the parameters above, operating at a 60 second
    // sample time, is entirely too high, and allowed an overshoot of 8 degree C
    // in only one second!
    const int SAMPLE_TIME_MS = PERIOD_MS;
    // dutyCycle is measured betwen 0.0 and 1.0 (and ends
    // up being a PWM duty cycle)
    this->pidControllerHeater =
      new PID(&(this->Input_temperature_C), &(this->dutyCycle_Output),
              &(this->HeaterSetPoint_C), FKp, FKi, FKd, DIRECT);
    this->pidControllerHeater->SetOutputLimits(-1.0, 1.0);
    this->pidControllerHeater->SetSampleTime(PERIOD_MS);
    this->pidControllerHeater->SetMode(AUTOMATIC);
  }

  bool HeaterPIDTask::_init()
  {

    OxCore::Debug<const char *>("HeaterPIDTask init\n");
    return true;
  }

  bool HeaterPIDTask::_run()
  {
    OxCore::Debug<const char *>("HeaterPIDTask run\n");
    this->HeaterSetPoint_C = TARGET_TEMP;

    double previousInput = this->Input_temperature_C;
    this->Input_temperature_C = localGetConfig()->report->post_heater_C;

    pidControllerHeater->Compute();

    double s = this->dutyCycle_Output + this->final_dutyCycle;

    s = min(s, 1.0);
    s = max(s, 0.0);
    this->final_dutyCycle = s;

    dutyCycleTask->dutyCycle = s;

    // This resets our duty_cycle computation
    dutyCycleTask->reset_duty_cycle();

    localGetConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;

    // The heater DutyCycle should go into the config, but
    // does not currently exist there!
    //        localGetConfig()->fanPWM = s;

    if (DEBUG_PID > 0) {
      OxCore::Debug<const char *>("previous input ");
      OxCore::DebugLn<double>(previousInput);
      OxCore::Debug<const char *>("Final dutyCycle_Output ");
      OxCore::DebugLn<double>(this->dutyCycle_Output);
      OxCore::Debug<const char *>("Final dutyCycle ");
      OxCore::DebugLn<double>(this->final_dutyCycle);
    }

    return true;
  }

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
    void addTempToQueue(float c);
    float tempFromTime(int t_ms);
    void calculateDdelta();
    void dumpQueue();
    int ringCompuation(int n);
  private:
    bool _init() override;
    bool _run() override;
  };

  void ReadTempsTask::addTempToQueue(float c) {
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
    float DdeltaRaw = t0 - t1;
    // now compute Ddelta_C_per_min...
    float Ddelta_C_per_min_computed = (DdeltaRaw * TEMPERATRUE_TIME_DELTA_MS) / (60.0*1000.0);
    // now we will set the global so that it is available to the other tasks
    Ddelta_C_per_min = Ddelta_C_per_min_computed;
  }

  void ReadTempsTask::updateTemperatures() {
    _readTemperatureSensors();
    float postHeaterTemp = _temperatureSensors[0].GetTemperature(0);
    float postStackTemp = _temperatureSensors[0].GetTemperature(1);
    float postGetterTemp = _temperatureSensors[0].GetTemperature(2);
    // Sometimes we get a data read error, that comes across
    // as -127.00. In that case, we will leave the
    // value unchanged from the last read.
    if (postHeaterTemp > -100.0) {
      localGetConfig()->report->post_heater_C = postHeaterTemp;
    } else {
      OxCore::Debug<const char *>("Bad post_heater_C\n");
    }
    if (postGetterTemp > -100.0) {
      localGetConfig()->report->post_getter_C = postGetterTemp;
    } else {
      OxCore::Debug<const char *>("Bad post_getter_C\n");
    }
    if (postStackTemp > -100.0) {
      localGetConfig()->report->post_stack_C = postStackTemp;
    } else {
      OxCore::Debug<const char *>("Bad post_stack_C\n");
    }
    if (DEBUG_READ_TEMPS > 0) {
      outputReport(localGetConfig()->report);
      OxCore::Debug<const char *>("Target Temp : ");
      OxCore::DebugLn<float>(TARGET_TEMP);
    }
    addTempToQueue(localGetConfig()->report->post_heater_C);
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
        OxCore::Debug<const char *>(localGetConfig()->TempLocationNames[i]);
        OxCore::Debug<const char *>(": ");
        OxCore::DebugLn<float>(temperature);
      }
    }
    if (DEBUG_READ_TEMPS > 1) {
      OxCore::Debug<const char *>("Ddelta_C_per_min :");
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
  State global_state = RampingUp;


  // This task controls the ramp up, hold, and ramp down.
  class SupervisorTask : public OxCore::Task
  {
  public:
    SupervisorTask();
    const int DEBUG_ID = 0;
    const int PERIOD_MS = 10000;
    //    Global_State global_state = RampingUp;
    unsigned long begin_hold_time;
    unsigned long begin_down_time;
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
    // This should not be necessary here, it should be required only once...
    _updateStackVoltage(STACK_VOLTAGE,machineConfig);
    _updateStackAmperage(STACK_AMPERAGE,machineConfig);

    const unsigned long ms = millis();
    switch (global_state) {
    case RampingUp: {
      analogWrite(fan->PWM_PIN[0],153);
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: RAMPING UP\n");
      }
      float postHeaterTemp = localGetConfig()->report->post_heater_C;
      if (postHeaterTemp > HOLD_TEMPERATURE) {
        global_state = Holding;
        OxCore::Debug<const char *>("State Changing to HOLDING\n");
        begin_hold_time = millis();
      } else {
        const unsigned long MINUTES_RAMPING_UP = ms / (60 * 1000);
        TARGET_TEMP = START_TEMPERATURE + MINUTES_RAMPING_UP * RAMP_UP_TARGET_D_MIN;
        TARGET_TEMP = min(TARGET_TEMP, HOLD_TEMPERATURE);
      }
      break;
    };
    case Holding: {
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: HOLDING\n");
      }
      unsigned long ms = millis();
      if (((ms - begin_hold_time) / 1000) > HOLD_TIME_SECONDS) {
        global_state = RampingDn;
        begin_down_time = ms;
        OxCore::Debug<const char *>("State: Changing to RAMPING DN\n");
      } else {
        const unsigned long MINUTES_HOLDING = ms / (60 * 1000);
      }
      break;
    };
    case RampingDn: {
      if (DEBUG_ID > 0) {
        OxCore::Debug<const char *>("State: RAMPING DN\n");
      }
      float postHeaterTemp = localGetConfig()->report->post_heater_C;
      if (postHeaterTemp < STOP_TEMPERATURE) {
        analogWrite(fan->PWM_PIN[0],5);
        OxCore::Debug<const char *>("Stop temperature reached!\n");
        OxCore::Debug<const char *>("=======================\n");
        delay(1000);
        while(1);
      } else {
        const unsigned long MINUTES_RAMPING_DN = (ms - begin_down_time) / (60 * 1000);
        TARGET_TEMP = HOLD_TEMPERATURE + MINUTES_RAMPING_DN * RAMP_DN_TARGET_D_MIN;
        TARGET_TEMP = max(TARGET_TEMP,STOP_TEMPERATURE);
      }
      break;
    };
    }
    return true;
  }
}

using namespace OxApp;
ReadTempsTask readTempsTask;
SupervisorTask supervisorTask;
// ControllerTask controllerTask;
HeaterPIDTask heaterPIDTask;


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

  machineConfig->report = new MachineStatusReport();
  machineConfig->hal = new MachineHAL();

  for(int i = 0; i < NUM_HEATERS; i++) {
    _ac_heaters[i].setHeater(0,LOW);
    _ac_heaters[i].setHeater(1,LOW);
  }

  bool initSuccess  = localGetConfig()->hal->init();
  Serial.println("about to start!");
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");
  }

  _stacks[0] = new SL_PS("FIRST_STACK",0);

  _stacks[0]->init();


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

  DutyCycleTask *dutyCycleTask;
  dutyCycleTask = new DutyCycleTask();
  heaterPIDTask.dutyCycleTask = dutyCycleTask;


  //  dutyCycleTask->NUM_HEATERS = 2;
  //  dutyCycleTask->dutyCycle = STARTING_DUTY_CYCLE_FRACTION;

  OxCore::TaskProperties cogProperties;
  cogProperties.name = "cog";
  cogProperties.id = 20;
  cogProperties.state_and_config = (void *) localGetConfig();
  delay(1000);
  Serial.println("About to run test!");
  delay(100);
  fan = new SanyoAceB97("FIRST_FAN",0,RF_FAN,1.0);
  Serial.println("Fan Set up");
  fan->_init();
  Serial.println("fan init done!");
  fan->DEBUG_FAN = 0;


  pinMode(fan->PWM_PIN[0], OUTPUT);
  // It is safer to be completely off until we are ready to start!
  analogWrite(fan->PWM_PIN[0],0);


  Serial.println("Fan Set up");
  delay(100);
  OxCore::TaskProperties readTempsProperties;
  readTempsProperties.name = "readTemps";
  readTempsProperties.id = 25;
  readTempsProperties.period = readTempsTask.PERIOD_MS;
  readTempsProperties.priority = OxCore::TaskPriority::High;
  readTempsProperties.state_and_config = (void *) localGetConfig();
  delay(300);
  core.AddTask(&readTempsTask, &readTempsProperties);
  delay(100);

  OxCore::TaskProperties supervisorProperties;
  supervisorProperties.name = "supervisor";
  supervisorProperties.id = 26;
  supervisorProperties.period = supervisorTask.PERIOD_MS;
  supervisorProperties.priority = OxCore::TaskPriority::High;
  supervisorProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(&supervisorTask, &supervisorProperties);


  // OxCore::TaskProperties controllerProperties;
  // controllerProperties.name = "controller";
  // controllerProperties.id = 27;
  // controllerProperties.period = controllerTask.PERIOD_MS;
  // controllerProperties.priority = OxCore::TaskPriority::High;
  // controllerProperties.state_and_config = (void *) localGetConfig();
  // core.AddTask(&controllerTask, &controllerProperties);


  OxCore::TaskProperties dutyCycleProperties;
  dutyCycleProperties.name = "dutyCycle";
  dutyCycleProperties.id = 28;
  dutyCycleProperties.period = dutyCycleTask->PERIOD_MS;
  dutyCycleProperties.priority = OxCore::TaskPriority::Low;
  dutyCycleProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(dutyCycleTask, &dutyCycleProperties);


  OxCore::TaskProperties HeaterPIDProperties;
  HeaterPIDProperties.name = "HeaterPID";
  HeaterPIDProperties.id = 29;
  HeaterPIDProperties.period = heaterPIDTask.PERIOD_MS;
  HeaterPIDProperties.priority = OxCore::TaskPriority::High;
  HeaterPIDProperties.state_and_config = (void *) localGetConfig();
  core.AddTask(&heaterPIDTask, &HeaterPIDProperties);

  OxCore::Debug<const char *>("Added tasks\n");

  _updateStackVoltage(STACK_VOLTAGE,machineConfig);
  _updateStackAmperage(STACK_AMPERAGE,machineConfig);
  analogWrite(fan->PWM_PIN[0],153);

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
