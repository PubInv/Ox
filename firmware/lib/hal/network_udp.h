// Copyright (C) 2023 Robert Read.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.



#ifndef NETWORK_UDP_H
#define NETWORK_UDP_H

#define UDP_TIMEOUT 2000


// buffers for receiving and sending data
#define buffMax 64*1024


//Defines so the device can do a self reset
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)

class NetworkUDP  {
 public:
    int DEBUG_UDP = 0;
    unsigned long epoch = 0;

    bool sendData(char *data, unsigned long current_time, uint16_t timeout);
    // returns true if we are reading a packet

    void printTime(unsigned long time);
    bool getPacket();
    uint8_t networkCheck();
    void printPacketInfo(int packetsize);
    uint32_t setGlobalMacAddress();
    uint8_t networkStart();
    unsigned long getTime(uint16_t timeout);
    bool getParams(uint16_t timeout);
    int getcontrol();
    void printNet();
};

#endif
