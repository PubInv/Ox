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
	MachineState,
	TargetC,
	SetpointC,
	RampC,
	MaxStackA,
	MaxStackW,
	FanPWM,
	HeaterC,
	StackC,
	GetterC,
	HeaterDutyCycle,
	StackA,
	StackW,
	StackV,
	StackOhms,
	StackOhms,
	FanRPM,

};

struct LogRecordEntry {
 //   LogRecordLevel level;
//    LogRecordCode type,
	float target_temp,
    float setpoint_temp,
    float measured_temp,
                          float heater_duty_cycle,
                          float ramp_C_per_min;
						  
						  
	
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

    void addLog(MachineStatusReport *msr);

  void outputLogReport(MachineStatusReport *msr);
  void createLogJSONReport(MachineStatusReport *msr, char *buffer);
 void clearLogs();
    };
}

#endif
