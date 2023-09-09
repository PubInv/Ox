/*

  temp_refresh_task.h -- adjust recent temp so that the actual temperature never gets too far

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

#ifndef TEMP_REFRESH_TASK_H
#define TEMP_REFRESH_TASK_H

#include <core_defines.h>
#include <core.h>

// Runs the Pressure Swing Adsorption cycle
  class TempRefreshTask : public OxCore::Task
  {
  public:
    TempRefreshTask();
    int DEBUG_TEMP_REFRESH = 0;
    const int PERIOD_MS = 5*60*1000;
    unsigned long time_of_last_refresh = 0;
    // In this case, we need a public ability to run the task
    void computeRefreshedTargetTemp(float t,MachineState ms,float temp_refresh_limit);
    bool run();
  private:
    bool _init() override;
    bool _run() override;
  };

#endif
