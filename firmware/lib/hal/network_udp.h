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

// buffers for receiving and sending data
#define buffMax 32*1024

class NetworkUDP  {
 public:
    int DEBUG_UDP = 0;
    unsigned long epoch = 0;
    uint8_t networkDown = 1;

    bool sendData(char *data, unsigned long current_time, uint16_t timeout);
    // returns true if we are reading a packet

    void printTime(unsigned long time);
    bool getPacket();
    uint8_t networkCheck();
    void printPacketInfo(int packetsize);
    uint8_t networkStart();
    unsigned long getTime(uint16_t timeout);
    bool getParams(uint16_t timeout);
    void printNet();
};

#endif
