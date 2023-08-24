/*

  duty_cycle_task.h -- control an AC heater with an SSR

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

#ifndef DUTY_CYCLE_TASK_H
#define DUTY_CYCLE_TASK_H

#include <core_defines.h>
#include <core.h>
#include "GGLabsSSR1.h"

using namespace OxCore;

class DutyCycleTask : public OxCore::Task
  {
  public:
    DutyCycleTask();
    float dutyCycle;
    int NUM_HEATERS;
    GGLabsSSR1 **_ac_heaters;
    const int PERIOD_MS = 50;
    boolean isOn = false;
    float recorded_duty_cycle = 0;
    unsigned long recorded_dc_ms = 0;
    unsigned long time_of_last_check = 0;
    int DEBUG_DUTY_CYCLE = 0;
    void reset_duty_cycle();
  private:
    bool _init() override;
    bool _run() override;
  };


#endif
