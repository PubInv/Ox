//#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h> // UDP library from: bjoern@cs.stanford.edu 12/30/2008

#define ADDRESS 0x04
#define MYDELAY 500

uint32_t epoch = 0;
uint32_t startMillis = 0;

char packetBuffer[10 * 1024];

#define TIMESERVER "time.nist.gov"
#define DATASERVER "cogs.coslabs.com"
#define DATAPORT 6060

#define localPort 2390
#define UDP_TIMEOUT 5000

IPAddress ip(192, 168, 1, 177);

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

#define NUMBER_OF_MAC      20
byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

char manuf[17]; // INFO 0
char model[17];  // INFO 1
char voltage_string[5]; // INFO 2
char revision[5]; // INFO 3
char manuf_date[9];  // INFO 4
char serial[17];  // INFO 5
char country[17];  // INFO 6
uint16_t rate_voltage;  //50-51
uint16_t rate_current;  //52-53
uint16_t max_voltage;  //54-55
uint16_t max_current;  //56-57
uint16_t out_voltage;  //60-61
uint16_t out_current;  //62-63
uint8_t temp;  //68
uint8_t status0;  //6C
uint8_t status1;  //6F
uint16_t set_voltage; // 70-71 r/w
uint16_t set_current; // 72-73 r/w
uint8_t control; //  7C  r/w
uint8_t on_off;

int setPS_Addr(uint8_t addr) {
  Serial1.print("ADDS "); Serial1.print(addr); Serial1.print("\r\n");
  delay(50);
  char buff[5];
  uint8_t c = Serial1.readBytesUntil('\n', buff, sizeof buff);
  if (c != 3 || buff[0] != '=' || buff[1] != '>') return 0;
  return 1;
}

int setPS_Val(uint8_t addr, const char *loc, const char *val) {
  if (!setPS_Addr(addr)) {
    Serial.println("didn't set address");
    return 0;
  }

  Serial1.print(loc); Serial1.print(' '); Serial1.print(val); Serial1.print("\r\n");
  delay(100);
  char b[5];
  int c = Serial1.readBytesUntil('\n', b, sizeof b);
  if (c != 3 || b[0] != '=' || b[1] != '>') return 0;
  return 1;
}

int setPS_GlobOnOff(uint8_t addr, const char *val) {
  if (strcasecmp(val, "on") == 0) val = "1";
  else val = "0";
  return setPS_Val(addr, "GLOB", val);
}

int setPS_OnOff(uint8_t addr, const char *val) {
  if (strcasecmp(val, "on") == 0) val = "1";
  else val = "0";
  return setPS_Val(addr, "POWER", val);
}

int setPS_Voltage(uint8_t addr, uint16_t volts) {
  char b[7];
  snprintf(b, sizeof b, "%4.1f", volts / 100.0);
  return setPS_Val(addr, "SV", b);
}

int setPS_Current(uint8_t addr, uint16_t amps) {
  char b[7];
  snprintf(b, sizeof b, "%4.1f", amps / 100.0);
  return setPS_Val(addr, "SI", b);
}

char *getPS_Val(uint8_t addr, const char *val) {
  static char rval[50];
  if (!setPS_Addr(addr)) {
    Serial.println("didn't set address");
    return 0;
  }

  Serial1.print(val); Serial1.print("\r\n");
  delay(100);
  int c = Serial1.readBytesUntil('\n', rval, sizeof rval);
  rval[c-1] = '\0';
  delay(10);
  char b[5];
  c = Serial1.readBytesUntil('\n', b, sizeof b);
  if (c != 3 || b[0] != '=' || b[1] != '>') return NULL;
  return rval;
}

void getPS_Manuf(int addr) {
  char *r = getPS_Val(addr, "INFO 0");
  strncpy(manuf, r, sizeof manuf);
}

void getPS_Model(int addr) {
  char *r = getPS_Val(addr, "INFO 1");
  strncpy(model, r, sizeof model);
}

void getPS_VoltageString(int addr) {
  char *r = getPS_Val(addr, "INFO 2");
  strncpy(voltage_string, r, sizeof voltage_string);
}

void getPS_Revision(int addr) {
  char *r = getPS_Val(addr, "INFO 3");
  strncpy(revision, r, sizeof revision);
}

void getPS_ManufDate(int addr) {
  char *r = getPS_Val(addr, "INFO 4");
  strncpy(manuf_date, r, sizeof manuf_date);
}

void getPS_Serial(int addr) {
  char *r = getPS_Val(addr, "INFO 5");
  strncpy(serial, r, sizeof serial);
}

void getPS_Country(int addr) {
  char *r = getPS_Val(addr, "INFO 6");
  strncpy(country, r, sizeof country);
}

