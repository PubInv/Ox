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
#include <retrieve_script_UDP_task.h>
#include <stdio.h>
#include <string.h>
#include <PIRCS.h>
#include <machine.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "utility/w5100.h"
#include <network_udp.h>

// This is defined in network_udp.h. It is true global;
// hopefully it is only referenced here.
extern byte packetBuffer[buffMax];

// char timeServer[] = "time.nist.gov";
// char mcogs[] = "mcogs.coslabs.com";

// // #define serverPort 2390
// #define serverPort 57573



// #define UDP_TIMEOUT 2000

// #define FLASH_ACCESS_MODE_128    EFC_ACCESS_MODE_128
// #define FLASH_ACCESS_MODE_64     EFC_ACCESS_MODE_64



// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// This is the mac for ROB READs Due in Austin TX! Change if you are installing elsewhere
// until we can get dynamic mac addresss solved.
//byte mac[] = {
//   0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
//};

using namespace OxCore;

// TODO: Move this on to the network_udp object
uint8_t networkDown = 1;


namespace OxApp
{

  bool RetrieveScriptUDPTask::_init() {

    for (uint8_t i = 0; i < 10 && networkDown; i++) {
      switch(net_udp.networkStart()) {
      case 0: networkDown = 0; break;
      case 1: Serial.println("W5xx init failed"); break;
      case 2: Serial.println("No ethernet boad"); break;
      case 3: Serial.println("No link"); break;
      case 4: Serial.println("No DHCP"); break;
      }
    }

    if (networkDown) {
      // Be sure to call safeDelay or watchdogReset
      while(1) {
        Serial.println("CRITICAL ERROR! CONFIGURED FOR ETHERNET, BUT NONE FOUND!");
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

    Serial.println("Network started");
    Serial.println();
    Serial.println();
    return true;
  }
  bool RetrieveScriptUDPTask::_run()  {
    if (DEBUG_UDP > 1) {
      DebugLn<const char *>("The RetrieveScriptUDPTask was run\n");
    }

    switch(net_udp.networkCheck()) {
    case 1:
    case 2: Serial.println("Lost network link"); networkDown++; break;
    case 3: Serial.println("Lost IP address"); networkDown++; break;
    case 100: networkDown = 0;
      break;
    }

    // This is the (currently unused) retrieval of scripts to set parameters
    bool new_packet = net_udp.getPacket();
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
    // have to add a timeout here!
    net_udp.sendData(buffer,current_epoch_time, UDP_TIMEOUT);
  }


}
