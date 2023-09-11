#define ETHERNET_BOARD_PRESENT 1
// #define ETHERNET_BOARD_PRESENT 0 //No ethernet.

#define BAUDRATE 9600

#define DEBUG_UDP 2
#define FLASH_ACCESS_MODE_128    EFC_ACCESS_MODE_128
#define FLASH_ACCESS_MODE_64     EFC_ACCESS_MODE_64

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif


#include <stdio.h>
#include <string.h>

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include "utility/w5100.h"

char timeServer[] = "time.nist.gov";
char mcogs[] = "mcogs.coslabs.com";

// #define localPort 2390
#define serverPort 57573

uint8_t networkDown = 1;

unsigned long epoch = 0; // global to hold time

byte mac[6];
char macString[20];

// buffers for receiving and sending data
#define buffMax 64*1024
byte packetBuffer[buffMax]; //buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
#define W5200_CS  10
#define SDCARD_CS 4

//Defines so the device can do a self reset
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)

#define WATCH_DOG_TIME 2000
//void watchdogSetup() {
//  watchdogEnable(WATCH_DOG_TIME);
//}

void
printPacketInfo(int packetsize) {
  Serial.print(F("UDP Packet received, size "));
  Serial.println(packetsize);
  Serial.print(F("From "));
  IPAddress remoteIp = Udp.remoteIP();
  Serial.print(remoteIp);
  Serial.print(F(", port "));
  Serial.println(Udp.remotePort());
}

