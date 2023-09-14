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

#ifndef RETRIEVE_SCRIPT_UDP_TASK_H
#define RETRIEVE_SCRIPT_UDP_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif

#include <core.h>

#include <machine.h>

namespace OxApp
{

  class RetrieveScriptUDPTask : public OxCore::Task {
  private:
  public:
    // DEBUG_UDP == 1 means debug logging,
    // DEBUG_UDP == 2 means debug script retreival
    int DEBUG_UDP = 0;

    char cmd[1024];
    int lastnonce = 0;


    bool _init() override;
    bool _run() override;
    void printPacketInfo(int packetsize);
    void printTime(unsigned long time);

// send an NTP request to the time server at the given address

    // TODO: Refactor these into their own address
    bool sendData(char *data, unsigned long current_time, uint16_t timeout);
    // returns true if we are reading a packet
    bool getPacket();

    uint32_t setGlobalMacAddress();
    uint8_t networkStart();
    unsigned long getTime(uint16_t timeout);
    bool getParams(uint16_t timeout);
    int getcontrol();
    void printNet();
  };
}

#endif
