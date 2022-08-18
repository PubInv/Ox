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

// This goal here is to run a PID controller
// to control the fan.

#ifndef FANPID_TASK_H
#define FANPID_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include <machine.h>

#include "model.h"
#include <PID_v1.h>


namespace OxApp
{
    // Runs the Pressure Swing Adsorption cycle
    class FanPIDTask : public OxCore::Task
    {
    public:
      FanPIDTask();
      PID *pidControllerFlow;
      // These are on a scale of 100
      double fanSpeed_Output = 0.0;
      double final_fanSpeed = 0.0;
      double Flow_Setpoint_mlps = 0.0;
      double Input_mlps = 0.0;

    private:
      bool _init() override;
      bool _run() override;
      // This would go into the abstract class.
    };
}

#endif
