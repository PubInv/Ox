/*
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

#include "log_recorder_task.h"
#include <core.h>
using namespace std;


namespace OxApp
{
    bool Log_Recorder_Task::_init()
    {
        OxCore::Debug<const char *>("LogRecorderTask init\n");
		_recordCount = 0;
        return true;
    }

    bool Log_Recorder_Task::_run()
    {
		OxCore::Debug<const char *>("LogRecorderTask run: ");
		OxCore::DebugLn<int>(_recordCount);
		getConfig()->report->timestamp = millis();
		getConfig()->_log_entry[_recordCount] = *getConfig()->report;
		/*testing log recorder*/
		_recordCount++;
		if(_recordCount>getConfig()->MAX_RECORDS)
		{
				_recordCount = 0;
				getConfig()->dumpAllData10Hz();
		}
		/*end test*/
	        return true;
    }	
	int Log_Recorder_Task::recordCount()
	{
		return _recordCount;
	}
    void Log_Recorder_Task::clearLogs(){
	    _recordCount = 0; 
    }; 
}
