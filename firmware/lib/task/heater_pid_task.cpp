/*

  heater_pid_ttask.cpp -- A simple PID controller for the SSR AC heater

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

#include <heater_pid_task.h>

// Note: This routine may now be obsolete
HeaterPIDTask::HeaterPIDTask() {
  // dutyCycle is measured betwen 0.0 and 1.0 (and ends
  // up being a PWM duty cycle)
  this->pidControllerHeater =
    new PID(&(this->Input_temperature_C), &(this->dutyCycle_Output),
            &(this->HeaterSetPoint_C), FKp, FKi, FKd, DIRECT);
  this->pidControllerHeater->SetOutputLimits(-1.0, 1.0);
  this->pidControllerHeater->SetSampleTime(MachineConfig::INIT_PID_PERIOD_MS);
  this->pidControllerHeater->SetMode(AUTOMATIC);
}

bool HeaterPIDTask::_init()
{
  OxCore::Debug<const char *>("HeaterPIDTask init\n");
  return true;
}

void HeaterPIDTask::printTunings() {
    Serial.print("Tunings for            : ");
    Serial.print(MachineConfig::HeaterNames[whichHeater]);
    Serial.print(" ");
    Serial.print(FKp,5);
    Serial.print(", ");
    Serial.print(FKi,5);
    Serial.print(", ");
    Serial.print(FKd,5);
    Serial.println();
    Serial.print("Tunings (Inverted, 1/x): ");
    Serial.print(MachineConfig::HeaterNames[whichHeater]);
    Serial.print(" ");
    if (FKp == 0.0) {
      Serial.print("nan");
    } else {
      Serial.print(1.0/FKp,2);
    }
    Serial.print(", ");
    if (FKi == 0.0) {
      Serial.print("nan");
    } else {
      Serial.print(1.0/FKi,2);
    }
    Serial.print(", ");
    if (FKd == 0.0) {
      Serial.print("nan");
    } else {
      Serial.print(1.0/FKd,2);
    }
    Serial.println();
}

void HeaterPIDTask::SetTunings(double p, double i, double d) {
  FKp = p;
  FKi = i;
  FKd = d;
  this->pidControllerHeater->SetTunings(p,i,d);
  if (DEBUG_PID > 0) {
    printTunings();
  }
}

double HeaterPIDTask::GetKp() {
  return FKp;
}
double HeaterPIDTask::GetKi() {
  return FKi;
}
double HeaterPIDTask::GetKd() {
  return FKd;
}

void HeaterPIDTask::shutHeaterDown() {
  this->HeaterSetPoint_C = 25.0;
  getConfig()->report->heater_duty_cycle = 0.0;
  dutyCycleTask->dutyCycle = 0.0;
}
bool HeaterPIDTask::_run()
{
  if (DEBUG_PID > 1) {
    OxCore::Debug<const char *>("HeaterPIDTask run\n");
    double test_spud = getConfig()->report->post_heater_C;
    OxCore::DebugLn<const char *>("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    OxCore::DebugLn<float>(this->HeaterSetPoint_C);
    OxCore::DebugLn<float>(this->Input_temperature_C);
  }

  MachineState ms = getConfig()->ms;
  if ((ms == Off) || (ms == EmergencyShutdown) || (ms == OffUserAck)) {
    // in this case, we do nothing...but we will put the set point
    // to room temperature.
    this->HeaterSetPoint_C = 25.0;
    getConfig()->report->heater_duty_cycle = 0.0;
    dutyCycleTask->dutyCycle = 0.0;
    return true;
  }

  double previousInput = this->Input_temperature_C;

  this->Input_temperature_C = getConfig()->report->post_heater_C;

  // didn't hang when return was here.
  pidControllerHeater->Compute();

  // double s = this->dutyCycle_Output + this->final_dutyCycle;
  double s = this->dutyCycle_Output;
  // didn't hang when return was here

  s = min(s, 1.0);
  s = max(s, 0.0);
  this->final_dutyCycle = s;

  dutyCycleTask->dutyCycle = s;

  // This resets our duty_cycle computation
  //  dutyCycleTask->reset_duty_cycle();

  getConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;

  if (DEBUG_PID > 1) {
    OxCore::Debug<const char *>("Setpoint");
    Serial.println(this->HeaterSetPoint_C,2);
    OxCore::Debug<const char *>("previous input ");
    Serial.println(previousInput,5);
    OxCore::Debug<const char *>("Final dutyCycle_Output ");
    Serial.println(this->dutyCycle_Output,5);
    OxCore::Debug<const char *>("Final dutyCycle ");
    Serial.println(this->final_dutyCycle,5);
  }

  return true;
}
