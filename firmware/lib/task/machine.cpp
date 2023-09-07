/*
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

#include <machine.h>
// there is not yet anything for this to do
#include <core.h>
// #include <Wire.h>
#include <assert.h>


void MachineConfig::outputReport(MachineStatusReport *msr) {
        OxCore::DebugLn<const char *>("");
        OxCore::Debug<const char *>("Machine State: ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[msr->ms]);
        OxCore::Debug<const char *>("Target      C: ");
        OxCore::DebugLn<float>(msr->target_temp_C);
        OxCore::Debug<const char *>("Post Heater C: ");
        OxCore::DebugLn<float>(msr->post_heater_C);
        OxCore::Debug<const char *>("Post Getter C: ");
        OxCore::DebugLn<float>(msr->post_getter_C);
        OxCore::Debug<const char *>("Post Stack  C: ");
        OxCore::DebugLn<float>(msr->post_stack_C);
        OxCore::Debug<const char *>("Stack volts V: ");
        OxCore::DebugLn<float>(msr->stack_voltage);
        OxCore::Debug<const char *>("Stack amps  A: ");
        OxCore::DebugLn<float>(msr->stack_amps);
        OxCore::Debug<const char *>("Stack ohms  O: ");
        if (isnan(msr->stack_ohms) || msr->stack_ohms < 0.0) {
          OxCore::DebugLn<const char*>(" N/A");
        } else {
          OxCore::DebugLn<float>(msr->stack_ohms);
        }
        OxCore::Debug<const char *>("Heater DC    : ");
        Serial.println(msr->heater_duty_cycle,4);
        OxCore::Debug<const char *>("Fan PWM      : ");
        OxCore::DebugLn<float>(msr->fan_pwm);
        OxCore::Debug<const char *>("Fan RPM      : ");
        OxCore::DebugLn<float>(msr->fan_rpm);
}

void MachineConfig::createJSONReport(MachineStatusReport* msr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"MachineState\": %d,\n",msr->ms);
  sprintf(buffer+strlen(buffer), "\"TargetC\": %.2f,\n",msr->target_temp_C);
  sprintf(buffer+strlen(buffer), "\"HeaterC\": %.2f,\n",msr->post_heater_C);
  sprintf(buffer+strlen(buffer), "\"StackC\": %.2f,\n",msr->post_stack_C);
  sprintf(buffer+strlen(buffer), "\"GetterC\": %.2f,\n",msr->post_getter_C);
  sprintf(buffer+strlen(buffer), "\"StackV\": %.2f,\n",msr->stack_voltage);
  sprintf(buffer+strlen(buffer), "\"StackA\": %.2f,\n",msr->stack_amps);
  if (isnan(msr->stack_ohms) || isinf(msr->stack_ohms) || msr->stack_ohms < 0.0) {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": -1.0,\n");
  } else {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": %.2f,\n",msr->stack_ohms);
  }
  sprintf(buffer+strlen(buffer), "\"HeaterDutyCycle\": %.2f,\n",msr->heater_duty_cycle);
  sprintf(buffer+strlen(buffer), "\"FanPWM\": %.2f,\n",msr->fan_pwm);
  sprintf(buffer+strlen(buffer), "\"FanRPM\": %.2f\n",msr->fan_rpm);
}

bool MachineHAL::init() {
  init_heaters();

}


MachineConfig::MachineConfig() {
 script = new MachineScript();
 report = new MachineStatusReport();

}

bool MachineConfig::init() {
  // How we make certain assertions to make sure we are well configured
  Serial.println("BEGINNING ASSERTION CHECKS!!");
  Serial.println("IF YOU DO NOT SEE THE WORDS 'ALL CLEAR' BELOW AN ASSERTION HAS FAILED");
  delay(100);
  assert(RAMP_UP_TARGET_D_MIN >= 0.0);
  assert(RAMP_DN_TARGET_D_MIN <= 0.0);

  assert(YELLOW_TEMP < RED_TEMP);
  assert(OPERATING_TEMP < YELLOW_TEMP);
  assert(STOP_TEMP < OPERATING_TEMP);

  assert(FAN_SPEED_AT_OPERATING_TEMP < FULL_POWER_FOR_FAN);
  assert(TEMP_TO_BEGIN_FAN_SLOW_DOWN < OPERATING_TEMP);
  assert(OPERATING_TEMP < END_FAN_SLOW_DOWN);
  Serial.println("ALL CLEAR!!");

}
