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

        return true;
    }

    bool Log_Recorder_Task::_run()
    {
		
			addLog(MachineStatusReport *msr);
	        return true;
    }	
	void addMSR(MachineStatusReport *);
	
    void Log_Recorder_Task::addLog(MachineStatusReport *msr) {
    LogRecordEntry lre = {
        mrs->report->post_heater_C, \
		mrs->fanDutyCycle, 	\
		mrs->CriticalError errors[NUM_CRITICAL_ERROR_DEFINITIONS],	\
		mrs->RAMP_UP_TARGET_D_MIN ,\/* R (degrees C per minute)*/
		mrs->RAMP_DN_TARGET_D_MIN , \/* R (degrees C per minute)*/
		mrs->TARGET_TEMP_C , \/* This is the goal target*/
		mrs->MAX_AMPERAGE , \/* A (Amperes)*/
		mrs->MAX_STACK_WATTAGE , \/* W (Wattage)*/
		mrs->FAN_SPEED , \/* F (fraction between 0.0 and 1.0)*/
		mrs->BEGIN_DN_TIME_MS , \
		mrs->BEGIN_UP_TIME_MS , \
		mrs->SETPOINT_TEMP_C , /*This is the CURRENT setpoint, which ramps up or down to TARGET_TEMP.*/

  // These are bounds; we won't let values go outside these.
  // They can only be changed here and forcing a recompilation.
  //const float BOUND_MAX_TEMP = 750.0;
  //const float BOUND_MIN_TEMP = 25.0;
  //const float BOUND_MAX_AMPERAGE_SETTING = 60.0;
  //const float BOUND_MAX_WATTAGE = 300.0;
  //const float BOUND_MAX_RAMP = 3.0;

  // The beginning temperature of the current warming
  // or cooling cycle.
		mrs->COOL_DOWN_BEGIN_TEMP,
		mrs->WARM_UP_BEGIN_TEMP,


  //const unsigned long THERMOCOUPLE_FAULT_TOLERATION_TIME_MS = 2 * 60 * 1000;

  // TODO: This would better be attached to the statemanager
  // class, as it is used in those task---but also in the
  // separate temp_refresh_task. Until I can refactor
  // temp_refresh_task by placing its funciton in the
  // state manager, I need this gloabl.
		mrs->GLOBAL_RECENT_TEMP,

  //const float MAX_STACK_VOLTAGE = 12.0;
  //static constexpr float IDLE_STACK_VOLTAGE = 1.0;
  //static constexpr float MIN_OPERATING_STACK_VOLTAGE = 1.0;
  //
  // Note: The MAX31850, OneWire system, and the MAX31855, both,
  // can not read reliably faster than 100ms.
  // We have tested the TEMP_READ_PERIOD_MS at 100,
  // but see no reason for it to be that fast.
  // At present the code does not really use or log readings
  // that are faster than the heater PID task period, so
  // there is no reason for it to be more than twice as fast as that.
  // Please refer to the documentation here:
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31850-MAX31851.pdf
  // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31855.pdf
  // Please make sure that the INIT_PID_PERIOD_MS is more than
  // the TEMP_READ_PERIOD_MS.
  //static const int TEMP_READ_PERIOD_MS = 225; // this is intentionally a little less than half the PID PERIOD
  //static const int INIT_PID_PERIOD_MS = 500;

  // static const int INIT_HEARTBEAT_PERIOD_MS = 500;
 
 // static const int NUM_MACHINE_STATES = 8;



  mrs-> ms;
  // This is used to make decisions that happen at transition time.
  mrs-> previous_ms;
  //bool IS_STAGE2_HEATER_CONFIG = false;
 // Stage2Heater s2heater;

  //MachineScript* script;

   mrs-> idleOrOperate ;

  // This is a range from 0.0 to 1.0!
  // However, when used in the Arduino it has to be mapped
  // onto a an integer (usually 0-255) but this should be
  // the last step.
  mrs-> fanDutyCycle;

  MachineHAL* hal;
  MachineStatusReport *report;

  mrs-> Ddelta_C_per_min;
		
	};  // LogRecordEntry lre 
    _log_entry.add(lre);
/* 
#ifdef ARDUINO
            OxCore::Debug<const char *>(ErrorLevelText[static_cast<int>(level)]);
            OxCore::Debug<const char *>(": ");
            OxCore::DebugLn<const char *>(ErrorMessage[static_cast<int>(type)]);
#else
            std::cout << ErrorMessage[static_cast<int>(type)] << std::endl;
#endif */
/*         break;
        default:

        break;
    }
    if (level == ErrorLevel::Critical) {
        Core::RaiseCriticalError();
    } */
}


	  void Log_Recorder_Task::clearLogs(){
		  
	  };
}
