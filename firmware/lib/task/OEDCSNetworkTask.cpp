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
#include <OEDCSNetworkTask.h>


// This is defined in network_udp.h. It is true global;
// hopefully it is only referenced here.
extern byte packetBuffer[buffMax];

using namespace OxCore;

// TODO: Move this on to the network_udp object
// uint8_t networkDown = 1;

namespace OxApp
{
  bool OEDCSNetworkTask::_run()  {
    NetworkTask::_run();

    // This is the (currently unused) retrieval of scripts to set parameters
    bool new_packet = NetworkTask::net_udp.getParams(3000);
    if (new_packet) {
      // This would be better done with a static member
      MachineScript *old = getConfig()->script;
      MachineScript *ms = old->parse_buffer_into_new_script((char *) packetBuffer);
      getConfig()->script = ms;
      delete old;
    }

    // This is a preliminary data loggging test. There is no reason
    // that the datalogging should be done at the frequency as checking
    // for a new script, but for now we will keep here rather than
    // creating a new task that we could schedule separately.
    getConfig()->outputReport(getConfig()->report);
    char buffer[1024];
    // we need to make sure we start with a null string...
    buffer[0] = 0;
    getConfig()->createJSONReport(getConfig()->report,buffer);
    if (DEBUG_UDP > 0) {
      Debug<const char *>("Sending buffer:");
      DebugLn<const char *>(buffer);
    }
    unsigned long current_epoch_time = net_udp.epoch + millis() / 1000;
    net_udp.sendData(buffer,current_epoch_time, UDP_TIMEOUT);
  }

}
