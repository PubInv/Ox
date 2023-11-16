/*
 Copyright (C) 2022 Robert L. Read

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

#ifndef LOG_RECORDER_TASK_H
#define LOG_RECORDER_TASK_H


#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/circular_array.h"
#include <machine.h>
#include <machine_core_defs.h>

namespace OxApp
{

  class Log_Recorder_Task : public OxCore::Task
  {
  public:
    int DEBUG_LOG_RECORDER = 0;
    int recordCount();
  private:
    bool _init() override;
    bool _run() override;
    int _recordCount;
    //static OxCollections::CircularArray<Error, MAX_ERRORS> errors;
    //void addLog(MachineStatusReport *msr);

    //OxCollections::CircularArray * recordLog();
    //void clearLogs();
  };
}

#endif
