/*

  temp_refresh_ttask.cpp -- adjust recent temp so that the actual temperature never gets too far

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

#include <temp_refresh_task.h>


TempRefreshTask::TempRefreshTask() {

  }

  bool TempRefreshTask::_init()
  {

    OxCore::Debug<const char *>("TempRefreshTask init\n");
    return true;
  }

bool TempRefreshTask::run() {
  _run();
}
bool TempRefreshTask::_run()
{
  if (DEBUG_TEMP_REFRESH > 0) {
    OxCore::Debug<const char *>("TempRefreshTask run\n");
  }
  float t = max(max(getConfig()->report->post_heater_C,
                    getConfig()->report->post_getter_C),
                getConfig()->report->post_stack_C);

  if (getConfig()->ms == Warmup) {
    time_of_last_refresh = millis();

    float max_t =MachineConfig::OPERATING_TEMPERATURE+MachineConfig::OPERATING_TEMPERATURE_OVERTARGET_DELTA;
    t = min(t,max_t);

    if (abs(t - getConfig()->RECENT_TEMPERATURE) > getConfig()->TEMP_REFRESH_LIMIT) {
      getConfig()->BEGIN_UP_TIME_MS = time_of_last_refresh;
      getConfig()->RECENT_TEMPERATURE = t;
      getConfig()->TARGET_TEMP = t;
    }


  } else if (getConfig()->ms == Cooldown) {
    t = min(t,MachineConfig::OPERATING_TEMPERATURE);

    time_of_last_refresh = millis();
    if (abs(t - getConfig()->RECENT_TEMPERATURE) > getConfig()->TEMP_REFRESH_LIMIT) {
      getConfig()->BEGIN_UP_TIME_MS = time_of_last_refresh;
      getConfig()->COOL_DOWN_BEGIN_TEMPERATURE = t;
      getConfig()->TARGET_TEMP = t;
    }
  }

  return true;
}
