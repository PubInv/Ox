/*
  stage2_config -- configuration specifically for the Stage2 heaters of the high-oxygen experiment

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

#include <stage2_config.h>
#include <core.h>


void MachineConfig::outputStage2Report(
                                       Stage2Heater s2h,MachineStatusReport *msr,
                                       float target_temp,float measured_temp,float duty_cycle) {
  OxCore::Debug<const char *>("Stage2Heater: ");

  OxCore::DebugLn<const char *>(MachineConfig::HeaterNames[s2h]);

  OxCore::Debug<const char *>("Machine State: ");
  OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[msr->ms]);

  OxCore::Debug<const char *>("Target      C: ");
  OxCore::DebugLn<float>(target_temp);
  OxCore::Debug<const char *>("Temp C       : ");
  // This needs to change base on our state...
  OxCore::DebugLn<float>(measured_temp);
  OxCore::Debug<const char *>("Heater DC    : ");
  Serial.println(duty_cycle,4);

}

void MachineConfig::createStage2JSONReport(Stage2Heater s2h,MachineStatusReport* msr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"Stage2Heater\": %d,\n",s2h);
  sprintf(buffer+strlen(buffer), "\"MachineState\": %d,\n",msr->ms);
  sprintf(buffer+strlen(buffer), "\"TargetC\": %.2f,\n",msr->target_temp_C);
  sprintf(buffer+strlen(buffer), "\"HeaterC\": %.2f,\n",msr->post_heater_C);
  sprintf(buffer+strlen(buffer), "\"HeaterDutyCycle\": %.2f,\n",msr->heater_duty_cycle);


}
