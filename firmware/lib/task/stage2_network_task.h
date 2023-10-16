/*
Copyright (C) 2023 Robert Read, Geoff Mulligan.

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

#ifndef STAGE2_NETWORK_TASK_H
#define STAGE2_NETWORK_TASK_H

#include <Arduino.h>

#include <core.h>
#include <machine.h>
#include <network_udp.h>
#include <network_task.h>

namespace OxApp
{
  class Stage2NetworkTask : public NetworkTask {
  public:
    int PERIOD_MS =  20*1000;
    // We need all three configs to be able to report on all of them
    MachineConfig *mcs[3];
    bool _run() override;
  };
}

#endif
