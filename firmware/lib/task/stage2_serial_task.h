/*
  Copyright (C) 2023 Robert L. Read

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

#ifndef STAGE2_SERIAL_TASK_H
#define STAGE2_SERIAL_TASK_H

#include <core.h>
#include <serial_task.h>
#include <machine.h>

namespace OxApp
{

class Stage2SerialTask : public AbstractSerialTask {
  public:
    // Stage2 consists of three separate Heating machines,
    MachineConfig *machineConfigs[3];
    // There is only HAL, and we need it for configuration
    // switching
    Stage2HAL *hal;

    MachineConfig *getConfig(int i);
    bool one_char_command_found(int num_read, char buffer[], int k) override;
    bool _init() override;
    bool _run() override;
  };


}

#endif
