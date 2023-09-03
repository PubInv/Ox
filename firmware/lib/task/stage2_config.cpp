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


void MachineConfig::outputStage2Report(Stage2StatusReport *s2sr) {
        OxCore::DebugLn<const char *>("Machine State: (INT1, EXT1, EXT2) ");
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[s2sr->ms[Int1]]);
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[s2sr->ms[Ext1]]);
        OxCore::DebugLn<const char *>(MachineConfig::MachineStateNames[s2sr->ms[Ext2]]);

        OxCore::Debug<const char *>("target_int1_temp_C: ");
        OxCore::DebugLn<float>(s2sr->target_temp_C[Int1]);
        OxCore::Debug<const char *>("target_ext1_temp_C: ");
        OxCore::DebugLn<float>(s2sr->target_temp_C[Ext1]);
        OxCore::Debug<const char *>("target_ext2_temp_C: ");
        OxCore::DebugLn<float>(s2sr->target_temp_C[Ext2]);

        OxCore::Debug<const char *>("int1_temp_C: ");
        OxCore::DebugLn<float>(s2sr->temp_C[Int1]);
        OxCore::Debug<const char *>("ext1_temp_C: ");
        OxCore::DebugLn<float>(s2sr->temp_C[Ext1]);
        OxCore::Debug<const char *>("ext2_temp_C: ");
        OxCore::DebugLn<float>(s2sr->temp_C[Ext2]);

        OxCore::Debug<const char *>("Heater DC int1: ");
        Serial.println(s2sr->heater_duty_cycle[Int1],4);
        OxCore::Debug<const char *>("Heater DC ext1: ");
        Serial.println(s2sr->heater_duty_cycle[Ext1],4);
        OxCore::Debug<const char *>("Heater DC ext2: ");
        Serial.println(s2sr->heater_duty_cycle[Ext2],4);
}

void MachineConfig::createStage2JSONReport(Stage2StatusReport* s2sr, char *buffer) {
  sprintf(buffer+strlen(buffer), "\"MachineState INT1\": %d,\n",s2sr->ms[Int1]);
  sprintf(buffer+strlen(buffer), "\"MachineState EXT1\": %d,\n",s2sr->ms[Ext1]);
  sprintf(buffer+strlen(buffer), "\"MachineState EXT2\": %d,\n",s2sr->ms[Ext2]);
  sprintf(buffer+strlen(buffer), "\"target_int1_temp_C\": %.2f,\n",s2sr->target_temp_C[Int1]);
  sprintf(buffer+strlen(buffer), "\"target_ext1_temp_C\": %.2f,\n",s2sr->target_temp_C[Ext1]);
  sprintf(buffer+strlen(buffer), "\"target_ext2_temp_C\": %.2f,\n",s2sr->target_temp_C[Ext2]);

  sprintf(buffer+strlen(buffer), "\"int1_temp_C\": %.2f,\n",s2sr->temp_C[Int1]);
  sprintf(buffer+strlen(buffer), "\"ext1_temp_C\": %.2f,\n",s2sr->temp_C[Ext1]);
  sprintf(buffer+strlen(buffer), "\"ext2_temp_C\": %.2f,\n",s2sr->temp_C[Ext2]);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_int1\": %.2f,\n",s2sr->heater_duty_cycle[Int1]);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_ext1\": %.2f,\n",s2sr->heater_duty_cycle[Ext1]);
  sprintf(buffer+strlen(buffer), "\"heater_duty_cycle_ext2\": %.2f,\n",s2sr->heater_duty_cycle[Ext2]);
}
