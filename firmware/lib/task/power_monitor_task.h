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

// power_monitor_task

#ifndef POWERMONITOR_TASK_H
#define POWERMONITOR_TASK_H


#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/array.h"
#include <machine.h>


#ifndef RIBBONFISH
//
#else
//
#endif


namespace OxApp
{
    // Runs the Pressure Swing Adsorption cycle
   // class HeartbeatTask : public OxCore::Task
    class PowerMonitorTask : public OxCore::Task
    {
    public:
    bool powerIsGood = false;
    int lowThreshold24V = 1023 * 3 / 4;

    bool setThreshold24V(int setThreshold);

    private:
      bool _init() override;
      bool _run() override;
      // This would go into the abstract class.

    };
}

#endif
