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


Stage2Config::Stage2Config()  {
    s2sr = new Stage2StatusReport();
  }


void Stage2Config::outputReport(Stage2StatusReport *msr) {
        OxCore::DebugLn<const char *>("");
        OxCore::Debug<const char *>("Machine State: ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[msr->ms]);
        OxCore::Debug<const char *>("int1_temp_C: ");
        OxCore::DebugLn<float>(msr->int1_temp_C);
        OxCore::Debug<const char *>("ext1_temp_C: ");
        OxCore::DebugLn<float>(msr->ext1_temp_C);
        OxCore::Debug<const char *>("ext2_temp_C: ");
        OxCore::DebugLn<float>(msr->ext2_temp_C);

        OxCore::Debug<const char *>("Heater DC int1: ");
        Serial.println(msr->heater_duty_cycle_int1,4);
        OxCore::Debug<const char *>("Heater DC ext1: ");
        Serial.println(msr->heater_duty_cycle_ext1,4);
        OxCore::Debug<const char *>("Heater DC ext2: ");
        Serial.println(msr->heater_duty_cycle_ext2,4);
}

void Stage2Config::createJSONReport(Stage2StatusReport* msr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"MachineState\": %d,\n",msr->ms);
  sprintf(buffer+strlen(buffer), "\"int1_temp_C\": %.2f,\n",msr->int1_temp_C);
  sprintf(buffer+strlen(buffer), "\"ext1_temp_C\": %.2f,\n",msr->ext1_temp_C);
  sprintf(buffer+strlen(buffer), "\"ext2_temp_C\": %.2f,\n",msr->ext2_temp_C);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_int1\": %.2f,\n",msr->heater_duty_cycle_int1);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_ext1\": %.2f,\n",msr->heater_duty_cycle_ext1);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_ext2\": %.2f,\n",msr->heater_duty_cycle_ext2);
}
