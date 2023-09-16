/*
  serial_input_task.h -- parse serial input to commands
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

#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#include <Arduino.h>
#include <core.h>
#include <debug.h>
#include <machine.h>
#include <stage2_hal.h>

#define SERIAL_BAUD_RATE 19200
// #define SERIAL_BAUD_RATE 9600
#define SERIAL_DELAY 1000
#define SERIAL_TIMEOUT_MAX 10000

#define INPUT_BUFFER_SIZE 256


namespace OxApp
{

  struct InputCommand {
    char com_c;
    float value_f;
  };

  class SerialInputTask : public OxCore::Task {
  public:
    int DEBUG_SERIAL = 2;
    char input_buffer[INPUT_BUFFER_SIZE];
    bool _init() override;
    bool _run() override;
    int DEBUG_LEVEL = 0;
    virtual bool listen(InputCommand &ic);
  };

  class OEDCSSerialTask : public SerialInputTask {
  public:
    int PERIOD_MS = 250;
    bool executeCommand(InputCommand ic);
    bool _init() override;
    bool _run() override;
  };

}

#endif
