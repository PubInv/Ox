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
#include <OnePinHeater.h>

using namespace OxCore;

/*
  This whole class might seem like overkill;
  why do something so complicated that resemble's
  Bresenham's line drawing algorithm when PWM would work?
  SSR only turn off on a zero-crossing (usually). Since
  the AC frequency is 60 Hz, this is MUCH slower than
  our PWM frequency of 2500 Hz typical of an Arduino.
  It is very unclear to me what would happen if you
  tried to control an SSR with a PWM signal. - rlr
 */

class DutyCycleTask : public OxCore::Task
  {
  public:
    DutyCycleTask();
    Stage2Heater whichHeater;
    float dutyCycle;
    const int PERIOD_MS = 50;
    boolean isOn = false;
    float recorded_duty_cycle = 0;
    unsigned long recorded_dc_ms = 0;
    unsigned long time_of_last_check = 0;
    int DEBUG_DUTY_CYCLE = 0;
    void reset_duty_cycle();
    // By placing a pointer here, we can have
    // individual control of however many heaters are in the system.
    OnePinHeater *one_pin_heater;
  private:
    bool _init() override;
    bool _run() override;
  };


#endif