void
printTime(unsigned long time) {
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

uint8_t
networkCheck() {
  switch(Ethernet.linkStatus()) {
  case Unknown: return 1; break;
  case LinkON: break;
  case LinkOFF: return 2; break;
  }

  switch (Ethernet.maintain()) {
  case 0: break; // nothing
  case 1: return 3; // renewal failed
  case 2: break; // renew success
  case 3: return 3; // rebind failed
  case 4: break; // rebind success
  }

  return 100;
}

// send an NTP request to the time server at the given address
unsigned long
getTime(uint16_t timeout) {
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
  
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  if (!Udp.beginPacket(timeServer, 123)) { //NTP requests are to port 123
    if (DEBUG_UDP > 2) Serial.println("can't resolve timeserver");
    return 0;
  }
  Udp.write(ntpBuffer, NTP_PACKET_SIZE);
  if (!Udp.endPacket()) {
    if (DEBUG_UDP > 2) Serial.println("Could not send time request");
    return 0;
  }
  // Note: This is a hard loop --- UDP_TIMEOUT blocks the machine for that time.
  unsigned long startMs = millis();
  int packetSize = 0;
  while ((packetSize = Udp.parsePacket()) == 0 && (millis() - startMs) < timeout) { 
    delay(100);
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) Serial.println("No time data returned");
    return 0;
  }
  
  if (DEBUG_UDP > 1) printPacketInfo(packetSize);

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

void
printNet() {
  //  Ethernet.MACAddress(mac);
  Serial.print(F("The MAC address is: "));
  for (uint8_t i = 0; i < 6; i++) {
    if (mac[i] < 10) Serial.print(F("0"));
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(F(":"));
  }
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

bool
getParams(uint16_t timeout) {
  if (! Udp.beginPacket(mcogs, 57573)) {
    if (DEBUG_UDP > 2) Serial.println("can't resolve mcogs");
    return false;
  }
  Udp.write("GET ", 4);
  Udp.write("/", 1);
  Udp.write(macString, 17);
  Udp.write("/", 1);
  Udp.write("Params\n", 7);
  if (! Udp.endPacket()) {
    if (DEBUG_UDP > 2) Serial.println("can't send request");
    return false;
  }
  
  unsigned long startMs = millis();
  int packetSize = 0;
  while ((packetSize = Udp.parsePacket()) == 0 && (millis() - startMs) < timeout) {
    delay(50);
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) Serial.println("no params returned");
    return false;
  }
  
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
}

bool
sendData(char *data, unsigned long current_time, uint16_t timeout) {
  if (! Udp.beginPacket(mcogs, serverPort)) {
    if (DEBUG_UDP > 2) Serial.println("Can't resolve mcogs");
    return false;
  }

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
  if (!Udp.endPacket()) {
    if (DEBUG_UDP > 2) Serial.println("Can't send data");
    return false;
  }
  
  unsigned long startMs = millis();
  int packetSize = 0;
  while ((packetSize = Udp.parsePacket()) == 0 && (millis() - startMs) < timeout) { 
    delay(100);
    watchdogReset();
  }

  if (!packetSize) {
    if (DEBUG_UDP > 2) Serial.println("no response");
    return false;
  }

  Udp.read(packetBuffer, packetSize);
  packetBuffer[packetSize] = '\0';
  if (strncmp((char *)packetBuffer, "posted", 6)) return false;
  if (DEBUG_UDP > 2) Serial.println((char *)packetBuffer);
  return true;
}

uint32_t
setGlobalMacAddress() {
  uint32_t rv = efc_init((Efc *) EFC0, FLASH_ACCESS_MODE_128, 4);
  if (rv != EFC_RC_OK) return rv;

  uint32_t uid_buf[4];
  rv = efc_perform_read_sequence((Efc *)EFC0, EFC_FCMD_STUI, EFC_FCMD_SPUI, uid_buf, 4);
  if (rv != EFC_RC_OK) return EFC_RC_ERROR;

   //  Serial.print(F(" ID = "));
   //  Serial.print(uid_buf[0]);Serial.print(",");
   //  Serial.print(uid_buf[1]);Serial.print(",");
   //  Serial.print(uid_buf[2]);Serial.print(",");
   //  Serial.println(uid_buf[3]);

  uint32_t hash32 = uid_buf[0];
  hash32 ^= uid_buf[1];
  hash32 ^= uid_buf[2];
  hash32 ^= uid_buf[3];
  
  mac[0] = 0xFE;
  mac[1] = 0xED;
  mac[2] = (hash32>>24) & 0xFF;
  mac[3] = (hash32>>16) & 0xFF;
  mac[4] = (hash32>>8) & 0xFF;
  mac[5] = (hash32>>0) & 0xFF;

  sprintf(macString, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print(F("MAC Address: "));
  Serial.println(macString);
  return 0;
}

uint8_t
networkStart() {
  Ethernet.init(10);

  if (W5100.init() == 0) return 1;
  
  if (!Ethernet.hardwareStatus()) return 2;

  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac);
  uint32_t add = 0;
  W5100.setIPAddress((uint8_t *) &add);
  SPI.endTransaction();
  
  uint32_t startMs = millis();
  while (W5100.getLinkStatus() != LINK_ON && (millis() - startMs) < 3000) {
    delay(100);
    watchdogReset();
  }

  if (W5100.getLinkStatus() != LINK_ON) return 3;
  
  if (setGlobalMacAddress()) {
    mac[0] = 0xFE;
    mac[1] = 0xED;
    mac[2] = 0x03;
    mac[3] = 0x04;
    mac[4] = 0x05;
    mac[5] = 0x06;
  }

  if (Ethernet.begin(mac, 10000, 3000) == 0) return 4;

  Udp.stop();
  if (!Udp.begin(2390)) return 5;

  if (DEBUG_UDP > 1) printNet();

  for (int i = 10; epoch == 0 && i > 0; i--) {
    epoch = getTime(2000);
    delay(500);
  }

  printTime(epoch);

  sendData("\"HELLO\": 1", epoch, 2000);
  
  return 0;
}
  
void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial) {
    watchdogReset();
  }
  Serial.println(F("starting"));

  networkDown = 1;
  for (uint8_t i = 0; i < 10 && networkDown; i++) {
    switch(networkStart()) {
    case 0: networkDown = 0; break;
    case 1: Serial.println("W5xx init failed"); break;
    case 2: Serial.println("No ethernet boad"); break;
    case 3: Serial.println("No link"); break;
    case 4: Serial.println("No DHCP"); break;
    }
  }

  if (networkDown) REQUEST_EXTERNAL_RESET ; //this will reset processor

  Serial.println("Network started");
  Serial.println();
  Serial.println();
  
  return;
}

void loop() {
  delay(1000);
  epoch += 1;
  printTime(epoch);

  switch(networkCheck()) {
  case 1: 
  case 2: Serial.println("Lost network link"); networkDown++; break;
  case 3: Serial.println("Lost IP address"); networkDown++; break;
  case 100: networkDown = 0; break;
  }
  
  if (networkDown >= 10) REQUEST_EXTERNAL_RESET ; //this will reset processor

  if (epoch > 0 && epoch % 15 == 0) {
    Serial.println(F("Get time"));
    epoch = getTime(2000);
  }
}
