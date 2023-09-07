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


#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif



#include <debug.h>
#include <retrieve_script_UDP_task.h>
#include <stdio.h>
#include <string.h>
#include <PIRCS.h>
#include <machine.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008




char timeServer[] = "time.nist.gov";
char mcogs[] = "mcogs.coslabs.com";

// #define localPort 2390
#define localPort 57573

#define UDP_TIMEOUT 2000

unsigned long epoch = 0; // global to hold time

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
// This is the mac for ROB READs Due in Austin TX! Change if you are installing elsewhere
// until we can get dynamic mac addresss solved.
//byte mac[] = {
//   0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
//};

byte mac[] = {
   0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFF
};
//byte mac[] = {
//   0xD4, 0x3D, 0x39, 0x39, 0x69, 0x5a
//};

char macString[20];

IPAddress ip(192, 168, 1, 177);

// buffers for receiving and sending data
#define buffMax 64*1024
byte packetBuffer[buffMax]; //buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
#define W5200_CS  10
#define SDCARD_CS 4


using namespace OxCore;
#define DEBUG_RETRIEVE_SCRIPT_UDP_TASK 1


namespace OxApp
{

  bool RetrieveScriptUDPTask::_init() {
    // start the Ethernet and UDP:
    //  pinMode(SDCARD_CS, OUTPUT);
    //  digitalWrite(SDCARD_CS, HIGH); //Deselect the SD card

    Serial.println(F("starting"));

    Ethernet.begin(mac);
    Udp.begin(localPort);

    sprintf(macString, "%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    printNet();
    delay(1000);

    Serial.println("BBB");
    for (int i = 10; epoch == 0 && i > 0; i--) {
      epoch = getTime();
      delay(500);
    }
    Serial.println("CCC");
    if (epoch == 0) Serial.println(F("Can't get time!"));
    else printTime(epoch);

    // I'm sure this can fail in a variety of ways, but we haven't done a good job
    // figuring that out...at present this is a "happy path" solution
    return true;
  }

  bool RetrieveScriptUDPTask::_run()  {
    if (DEBUG_UDP > 1) {
      DebugLn<const char *>("The RetrieveScriptUDPTask was run\n");
    }
    bool new_packet = getPacket();
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
    unsigned long current_epoch_time = epoch + millis() / 1000;
    sendData(buffer,current_epoch_time);
  }


  void RetrieveScriptUDPTask::printPacketInfo(int packetsize) {
    if (DEBUG_UDP > 1) {
      Serial.print(F("UDP Packet received, size "));
      Serial.println(packetsize);
      Serial.print(F("From "));
    }
    IPAddress remoteIp = Udp.remoteIP();
    if (DEBUG_UDP > 1) {
      Serial.print(remoteIp);
      Serial.print(F(", port "));
      Serial.println(Udp.remotePort());
    }
  }

  void RetrieveScriptUDPTask::printTime(unsigned long time) {
    Serial.print(F("Unix time = "));
    Serial.println(time);
    // print the hour, minute and second:
    Serial.print(F("The UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((time  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(F(":"));

    // In the first 10 minutes of each hour, we'll want a leading '0'
    if (((time % 3600) / 60) < 10) Serial.print(F("0"));

    // print the minute (3600 equals secs per minute)
    Serial.print((time  % 3600) / 60);
    Serial.print(F(":"));

    // In the first 10 seconds of each minute, we'll want a leading '0'
    if ((time % 60) < 10) Serial.print(F("0"));

    Serial.println(time % 60); // print the second
  }

  // send an NTP request to the time server at the given address
  unsigned long RetrieveScriptUDPTask::getTime() {
    Serial.println("AAA");
#define NTP_PACKET_SIZE 48
    byte ntpBuffer[NTP_PACKET_SIZE];

    memset(ntpBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in buffer to 0

    // Initialize values needed to form NTP request
    ntpBuffer[0] = 0b11100011;   // LI, Version, Mode
    ntpBuffer[1] = 0;     // Stratum, or type of clock
    ntpBuffer[2] = 6;     // Polling Interval
    ntpBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    ntpBuffer[12]  = 49;
    ntpBuffer[13]  = 0x4E;
    ntpBuffer[14]  = 49;
    ntpBuffer[15]  = 52;

    Serial.println("QQQQ");
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(timeServer, 123); //NTP requests are to port 123
    Serial.println("ZZZZZZ");
    Udp.write(ntpBuffer, NTP_PACKET_SIZE);
    Serial.println("MMMMMMM");
    Udp.endPacket();
    Serial.println("NNNNNNNN");
    delay(100);

    unsigned long startMs = millis();
    // Note: This is a hard loop --- UDP_TIMEOUT blocks the machine for that time.
    Serial.println("BEGINNING");
    delay(100);
    while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {
      Serial.println("x");
      delay(100);
      Serial.println(millis());
      delay(100);
    }

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();

    if (packetSize) {
      //    printPacketInfo(packetSize);

      Udp.read(ntpBuffer, NTP_PACKET_SIZE);

      // the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(ntpBuffer[40], ntpBuffer[41]);
      unsigned long lowWord = word(ntpBuffer[42], ntpBuffer[43]);

      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;

      // now convert NTP time into everyday time:
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      return secsSince1900 - seventyYears;
    }
    return 0;
  }

  void RetrieveScriptUDPTask::sendData(char *data,unsigned long current_time) {
    Udp.beginPacket(mcogs, 57573);
    Udp.write("PUT ", 4);
    Udp.write("/", 1);
    Udp.write(macString, 17);
    Udp.write("/", 1);
    Udp.write("Data\n", 5);

    Udp.write("{ \"TimeStamp\": ", 15);
    char ts[15];
    sprintf(ts, "%ld", current_time);
    Udp.write(ts, strlen(ts));
    Udp.write(",\n", 2);
    if (data && strlen(data)) Udp.write(data, strlen(data));
    Udp.write("}", 1);
    Udp.endPacket();

    unsigned long startMs = millis();
    while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}
  }

  bool RetrieveScriptUDPTask::getPacket() {
    Udp.beginPacket(mcogs, 57573);
    Udp.write("GET ", 4);
    Udp.write("/", 1);
    Udp.write(macString, 17);
    Udp.write("/", 1);
    Udp.write("Params\n", 7);
    Udp.endPacket();

    unsigned long startMs = millis();
    while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}
    if (DEBUG_UDP > 0) {
      Serial.print("Udp.available() = ");
      Serial.println(Udp.available());
    }
    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();

    if (packetSize) {
      if (DEBUG_UDP > 1) {
        Serial.print(F("UDP Packet received, size "));
        Serial.println(packetSize);
        Serial.print(F("From "));
      }
      IPAddress remoteIp = Udp.remoteIP();
      if (DEBUG_UDP > 1) {
        Serial.print(remoteIp);
        Serial.print(F(", port "));
        Serial.println(Udp.remotePort());
      }

      // We've received a packet, read the data from it into the buffer
      //    byte b[1024];
      Udp.read(packetBuffer, packetSize);
      packetBuffer[packetSize] = '\0';

      String config = String((char *)packetBuffer);
      if (DEBUG_UDP > 1) {
        Serial.println(config);
      }
      // Now, at this point, we roughly have a new script.
      // so we will parse it and poke it into the machine script as
      // a single pointer switch.
      return true;
    } else {
      return false;
    }
  }

  char cmd[1024];
  int lastnonce = 0;

  int RetrieveScriptUDPTask::getcontrol() {
    unsigned long startMs = millis();
    while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

    // if there's data available, read a packet
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      //    printPacketInfo(packetSize);
      // Changing on a supposition that this line is an error - rlr
      // Udp.read(buffer, packetSize);
      Udp.read(packetBuffer, packetSize);
    }

  }

  void RetrieveScriptUDPTask::printNet() {
    Ethernet.MACAddress(mac);
    Serial.print(F("The MAC address is: "));
    for (byte octet = 0; octet < 6; octet++) {
      if (DEBUG_UDP > 1) {
        Serial.println(octet);
        delay(100);
        //        Serial.print(mac[octet], HEX);
        Serial.println(mac[octet], HEX);
        delay(100);
      }
      if (octet < 5) {
        Serial.print(F(':'));
      }
    }
    if (DEBUG_UDP > 1) {
      Serial.println();
      Serial.print(F("IP address: "));
      Serial.println(Ethernet.localIP());
      Serial.print(F("Subnet Mask: "));
      Serial.println(Ethernet.subnetMask());
      Serial.print(F("Gateway: "));
      Serial.println(Ethernet.gatewayIP());
      Serial.print(F("DNS Server: "));
      Serial.println(Ethernet.dnsServerIP());
    }
  }


}