void getPS_RateVoltage(int addr) {
  char *r = getPS_Val(addr, "RATE?");
  char b[20];
  strncpy(b, r, sizeof b);
  char *ptr = NULL;
  rate_voltage = -1;
  if ((ptr = strchr(b, ' '))) {
    *ptr = '\0';
    rate_voltage = int(atof(b) * 100);
  }
}

void getPS_RateCurrent(int addr) {
  char *r = getPS_Val(addr, "RATE?");
  char b[20];
  strncpy(b, r, sizeof b);
  char *ptr = NULL;
  rate_current = -1;
  if ((ptr = strchr(b, ' '))) {
    rate_current = int(atof(ptr + 1) * 100);
  }
}

void getPS_OnOff(int addr) {
  char *r = getPS_Val(addr, "POWER 2");
  switch (r[0]) {
  case '0': on_off = 0; break;
  case '1': on_off = 1; break;
  case '2': on_off = 0; break;
  case '3': on_off = 1; break;
  }
}

void getPS_MaxVoltage(int addr) {
  max_voltage = -1;
}

void getPS_MaxCurrent(int addr) {
  max_current = -1;
}

void getPS_OutVoltage(int addr) {
  char *r = getPS_Val(addr, "RV?");
  out_voltage = int(atof(r) * 100);
}

void getPS_OutCurrent(int addr) {
  char *r = getPS_Val(addr, "RI?");
  out_current = int(atof(r) * 100);
}

void getPS_Status0(int addr) {
  char *r = getPS_Val(addr, "STUS 0");
  status0 = (r[0] - '0') << 4;
  status0 += (r[1] - '0') & 0x0F;
}

void getPS_Status1(int addr) {
  char *r = getPS_Val(addr, "STUS 1");
  status1 = (r[0] - '0') << 4;
  status1 += (r[1] - '0') & 0x0F;
}

void getPS_Temp(int addr) {
  char *r = getPS_Val(addr, "RT?");
  temp = atoi(r);
}

void getPS_SetVoltage(int addr) {
  char *r = getPS_Val(addr, "SV?");
  set_voltage = int(atof(r) * 100);
}

void getPS_SetCurrent(int addr) {
  char *r = getPS_Val(addr, "SI?");
  set_current = int(atof(r) * 100);
}

void getPS_Control(int addr) {
}

// send an NTP request to the time server at the given address
void getTime() {
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

  Udp.beginPacket(TIMESERVER, 123); //NTP requests are to port 123
  Udp.write(ntpBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();

  uint64_t startMs = millis();
  uint16_t packetSize = 0;
  do {
    packetSize = Udp.parsePacket();
  } while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT);

  if (packetSize) {
    //    Serial.print(F("UDP Packet received, size "));
    //    Serial.println(packetSize);
    //    Serial.print(F("From "));
    //IPAddress remoteIp = Udp.remoteIP();
    //    Serial.print(remoteIp);
    //    Serial.print(F(", port "));
    //    Serial.println(Udp.remotePort());
    Udp.read(ntpBuffer, NTP_PACKET_SIZE);
    startMillis = millis();

    // the timestamp starts at byte 40 of the received packet it is four bytes,
    // or two words, long. First, extract the two words:
    uint64_t highWord = word(ntpBuffer[40], ntpBuffer[41]);
    uint64_t lowWord = word(ntpBuffer[42], ntpBuffer[43]);

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    uint64_t secsSince1900 = highWord << 16 | lowWord;
    
    //    Serial.print(F("Seconds since Jan 1 1900 = "));
    //    Serial.println(secsSince1900);

    // now convert NTP time into )everyday time:
    Serial.print(F("Unix time = "));
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const uint64_t seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

    // print the hour, minute and second:
    Serial.print(F("The UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(F(":"));
    
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print(F("0"));
    }
    
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(F(":"));
    
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print(F("0"));
    }
    
    Serial.println(epoch % 60); // print the second
  }
}

// prints 8-bit data in hex with leading zeroes
void PrintHex8(uint8_t *data, uint8_t length) {
  Serial.print(F("0x"));
  for (int i = 0; i < length; i++) {
    if (data[i]<0x10) { Serial.print(F("0")); }
    Serial.print(data[i],HEX);
    Serial.print(F(" "));
  }
}

// prints 16-bit data in hex with leading zeroes
void PrintHex16(uint16_t *data, uint8_t length) {
  Serial.print(F("0x")); 
  for (int i = 0; i < length; i++) { 
      uint8_t MSB=byte(data[i]>>8);
      uint8_t LSB=byte(data[i]);
      
      if (MSB<0x10) {Serial.print(F("0"));} Serial.print(MSB,HEX); Serial.print(F(" ")); 
      if (LSB<0x10) {Serial.print(F("0"));} Serial.print(LSB,HEX); Serial.print(F(" ")); 
  }
}

