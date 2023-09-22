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
        // These are the dynamic targets
        OxCore::Debug<const char *>("Target     C: ");
        OxCore::DebugLn<float>(msr->target_temp_C);
        OxCore::Debug<const char *>("Setpoint   C: ");
        OxCore::DebugLn<float>(msr->setpoint_temp_C);
        OxCore::Debug<const char *>("Target Ramp C: ");
        OxCore::DebugLn<float>(msr->target_ramp_C);
        OxCore::Debug<const char *>("Max Stack   A: ");
        OxCore::DebugLn<float>(msr->max_stack_amps_A);
        OxCore::Debug<const char *>("Max Stack   W: ");
        OxCore::DebugLn<float>(msr->max_stack_watts_W);
        OxCore::Debug<const char *>("Fan PWM      : ");
        OxCore::DebugLn<float>(msr->fan_pwm);


        OxCore::Debug<const char *>("Post Heater C: ");
        OxCore::DebugLn<float>(msr->post_heater_C);
        OxCore::Debug<const char *>("Post Getter C: ");
        OxCore::DebugLn<float>(msr->post_getter_C);
        OxCore::Debug<const char *>("Post Stack  C: ");
        OxCore::DebugLn<float>(msr->post_stack_C);

        OxCore::Debug<const char *>("Heater DC    : ");
        OxCore::DebugLn<float>(msr->heater_duty_cycle);
        OxCore::Debug<const char *>("Stack amps  A: ");
        OxCore::DebugLn<float>(msr->stack_amps);
        OxCore::Debug<const char *>("Stack watts W: ");
        OxCore::DebugLn<float>(msr->stack_watts);
        OxCore::Debug<const char *>("Stack volts V: ");
        OxCore::DebugLn<float>(msr->stack_voltage);
        OxCore::Debug<const char *>("Stack ohms  O: ");
        if (isnan(msr->stack_ohms) || msr->stack_ohms < 0.0) {
          OxCore::DebugLn<const char*>(" N/A");
        } else {
          OxCore::DebugLn<float>(msr->stack_ohms);
        }
        OxCore::Debug<const char *>("Fan RPM      : ");
        OxCore::DebugLn<float>(msr->fan_rpm);
}

void MachineConfig::createJSONReport(MachineStatusReport* msr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"MachineState\": %d",msr->ms);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"TargetC\": %.2f",msr->target_temp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"SetpointC\": %.2f",msr->setpoint_temp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"RampC\": %.2f",msr->target_ramp_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"MaxStackA\": %.2f",msr->max_stack_amps_A);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"MaxStackW\": %.2f",msr->max_stack_watts_W);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"FanPWM\": %.2f",msr->fan_pwm);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"HeaterC\": %.2f",msr->post_heater_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackC\": %.2f",msr->post_stack_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"GetterC\": %.2f",msr->post_getter_C);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"HeaterDutyCycle\": %.2f",msr->heater_duty_cycle);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackA\": %.2f",msr->stack_amps);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackW\": %.2f",msr->stack_watts);
  strcat(buffer, ",\n");
  sprintf(buffer+strlen(buffer), "\"StackV\": %.2f",msr->stack_voltage);
  strcat(buffer, ",\n");
  if (isnan(msr->stack_ohms) || isinf(msr->stack_ohms) || msr->stack_ohms < 0.0) {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": -1.0");
    strcat(buffer, ",\n");
  } else {
    sprintf(buffer+strlen(buffer), "\"StackOhms\": %.2f",msr->stack_ohms);
    strcat(buffer, ",\n");
  }
  sprintf(buffer+strlen(buffer), "\"FanRPM\": %.2f",msr->fan_rpm);
  strcat(buffer, "\n");
}

void MachineConfig::change_ramp(float ramp) {
  RAMP_UP_TARGET_D_MIN = ramp;
  RAMP_DN_TARGET_D_MIN = -ramp;
  report->target_ramp_C = ramp;
}

bool MachineHAL::init() {
  init_heaters();
}


MachineConfig::MachineConfig() {
 script = new MachineScript();
 report = new MachineStatusReport();
 clearErrors();
 }

void MachineConfig::clearErrors() {
// here we init the errors...
 // If we lose a thermocouple for more than 10 seconds
 // we have to go into emergency shutdown.
 errors[POST_HEATER_TC_BAD].fault_present = false;
 errors[POST_GETTER_TC_BAD].fault_present = false;
 errors[POST_STACK_TC_BAD].fault_present  = false;

 errors[POST_HEATER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_GETTER_TC_BAD].toleration_ms = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;
 errors[POST_STACK_TC_BAD].toleration_ms  = THERMOCOUPLE_FAULT_TOLERATION_TIME_MS;

 errors[POST_HEATER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_GETTER_TC_BAD].response_state = EmergencyShutdown;
 errors[POST_STACK_TC_BAD].response_state  = EmergencyShutdown;
}

// This code is currently not invoked.
// On Sept. 15th, 2023, we chose to use the more manual "5 knobs"
// approach. I'm retaining this because I believe we will eventually
// need this for "one-button" operation.
void MachineConfig::runComplexAlgolAssertions() {
  // How we make certain assertions to make sure we are well configured
  Serial.println("BEGINNING ASSERTION CHECKS!!");
  Serial.println("IF YOU DO NOT SEE THE WORDS 'ALL CLEAR' BELOW AN ASSERTION HAS FAILED");
  delay(100);
  // assert(RAMP_UP_TARGET_D_MIN >= 0.0);
  // assert(RAMP_DN_TARGET_D_MIN <= 0.0);

  // assert(YELLOW_TEMP < RED_TEMP);
  // assert(OPERATING_TEMP < YELLOW_TEMP);
  // assert(STOP_TEMP < OPERATING_TEMP);

  // assert(FAN_SPEED_AT_OPERATING_TEMP < FULL_POWER_FOR_FAN);
  // assert(TEMP_TO_BEGIN_FAN_SLOW_DOWN < OPERATING_TEMP);
  // assert(OPERATING_TEMP < END_FAN_SLOW_DOWN);
  Serial.println("ALL CLEAR!!");
  delay(50);
}

bool MachineConfig::init() {
 Serial.println("error toleration times (ms)");
 for(int i = 0; i < 3; i++) {
   Serial.println(errors[i].toleration_ms);
 }
  return true;
}
