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

#ifndef NETWORK_TASK_H
#define NETWORK_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif

#include <core.h>

#include <machine.h>
#include <network_udp.h>

namespace OxApp
{
  // Note this class is really a virtual; it makes no sense to
  // instantiated it directly, though it would give you network functionality
  class NetworkTask : public OxCore::Task {
  public:
    // DEBUG_UDP == 1 means debug logging,
    // DEBUG_UDP == 2 means debug script retreival
    int DEBUG_UDP = 0;
    NetworkUDP net_udp;
    bool _init() override;
    bool _run() override;
  };
}

#endif