uint8_t readcommreg(uint16_t addr) {
  pinMode(SS, OUTPUT);

  //  SPI.begin(SS);
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(addr >> 8);  //address high
  SPI.transfer(addr & 0xFF);  //address low
  SPI.transfer(0x00); // byte read mode
  SPI.transfer(0x01); // byte read 1 byte

  uint8_t buffer = SPI.transfer(0x00);
  //  PrintHex8(&buffer, 1);
  //  Serial.println(buffer, HEX);
  SPI.endTransaction();
  //  SPI.end(SS);
  //  delay(10);
  return buffer;
}

char *timenow() {
  static char now[100];
  snprintf(now, sizeof now, "{ \"TimeStamp\": %lu }", epoch + (millis() - startMillis) / 1000);
  return now;
}

void sendMsg(const char *msg) {
  Udp.beginPacket(DATASERVER, DATAPORT);
  char b[10 * 1024 + 50];
  uint16_t c = snprintf(b, sizeof b, "[ %s, %s ]", timenow(), msg);
  Udp.write(b, c);
  Udp.endPacket();
}

int readResp() {
  uint64_t startMs = millis();
  int packetSize = 0;

  do {
    packetSize = Udp.parsePacket();
  } while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT);

  if (packetSize) {
    Serial.print(F("Received packet of size "));
    Serial.println(packetSize);
    Serial.print(F("From "));
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) Serial.print(F("."));
    }
    Serial.print(F(", port "));
    Serial.println(Udp.remotePort());
    
    // read the packet into packetBufffer
    uint16_t c = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    //    Serial.println(F("Contents:"));
    //    Serial.println(packetBuffer);
    return c;
  }
  return 0;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial1.begin(4800);
  while (!Serial1);

  randomSeed(analogRead(0));
  
  uint8_t index = random(0, NUMBER_OF_MAC);
  index=2; // xxxx remove this later
  Serial.print(F("Using mac index = "));
  Serial.println(index);

  Ethernet.begin(mac[index]);

  byte tmac[6];
  Ethernet.MACAddress(tmac);
  Serial.print(F("The MAC address is: "));
  for (byte octet = 0; octet < 6; octet++) {
    Serial.print(tmac[octet], HEX);
    if (octet < 5) {
      Serial.print(':');
    }
  }
  Serial.println();

  Serial.print(F("Version 5200 ")); Serial.print(readcommreg(0x001F));
  Serial.print(F("  PHY Status ")); Serial.print(readcommreg(0x0035));
  Serial.println();

  Udp.begin((uint16_t)localPort);

  Serial.print(F("IP address: ")); Serial.println(Ethernet.localIP());
  Serial.print(F("Gateway: ")); Serial.println(Ethernet.gatewayIP());
  Serial.print(F("Subnet Mask: ")); Serial.println(Ethernet.subnetMask());
  Serial.print(F("DNS Server: ")); Serial.println(Ethernet.dnsServerIP());
  Serial.println();

  getTime();

  getPS_Manuf(ADDRESS);
  Serial.print("Manuf: ");
  if (!strlen(manuf)) strcpy(manuf, "UNKWN");
  Serial.println(manuf);
  delay(MYDELAY);

  getPS_Model(ADDRESS);
  Serial.print("Model: ");
  if (!strlen(model)) strcpy(manuf, "UNKWN");
  Serial.println(model);
  delay(MYDELAY);

  getPS_VoltageString(ADDRESS);
  Serial.print("VoltageSt: ");
  if (!strlen(voltage_string)) strcpy(manuf, "UNKWN");
  Serial.println(voltage_string);
  delay(MYDELAY);

  getPS_Revision(ADDRESS);
  Serial.print("Rev: ");
  if (!strlen(revision)) strcpy(manuf, "UNKWN");
  Serial.println(revision);
  delay(MYDELAY);

  getPS_ManufDate(ADDRESS);
  Serial.print("ManufDate: ");
  if (!strlen(manuf_date)) strcpy(manuf, "UNKWN");
  Serial.println(manuf_date);
  delay(MYDELAY);

  getPS_Serial(ADDRESS);
  Serial.print("Serial: ");
  if (!strlen(serial)) strcpy(manuf, "UNKWN");
  Serial.println(serial);
  delay(MYDELAY);

  getPS_Country(ADDRESS);
  Serial.print("Country: ");
  if (!strlen(country)) strcpy(manuf, "UNKWN");
  Serial.println(country);
  delay(MYDELAY);

  getPS_RateVoltage(ADDRESS);
  Serial.print("RateVoltage: ");
  if (rate_voltage < 0) Serial.println("UNKWN");
  else Serial.println(rate_voltage);
  delay(MYDELAY);

  getPS_RateCurrent(ADDRESS);
  Serial.print("RateCurrent: ");
  if (rate_current < 0) Serial.println("UNKWN");
  else Serial.println(rate_current);
  delay(MYDELAY);

  getPS_MaxVoltage(ADDRESS);
  Serial.print("MaxVoltage: ");
  if (max_voltage < 0) Serial.println("UNKWN");
  else Serial.println(max_voltage);
  delay(MYDELAY);

  getPS_MaxCurrent(ADDRESS);
  Serial.print("MaxCurrent: ");
  if (max_current < 0) Serial.println("UNKWN");
  else Serial.println(max_current);
  delay(MYDELAY);

  snprintf(packetBuffer, sizeof packetBuffer, "{ \"Manufacturer\": \"%s\", \"Model\": \"%s\", \"VoltString\": \"%s\", \"Revision\": \"%s\", \"Serial\": \"%s\", \"VoltageRating\": %d, \"CurrentRating\": %d, \"MaxVoltage\": %d, \"MaxCurrent\": %d}", manuf, model, voltage_string, revision, serial, rate_voltage, rate_current, max_voltage, max_current);
  sendMsg(packetBuffer);

  if (setPS_OnOff(ADDRESS, "ON")) Serial.println("Turned it on");
  else Serial.println("failed to turn it on");

  if (setPS_Voltage(ADDRESS, 1000)) Serial.println("Set volts to 10");
  else Serial.println("failed to set volts");

  if (setPS_Current(ADDRESS, 500)) Serial.println("Set current to 5");
  else Serial.println("failed to set current");

}

