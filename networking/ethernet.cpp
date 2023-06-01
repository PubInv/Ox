#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

char timeServer[] = "time.nist.gov";
char mcogs[] = "mcogs.coslabs.com";

#define localPort 2390
#define UDP_TIMEOUT 2000

unsigned long epoch = 0; // global to hold time

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
char macString[20];

IPAddress ip(192, 168, 1, 177);

// buffers for receiving and sending data
#define buffMax 64*1024
byte packetBuffer[buffMax]; //buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
#define W5200_CS  10
#define SDCARD_CS 4

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

// send an NTP request to the time server at the given address
unsigned long
getTime() {
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
  Udp.beginPacket(timeServer, 123); //NTP requests are to port 123
  Udp.write(ntpBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();

  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

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

void
sendData(char *data) {
  Udp.beginPacket(mcogs, 57573);
  Udp.write("PUT ", 4);
  Udp.write("/", 1);
  Udp.write(macString, 17);
  Udp.write("/", 1);
  Udp.write("Data\n", 5);

  Udp.write("{ \"TimeStamp\": ", 15);
  char ts[15];
  sprintf(ts, "%ld", epoch);
  Udp.write(ts, strlen(ts));
  if (data && strlen(data)) Udp.write(data, strlen(data));
  Udp.write("}", 1);
  Udp.endPacket();

  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}
}

void getConfig() {
  Udp.beginPacket(mcogs, 57573);
  Udp.write("GET ", 4);
  Udp.write("/", 1);
  Udp.write(macString, 17);
  Udp.write("/", 1);
  Udp.write("Params\n", 7);
  Udp.endPacket();

  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();

  if (packetSize) {
    Serial.print(F("UDP Packet received, size "));
    Serial.println(packetSize);
    Serial.print(F("From "));
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(F(", port "));
    Serial.println(Udp.remotePort());

    // We've received a packet, read the data from it into the buffer
    //    byte b[1024];
    Udp.read(packetBuffer, packetSize);
    packetBuffer[packetSize] = '\0';
    
    String config = String((char *)packetBuffer);
    Serial.println(config);
  }
}

char cmd[1024];
int lastnonce = 0;

int getcontrol() {
  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    //    printPacketInfo(packetSize);
    Udp.read(buffer, packetSize);
  }

}

void printNet() {
  Ethernet.MACAddress(mac);
  Serial.print(F("The MAC address is: "));
  for (byte octet = 0; octet < 6; octet++) {
    Serial.print(mac[octet], HEX);
    if (octet < 5) {
      Serial.print(F(':'));
    }
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

void
setup() {
  // start the Ethernet and UDP:
  //  pinMode(SDCARD_CS, OUTPUT);
  //  digitalWrite(SDCARD_CS, HIGH); //Deselect the SD card

  Serial.begin(9600);
  while (!Serial) ;
  
  Serial.println(F("starting"));

  Ethernet.begin(mac);
  Udp.begin(localPort);

  sprintf(macString, "%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  printNet();
  delay(1000);

  for (int i = 10; epoch == 0 && i > 0; i--) {
    epoch = getTime();
    delay(500);
  }
  if (epoch == 0) Serial.println(F("Can't get time!"));
  else printTime(epoch);
}

void
loop() {
  getConfig();
  sendData(NULL);
  delay(20000);
}
