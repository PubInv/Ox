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
#include <flash.h>

// This is defined in network_udp.h. It is true global;
// hopefully it is only referenced here.
extern byte packetBuffer[buffMax];

using namespace OxCore;

namespace OxApp
{

  bool NetworkTask::_init() {

    for (uint8_t i = 0; i < 10 && net_udp.networkDown; i++) {
      switch(net_udp.networkStart()) {
      case 0: net_udp.networkDown = 0; break;
      case 1: Serial.println(F("W5x00 init failed")); break;
      case 2: Serial.println(F("No ethernet boad")); break;
      case 3: Serial.println(F("No link")); break;
      case 4: Serial.println(F("No DHCP")); break;
      }
    }

    if (net_udp.networkDown) {
      // Be sure to call safeDelay or watchdogReset
      while(1) {
        Serial.println(F("CRITICAL ERROR! CONFIGURED FOR ETHERNET, BUT NONE FOUND!"));
        watchdogReset();
        delay(5000);
      // WARNING --- there is a danger that this
      // prevents the system from coming up at all
      // if we have not connectivity...that might not be
      // the best behavior, but by our current understanding
      // it is safe.
      // REQUEST_EXTERNAL_RESET ; //this will reset processor
      }
    }

    Serial.println(F("Network started"));
    Serial.println();
    Serial.println();

    unsigned long current_epoch_time = net_udp.epoch + millis() / 1000;
    char buffer[1024];
    strcpy(buffer, "\"MachineStart\": ");
    switch(getResetCause()) {
    case 0: strcat(buffer, "\"GENERAL\""); break;
    case 1: strcat(buffer, "\"BACKUP\""); break;
    case 2: strcat(buffer, "\"WATCHDOG\""); break;
    case 3: strcat(buffer, "\"SOFTWARE\""); break;
    case 4: strcat(buffer, "\"USER\""); break;
    }
    net_udp.sendData(buffer, current_epoch_time, 2000);

    return true;
  }

  bool NetworkTask::_run()  {
    if (DEBUG_UDP > 1) {
      DebugLn<const char *>("The NetworkUDPTask was run\n");
    }

    switch(net_udp.networkCheck()) {
    case 1:
    case 2:
      Serial.println(F("Lost network link"));
      net_udp.networkDown++;
      break;
    case 3:
      Serial.println("Lost IP address");
      net_udp.networkDown++;
      break;
    case 100: net_udp.networkDown = 0;
      break;
    }
  }
}
