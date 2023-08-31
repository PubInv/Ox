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

#ifndef STAGE2_CONFIG_H
#define STAGE2_CONFIG_H

#include <machine.h>


struct Stage2StatusReport {
  MachineState ms;
  float int1_temp_C;
  float ext1_temp_C;
  float ext2_temp_C;
  float heater_duty_cycle_int1;
  float heater_duty_cycle_ext1;
  float heater_duty_cycle_ext2;
};

// TODO: move to a new file
class Stage2Config : public MachineConfig {
public:
  Stage2Config();
  Stage2StatusReport *s2sr;
  float STAGE2_DEFAULT_TEMP_INT1 = 25.0;
  float STAGE2_DEFAULT_TEMP_EXT1 = 25.0;
  float STAGE2_DEFAULT_TEMP_EXT2 = 25.0;
  float STAGE2_OPERATING_TEMP_INT1 = 750.0;
  float STAGE2_OPERATING_TEMP_EXT1 = 750.0;
  float STAGE2_OPERATING_TEMP_EXT2 = 750.0;
  void outputReport(Stage2StatusReport *msr);
  void createJSONReport(Stage2StatusReport *msr, char *buffer);
};


#endif
