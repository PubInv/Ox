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

#ifndef _TASK_H
#define LOG_RECORDER_TASK_H


#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/circular_array.h"
#include <machine.h>





namespace OxApp
{
	
	enum class VariableName {
    Null = 0,
    LessThanZero,
    OutOfBounds,
    CoreFailedToAddTask,
    CoreFailedToBoot,
    CoreFailedToRun,
    TaskTimeExceeded,
    WatchdogExceeded,
    NotImplemented,
};

struct LogRecordEntry {
    LogRecordLevel level;
    LogRecordCode type;
	
};
    // Runs the Pressure Swing Adsorption cycle
    class HeartbeatTask : public OxCore::Task
    {
    public:
    private:
	  unsigned int  MAX_RECORDS = 600;
	  static OxCollections::CircularArray<LogRecordEntry, MAX_RECORDS> _log_entry;
      bool _init() override;
      bool _run() override;
      // This would go into the abstract class.

    };
}

#endif