uint16_t count = 1;

void loop() {
  if (count % 4 == 0) {
    uint8_t r = random(0, 100);
    if (r > 80) {
      uint8_t v = random(0, 600);
      if (setPS_Voltage(ADDRESS, v)) {
	Serial.print("Set volts to ");
	Serial.print(v);
	Serial.print(" and ");
	if (setPS_OnOff(ADDRESS, "on")) Serial.println("turn ps on");
	else Serial.println("failed to set ON");
      }
      else Serial.println("failed to set volts");
    } else if (r > 60) {
      uint8_t v = random(601, 1150);
      if (setPS_Voltage(ADDRESS, v)) {
	Serial.print("Set volts to ");
	Serial.print(v);
	Serial.print(" and ");
	if (setPS_OnOff(ADDRESS, "on")) Serial.println("turn ps on");
	else Serial.println("failed to set ON");
      }
      else Serial.println("failed to set volts");
    } else if (r > 30) {
      if (setPS_OnOff(ADDRESS, "on")) Serial.println("turn ps on");
      else Serial.println("failed to set ON");
    } else {
      if (setPS_OnOff(ADDRESS, "off")) Serial.println("turn ps off");
      else Serial.println("failed to set OFF");
    }
  }

  getPS_OnOff(ADDRESS);
  if (on_off) Serial.print("ON  ");
  else Serial.print("OFF ");

  getPS_OutVoltage(ADDRESS);
  Serial.print("V: ");
  Serial.print(out_voltage);
  delay(MYDELAY);
  getPS_SetVoltage(ADDRESS);
  Serial.print("/");
  Serial.print(set_voltage);
  delay(MYDELAY);

  getPS_OutCurrent(ADDRESS);
  Serial.print(" A: ");
  Serial.print(out_current);
  delay(MYDELAY);
  getPS_SetCurrent(ADDRESS);
  Serial.print("/");
  Serial.print(set_current);
  delay(MYDELAY);

  getPS_Temp(ADDRESS);
  Serial.print(" T: ");
  Serial.print(temp);
  delay(MYDELAY);
  
  getPS_Status0(ADDRESS);
  Serial.print(" S0: ");
  Serial.print(status0, BIN);
  delay(MYDELAY);

  getPS_Status1(ADDRESS);
  Serial.print(" S1: ");
  Serial.print(status1, BIN);
  delay(MYDELAY);

  getPS_Control(ADDRESS);
  Serial.print(" C: ");
  Serial.println(control, BIN);
  delay(MYDELAY);

  snprintf(packetBuffer, sizeof packetBuffer,
	   "{ \"ON\": %s, \"VoltsOut\": %d, \"VoltsSet\": %d, \"Current\": %d, \"CurrentSet\": %d, \"Temp\": %d, \"Status0\": \"0X%X\", \"Status1\": \"0X%X\", \"Control\": %X }",
	   on_off ? "true" : "false", out_voltage, set_voltage, out_current, set_current, temp, status0, status1, control);
  sendMsg(packetBuffer);

  count++;
  delay(15 * 1000);
}
