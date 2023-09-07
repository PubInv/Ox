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
#include <stage2_hal.h>


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

void TempRefreshTask::computeRefreshedTargetTemp(float tmeasured,MachineState ms,float temp_refresh_limit) {
  float absolute_max_t =getConfig()->OPERATING_TEMP+getConfig()->OPERATING_TEMP_OVERTARGET_DELTA;
  float t_up = min(tmeasured,absolute_max_t);
  float t_dn = min(tmeasured,getConfig()->OPERATING_TEMP);

  if (getConfig()->ms == Warmup) {
    if (abs(t_up - getConfig()->GLOBAL_RECENT_TEMP) > getConfig()->TEMP_REFRESH_LIMIT) {
      getConfig()->BEGIN_UP_TIME_MS = millis();
      getConfig()->GLOBAL_RECENT_TEMP = t_up;
      getConfig()->TARGET_TEMP = t_up;
    }
  } else if (getConfig()->ms == Cooldown) {
    if (abs(t_dn - getConfig()->GLOBAL_RECENT_TEMP) > getConfig()->TEMP_REFRESH_LIMIT) {
      getConfig()->BEGIN_DN_TIME_MS = millis();
      getConfig()->GLOBAL_RECENT_TEMP = t_dn;
      getConfig()->TARGET_TEMP = t_dn;
    }
  }
}
bool TempRefreshTask::_run()
{
  if (DEBUG_TEMP_REFRESH > 0) {
    OxCore::Debug<const char *>("TempRefreshTask run\n");
    OxCore::Debug<int>(getConfig()->s2heater);
  }
  float t;
  if (!getConfig()->IS_STAGE2_HEATER_CONFIG) {
    // Whether this should be the max temperature is debatable.
    t = max(max(getConfig()->report->post_heater_C,
                      getConfig()->report->post_getter_C),
                  getConfig()->report->post_stack_C);
  } else {
    Stage2HAL* s2h = (Stage2HAL *)(getConfig()->hal);
    t =  s2h->
      getTemperatureReading(getConfig()->s2heater,
                            getConfig());
  }

  computeRefreshedTargetTemp(t,getConfig()->ms,getConfig()->TEMP_REFRESH_LIMIT);

  return true;
}
