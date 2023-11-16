/*

  wattage_pid_object.cpp -- A simple PID controller for the Toatal Wattage

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

#include <wattage_pid_object.h>

// Note: This routine may now be obsolete
WattagePIDObject::WattagePIDObject() {
    this->pidControllerWattage =
      new PID(&(this->input_temperature_C), &(this->totalWattage_Output_W),
              &(this->temperatureSetPoint_C), FKp, FKi, FKd, DIRECT);
    this->pidControllerWattage->SetOutputLimits(0.0, MachineConfig::HEATER_MAXIMUM_WATTAGE);
    // This should probably be changed...
    this->pidControllerWattage->SetSampleTime(MachineConfig::WATTAGE_PID_SAMPLE_TIME_MS);
    this->pidControllerWattage->SetMode(AUTOMATIC);
}

void WattagePIDObject::printTunings() {
    Serial.print("Tunings for Tottal Wattage PID Task: ");
    Serial.print(" ");
    Serial.print(FKp,5);
    Serial.print(", ");
    Serial.print(FKi,5);
    Serial.print(", ");
    Serial.print(FKd,5);
    Serial.println();
    Serial.print("Tunings (Inverted, 1/x): ");
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

void WattagePIDObject::SetTunings(double p, double i, double d) {
  FKp = p;
  FKi = i;
  FKd = d;
  this->pidControllerWattage->SetTunings(p,i,d);
  if (DEBUG_PID > 0) {
    printTunings();
  }
}

double WattagePIDObject::GetKp() {
  return FKp;
}
double WattagePIDObject::GetKi() {
  return FKi;
}
double WattagePIDObject::GetKd() {
  return FKd;
}

float WattagePIDObject::compute(float controlTemp)
{
  if (DEBUG_PID > 1) {
    OxCore::Debug<const char *>("WattagePIDObject run\n");
    OxCore::DebugLn<const char *>("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    OxCore::DebugLn<float>(this->temperatureSetPoint_C);
    OxCore::DebugLn<float>(this->input_temperature_C);
  }

  double previousInput = this->input_temperature_C;

  this->input_temperature_C = controlTemp;

  pidControllerWattage->Compute();

  double s = this->totalWattage_Output_W;

  this->final_totalWattage_W = s;


  if (DEBUG_PID > 1) {
    OxCore::Debug<const char *>("Setpoint");
    Serial.println(this->temperatureSetPoint_C,2);
    OxCore::Debug<const char *>("previous input ");
    Serial.println(previousInput,5);
    OxCore::Debug<const char *>("Final Total Wattage ");
    Serial.println(this->final_totalWattage_W,5);
  }

  return true;
}
