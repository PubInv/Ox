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

#include <Arduino.h>

#include <debug.h>
#include <network_task.h>
#include <stdio.h>
#include <string.h>
#include <PIRCS.h>
#include <machine.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "utility/w5100.h"
#include <network_udp.h>
#include <stage2_network_task.h>

// This is defined in network_udp.h. It is true global;
// hopefully it is only referenced here.
extern byte packetBuffer[buffMax];

using namespace OxCore;

namespace OxApp
{
  bool Stage2NetworkTask::_run()  {
    if (DEBUG_UDP > 1) {
      DebugLn<const char *>("Stage2NetworkTask was run\n");
    }
    NetworkTask::_run();

    char buffer[1024];
    // we need to make sure we start with a null string...
    buffer[0] = 0;
    getConfig()->createStage2JSONReport(getConfig()->s2heater,getConfig()->report,buffer);
    if (DEBUG_UDP > 0) {
      Debug<const char *>("Sending buffer:");
      DebugLn<const char *>(buffer);
    }
    unsigned long current_epoch_time = net_udp.epoch + millis() / 1000;
    // have to add a timeout here!
    net_udp.sendData(buffer, current_epoch_time, UDP_TIMEOUT);

    Serial.println("AAA");
  }
}
