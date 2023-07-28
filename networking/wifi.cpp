#include <Arduino.h>
//#include <SPI.h>         // needed for Arduino versions later than 0018

#define ESP Serial1
#define TTY Serial

#define DEFNTP "132.163.96.1"

#define SSID "ddiot"
#define WIFIPASS "IA4IoT2175"

enum esp_rsp {
  ESP_BAD_CMD = -5,
  ESP_MEM_ERR = -4,
  ESP_RSP_FAIL = -3,
  ESP_RSP_UNKNOWN = -2,
  ESP_RSP_TIMEOUT = -1,
  ESP_RSP_SUCCESS = 0
};

char timeServer[] = "time.nist.gov";
#define LOCALPORT 2390
#define UDP_TIMEOUT 2000
unsigned long gEPOCH = 0; // global to hold time

char *gBUFFER;
char gMAC[18];
char gIP[16];
char gGW[16];

void
printTime(unsigned long time) {
  TTY.print(F("Unix time = "));
  TTY.println(time);
  // print the hour, minute and second:
  TTY.print(F("The UTC time is "));       // UTC is the time at Greenwich Meridian (GMT)
  TTY.print((time  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  TTY.print(F(":"));
    
  // In the first 10 minutes of each hour, we'll want a leading '0'
  if (((time % 3600) / 60) < 10) TTY.print(F("0"));
    
  // print the minute (3600 equals secs per minute)
  TTY.print((time  % 3600) / 60);
  TTY.print(F(":"));
    
  // In the first 10 seconds of each minute, we'll want a leading '0'
  if ((time % 60) < 10) TTY.print(F("0"));
    
  TTY.println(time % 60); // print the second
}

#if 0
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
  udpEspWrite(timeServer, 123, ntpBuffer, NTP_PACKET_SIZE);
  udpEspRead(timeServer, 123);
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
#endif

void
writeEspp(const char *cmd, const char *params) {
  TTY.write("AT");
  TTY.write(cmd);
  if (params) {
    TTY.print('=');
    TTY.print(params);
  }
  TTY.print("\r\n");
}

void
writeEsp(const char *cmd, const char *params) {
  ESP.write("AT");
  ESP.write(cmd);
  if (params) {
    ESP.print('=');
    ESP.print(params);
  }
  ESP.print("\r\n");
}

#define MSIZE 1024
esp_rsp
readEsp(unsigned long wait, const char *target) {
  int found = (target == NULL);
  if (gBUFFER) free(gBUFFER);
  gBUFFER = (char *)malloc(MSIZE);
  //  memset(gBUFFER, 0, MSIZE);
  
  int left = MSIZE;
  int c = 0;
  char *ptr = gBUFFER;
  unsigned long timein = millis();
  char *line = gBUFFER;
  
  unsigned int matchcnt = 0;
  unsigned int targetlen = strlen(target);
  while (wait == 0 || timein + wait > millis()) {
    if (!ESP.available()) {delay(5); continue;}
    *ptr = ESP.read();
    if (*ptr == '\r') continue;
    if (!found) {
      if (*ptr == target[matchcnt]) {
	matchcnt++;
	ptr++;
	if (matchcnt == targetlen) {
	  found++;
	  ptr = gBUFFER;
	}
      } else {
	matchcnt = 0;
	if (*ptr == target[matchcnt]) matchcnt++;
	ptr++;
      }
    } else if (*ptr++ == '\n') {
      if (found && strncmp(line, "OK\n", 3) == 0) {
	ptr -= 4;
	*ptr = '\0';
	return ESP_RSP_SUCCESS;
      }
      if (strncmp(line, "ERROR", 5) == 0) {
	*ptr = '\0';
	return ESP_RSP_FAIL;
      }
      line = ptr;
    }
    left--;
    c++;
    if (left == 0) {
      char *t = (char *)realloc(gBUFFER, c + MSIZE);
      if (t == NULL) {
	if (gBUFFER) free(gBUFFER);
	return ESP_MEM_ERR;
      }
      gBUFFER = t;
      left = MSIZE;
      ptr = gBUFFER + c;
    }
  }
  *ptr = '\0';
  if (found) return ESP_RSP_SUCCESS;
  return ESP_RSP_TIMEOUT;
}

esp_rsp
readEsp2(unsigned long wait, const char *target) {
  int found = (target == NULL);
  if (gBUFFER) free(gBUFFER);
  gBUFFER = (char *)malloc(MSIZE);
  //  memset(gBUFFER, 0, MSIZE);
  
  int left = MSIZE;
  int c = 0;
  char *ptr = gBUFFER;
  unsigned long timein = millis();
  char *line = gBUFFER;
  
  while (wait == 0 || timein + wait > millis()) {
    if (!ESP.available()) {delay(5); continue;}
    *ptr = ESP.read();
    if (*ptr == '\r') continue;
    if (*ptr++ == '\n') {
      if (found && strncmp(line, "OK", 2) == 0) {
	*ptr = '\0';
	return ESP_RSP_SUCCESS;
      }
      if (!found && strncmp(line, target, strlen(target)) == 0) {
	
	found++;
	wait = 1000;
	timein = millis();
      }
      if (strncmp(line, "ERROR", 5) == 0) {
	*ptr = '\0';
	return ESP_RSP_FAIL;
      }
      line = ptr;
    }
    left--;
    c++;
    if (left == 0) {
      char *t = (char *)realloc(gBUFFER, c + MSIZE);
      if (t == NULL) {
	if (gBUFFER) free(gBUFFER);
	return ESP_MEM_ERR;
      }
      gBUFFER = t;
      left = MSIZE;
      ptr = gBUFFER + c;
    }
  }
  *ptr = '\0';
  if (found) return ESP_RSP_SUCCESS;
  return ESP_RSP_TIMEOUT;
}

#if 0
char buffer[9000];
void
readEsp1(unsigned long wait) {
  unsigned long timein = millis();
  char *ptr = buffer;
  while (timein + wait > millis()) {
    if (ESP.available()) {
      *ptr = ESP.read();
      //      if (*ptr == '\r') continue;
      ptr++;
    }
  }
  *ptr = '\0';
}
#endif

void
ttyout(esp_rsp r) {
  switch(r) {
  case ESP_RSP_SUCCESS: TTY.println(gBUFFER); break;
  case ESP_RSP_FAIL: TTY.print("FAIL: "); TTY.println(gBUFFER); break;
  case ESP_RSP_TIMEOUT: TTY.print("TIMEOUT: "); TTY.println(gBUFFER); break;
  default: TTY.println("OTHER ERROR"); break;
  }
}

int
SetupEsp() {
  ESP.begin(115200);
  delay(500);

  for(int x = 0; x < 10; x++) {
    ESP.write("ATZ\r\n");
    if (readEsp(1000, NULL) == ESP_RSP_SUCCESS) return true;
    delay(500);
    TTY.write('+');
  }
  return false;
}

esp_rsp
getEspMAC() {
  writeEsp("+WFMAC", NULL);
  if (readEsp(1000, NULL) == ESP_RSP_SUCCESS) {
    char *ptr = strstr(gBUFFER, "+WFMAC:");
    if (!ptr) return ESP_RSP_FAIL;
    ptr += 7;
    for(int x=0; x < 17; x++) gMAC[x] = *ptr++;
    return ESP_RSP_SUCCESS;
  }
  return ESP_RSP_FAIL;
}

/*
  +WFSTAT:sta0
mac_address=d4:3d:39:39:69:5a
bssid=12:0c:6b:60:d2:71
ssid=ddiot
id=0
mode=STATION
key_mgmt=WPA2-PSK
pairwise_cipher=CCMP
group_cipher=CCMP
channel=6
wpa_state=COMPLETED
*/

int
statusEsp() {
  writeEsp("+WFSTAT", NULL);
  if (readEsp(0, "+WFSTAT:") == ESP_RSP_SUCCESS) {
    if (!strstr(gBUFFER, "sta0")) return -2;
    if (strstr(gBUFFER, "wpa_state=INACTIVE")) return 0;
    if (strstr(gBUFFER, "wpa_state=DISCONNECTED")) return 0;
    if (strstr(gBUFFER, "wpa_state=COMPLETED")) return 1;
  }
  return -1;
}

esp_rsp
sendPing(const char *ip, int count) {
  char val[12];
  sprintf(val, "0,%s,%d", ip, count);
  writeEsp("+NWPING", val);
  esp_rsp r = readEsp(10000, "+NWPING:");
  //if (r == ESP_RSP_SUCCESS) TTY.print(gBUFFER);
  return r;
}

esp_rsp
getHost(const char *hostname) {
  for (int x = 0; x < 5; x++) {
    writeEsp("+NWHOST", hostname);
    esp_rsp r = readEsp(10000, "+NWHOST:");
    if (r != ESP_RSP_SUCCESS) return r;
    if (strlen(gBUFFER) > 3) break;
    delay(100);
  }
  return ESP_RSP_SUCCESS;
}

esp_rsp
setEspNtp(const char *hostname) {
  const char *ntpserver = DEFNTP;
  if (hostname) ntpserver = hostname;
  //    if (getHost(hostname) == ESP_RSP_SUCCESS) ntpserver = gBUFFER;

  // set sns server
  char val[strlen(ntpserver) + 10];
  TTY.println(ntpserver);
  sprintf(val, "1,%s,3600", ntpserver);
  writeEsp("+NWSNTP", val);
  if (readEsp(5000, NULL) != ESP_RSP_SUCCESS) return ESP_RSP_FAIL;
}

unsigned long
convert_time_to_epoch() {
  writeEsp("+TIME", NULL);
  if (readEsp(1000, "+TIME:") != ESP_RSP_SUCCESS) return 0;

  char *ptr = gBUFFER;
  TTY.print(ptr);
  TTY.print(" / ");
  char *ptr1 = strchr(ptr, '-');
  if (!ptr1) return 0;
  *ptr1++ = '\0';
  int year = atoi(ptr);
  year -= 1900;
  ptr = ptr1;
  ptr1 = strchr(ptr, '-');
  *ptr1++ = '\0';
  int month = atoi(ptr);
  ptr = ptr1;
  ptr1 = strchr(ptr, ',');
  *ptr1++ = '\0';
  int day = atoi(ptr);
  ptr = ptr1;
  ptr1 = strchr(ptr, ':');
  *ptr1++ = '\0';
  int hour = atoi(ptr);
  ptr = ptr1;
  ptr1 = strchr(ptr, ':');
  *ptr1++ = '\0';
  int minute = atoi(ptr);
  ptr = ptr1;
  int second = atoi(ptr);
  int yday = day + (153 * (month - 1) + 2) / 5 - (153 * (month - 1) + 2) / 100 + (153 * (month - 1) + 2) / 400;

  unsigned long epoch = second + minute * 60 + hour * 3600 + yday * 86400 +
    (year - 70) * 31536000 + ((year - 69) / 4) * 86400 -
    ((year - 1) / 100) * 86400 + ((year + 299) / 400) * 86400;
  
  return epoch;
}

esp_rsp
getEspIP() {
  writeEsp("+NWIP=", NULL);
  esp_rsp r;
  if ((r = readEsp(1000, "+NWIP:")) != ESP_RSP_SUCCESS) return r;
  char *ptr = gBUFFER + 2; // skip interface number
  char *ptr1 = strchr(ptr, ',');
  if (!ptr1) return ESP_RSP_FAIL;
  *ptr1++ = '\0';
  strcpy(gIP, ptr);
  ptr = ptr1;
  ptr1 = strchr(ptr, ',');
  if (!ptr1) return ESP_RSP_FAIL;
  ptr1++;
  ptr = ptr1;
  strcpy(gGW, ptr);
  return ESP_RSP_SUCCESS;
}

void rwEsp() {
  TTY.println("enter cmds");
  while(1) {
    while (TTY.available()) ESP.write(Serial.read());
    while (ESP.available()) TTY.write(ESP.read());
  }
}
    
esp_rsp
beginUdp(int localport) {
  // close all sessions
  //  writeEsp("+TRTALL", NULL);
  //  if (readEsp(1000, NULL) != ESP_RSP_SUCCESS) return ESP_RSP_FAIL;

  // close udp sessions
  writeEsp("+TRTRM", "2");
  esp_rsp r = readEsp(1000, NULL);
  if (r == ESP_RSP_FAIL || r != ESP_RSP_SUCCESS)
    if (strstr(gBUFFER, "ERROR:-99") == NULL) return ESP_RSP_FAIL;

  // set local udp port
  char buff[10];
  sprintf(buff, "%d", localport);
  writeEsp("+TRUSE", buff);
  return readEsp(1000, NULL);
}

esp_rsp
sendUdp(const char *host, unsigned int port, const char *data) {
  if (host == NULL || data == NULL || strlen(data) == 0 || port <= 0) return ESP_RSP_FAIL;

  char buffer[strlen(host) + 1 + 5 + 1];
  sprintf(buffer, "%s,%d", host, port);
  writeEsp("+TRUR", buffer);
  if (readEsp(1000, NULL) != ESP_RSP_SUCCESS) return ESP_RSP_FAIL;

  char buffer1[6 + 5 + 5 + strlen(data) + 2];
  sprintf(buffer1, "\033S2%d,0,0,%s\n", strlen(data) + 1, data);
  ESP.write(buffer1);
  return(readEsp(1000, NULL));
}

#define udpsize 128
esp_rsp
rcvUdp(unsigned long wait) {
  wait=5000;
  if (gBUFFER) free(gBUFFER);
  gBUFFER = (char *)malloc(udpsize);
  char *ptr = gBUFFER;
  int left = udpsize;
  unsigned long timein = millis();
  
  while (wait == 0 || timein + wait > millis()) {
    if (!ESP.available()) {delay(5); continue;}
    *ptr = ESP.read();
    if (*ptr == '\r') continue;
    ptr++;
    left--;
    if (left == 1) break;
  }
  *ptr = '\0';
  return ESP_RSP_SUCCESS;
}

esp_rsp
rcvUdp1(unsigned long wait) {
  TTY.println("start rcvudp");
  if (gBUFFER) free(gBUFFER);
  gBUFFER = (char *)malloc(udpsize);
  char *ptr = gBUFFER;
  unsigned long timein = millis();
  char *line = gBUFFER;
  int left = udpsize;
  int msglen = 0;
  
  int found = 0;
  unsigned int matchcnt = 0;
  char *target = "+TRDUS:2";
  unsigned int targetlen = strlen(target);
  while (wait == 0 || timein + wait > millis()) {
    if (!ESP.available()) {delay(5); continue;}
    if (left == 0) {
      *ptr = '\0';
      TTY.println("NO more buffer space");
      return ESP_RSP_FAIL;
    }

    *ptr = ESP.read();
    if (*ptr == '\r') { if (msglen > 0) msglen--; continue; }

    if (found == -1) {
      if (*ptr == '\n') {
	*ptr = '\0';
	TTY.println("parse fail");
	return ESP_RSP_FAIL;
      }
      left--;
      ptr++;
      continue;
    }

    if (found == 0) {
      if (*ptr == target[matchcnt]) {
	matchcnt++;
	if (matchcnt == targetlen) {
	  found++;
	  ptr = gBUFFER;
	  left = udpsize;
	}
      } else {
	matchcnt = 0;
	if (*ptr == target[matchcnt]) matchcnt++;
	ptr++;
	left--;
      }
      continue;
    }
    if (found == 1) {
      if (*ptr != ',') found = -1;
      else {
	ptr = gBUFFER;
	left = udpsize;
	found++;
      }
      continue;
    }
    if (found == 2) {     // skip over ip address
      if (*ptr == ',') found++;
      left--;
      ptr++;
      continue;
    }
    if (found == 3) { // skip over port
      if (*ptr == ',') found++;
      left--;
      ptr++;
      continue;
    }
    char *newbuff;
    if (found == 4) {   // get len
      char b[2];
      if (*ptr >= '0' && *ptr <= '9') {
	b[0] = *ptr;
	b[1] = '\0';
	msglen = (msglen * 10) + atoi(b);
	left--;
	ptr++;
	continue;
      }
      if (*ptr == ',') {
	found++;
	if (msglen == 0) return ESP_RSP_FAIL;
	TTY.print("packetlen is ");
	TTY.println(msglen);
	msglen += 2; // for last \r\n
	newbuff = (char *)malloc(msglen);
	if (newbuff == NULL) return ESP_MEM_ERR;
	if (gBUFFER) free(gBUFFER);
	gBUFFER = newbuff;
	ptr = gBUFFER;
	left = msglen;
	wait = 1000;
	timein = millis();
      } else { //badlength
	found = -1;
	left--;
	ptr++;
      }
      continue;
    }
    if (found == 5) { // read rest of udp packet
      ptr++;
      msglen--;
      *ptr = '\0';
      left--;
      if (msglen == 0) {
	*(ptr-2) = '\0'; // take off extra \r\n
	return ESP_RSP_SUCCESS;
      }
    }
  }
  *ptr = '\0';
  return ESP_RSP_TIMEOUT;
}

esp_rsp
connectEsp(const char *ssid, const char *passwd) {
  // check mode
  writeEsp("+WFMODE", NULL);
  if (readEsp(1000, "+WFMODE:") == ESP_RSP_SUCCESS) {
    if (gBUFFER[0] != '0') { // not in station mode
      writeEsp("+WFMODE", "0");
      if (readEsp(1000, NULL) != ESP_RSP_SUCCESS) return ESP_RSP_FAIL;
    }
  } else return ESP_RSP_FAIL;

  ESP.write("AT");
  ESP.write("+WFJAPA");
  ESP.write('=');
  ESP.write(ssid);
  if (passwd != NULL) {
    ESP.write(",");
    ESP.write(passwd);
  }
  ESP.write("\r\n");
  readEsp(0, "+WFJAP:");
  /*+WFJAP:1,'ddiot',192.168.2.144}*/
  if (gBUFFER[0] == '1') return ESP_RSP_SUCCESS;
  return ESP_RSP_FAIL;
}

void setup() {
  TTY.begin(9600);

  while (!TTY);
  delay(200);

  for (int i=3; i > 0; i--) {
    TTY.print(i);
    TTY.print(F(" "));
    delay(100);
  }
  TTY.println();

  if (!SetupEsp()) {
    TTY.println("Can't talk to ESP. stopping");
    return;
  }
  TTY.println("ESP ready");
  
  esp_rsp rv;

  if (getEspMAC() == ESP_RSP_SUCCESS) {
    TTY.print("MAC is ");
    TTY.println(gMAC);
  }
  delay(200);
  
  TTY.println("Do scan");
  writeEsp("+WFSCAN", NULL);
  rv = readEsp(5000, "+WFSCAN:");
  TTY.println(gBUFFER);
  //  ttyout(rv);
  delay(500);
  
  int connected = 0;
  switch(statusEsp()) {
  case -2: TTY.println("Wrong Mode\n"); return; 
  case -1: TTY.println("Major Error\n"); TTY.println(gBUFFER); return; 
  case 0: TTY.println("Disconnected\n"); break;
  case 1: TTY.println("Connected\n"); connected = 1; break;
  default: return; 
  }

  if (!connected) {
    if ((rv = connectEsp(SSID, WIFIPASS)) == ESP_RSP_SUCCESS) {
      TTY.println("Connected");
      TTY.println(gBUFFER);
    } else {
      TTY.println("Did not connect");
      return;
    }
  } else TTY.println(gBUFFER);

  TTY.print("rssi: ");
  writeEsp("+WFRSSI", NULL);
  if (readEsp(1000, "+RSSI:") == ESP_RSP_SUCCESS) 
    TTY.println(gBUFFER);
  delay(500);
  
  if (getEspIP() == ESP_RSP_SUCCESS) {
    TTY.print("IP address: ");
    TTY.print(gIP);
    TTY.print(" GW: ");
    TTY.println(gGW);
  } else TTY.println("Get IP failed");

  if (sendPing("8.8.8.8", 2) == ESP_RSP_SUCCESS) TTY.println("Ping OK");
  else TTY.println("Ping Failed");

  if (getHost("buttercup.coslabs.com") == ESP_RSP_SUCCESS) {
    TTY.print("IP Address is ");
    TTY.println(gBUFFER);
  } else TTY.println("gethost failed");

  TTY.print("ntp:  ");
  setEspNtp("pool.ntp.org");

  TTY.print("Seconds: ");
  TTY.println(convert_time_to_epoch());
  
  if (beginUdp(LOCALPORT) != ESP_RSP_SUCCESS) {
    TTY.println("UDP start failed");
    TTY.println(gBUFFER);
    return;
  }

  TTY.println("get params");
  char msg[1024];
  sprintf(msg, "GET /%s/params", gMAC);
  if (sendUdp("mcogs.coslabs.com", 57573, msg) != ESP_RSP_SUCCESS) {
    TTY.println("Send UDP failed");
  } else {
    if (rcvUdp1(5000) != ESP_RSP_SUCCESS) {
      TTY.println("UDP recv failed");
    }
    TTY.println(gBUFFER);
  }
}

void loop() {
  
  TTY.println("send data");
  char msg[1024];
  sprintf(msg, "PUT /%s\r\n{ \"hello\": \"there\"}", gMAC);
  if (sendUdp("mcogs.coslabs.com", 57573, msg) != ESP_RSP_SUCCESS) {
    TTY.println("Send UDP failed");
  } else {
    if (rcvUdp1(5000) != ESP_RSP_SUCCESS) {
      TTY.println("UDP recv failed");
    }
    TTY.println(gBUFFER);
  }


  while (TTY.available()) ESP.write(Serial.read());
  while (ESP.available()) TTY.write(ESP.read());
  return;

  //  TTY.println(espping(PING_SRVR));
  //  delay(500);

  //For example, to set the mode to STA, connect to a network,
  //and check your IP address, type:
  //AT+CWMODE=1
  //AT+CWJAP="networkName","networkPassword"
  //AT+CIFSR

#if 0
  // join AP
  writeEsp("+WFJAP=ssid,auth,enc,key", NULL);
  //  writeEsp("+WFJAPA=ssid,enc");
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // connect to configured ap
  writeEsp("+WFCAP=ssid,auth,enc,key", NULL);
  writeEsp("+WFJAPA=ssid,enc");
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif
  
#if 0
  // quit ap
  writeEsp("+WFQAP", NULL);
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set wifi mode
  writeEsp("+WFAPWM", NULL);
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set wifi channel
  writeEsp("+WFAPCH", NULL);
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif
  
#if 0
  // set IP setting
  writeEsp("+NWIP", NULL); // iface, ip addr, netmask, gw
  rv = readEsp(5000, NULL);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set dns
  writeEsp("+NWDNS", NULL); // ip addr
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set dns2
  writeEsp("+NWDNS2", NULL); // ip addr
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // get host ip by name
  writeEsp("+NWHOST", NULL); // name
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // ping
  writeEsp("+NWPING", NULL); // iface, dest ip, count
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // enable/disable dhcp client
  writeEsp("+NWDHC", NULL); // dhcpc
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set time server
  // SNS, SNS1, SNS2
  writeEsp("+NWSNS", NULL); // ip addr
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set ntp client update period
  writeEsp("+NWSNUP", NULL); // time period
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // enable/disable ntp client service
  //SNTP, SNTP1, SNTP2
  writeEsp("+NWSNTP", NULL); // sntp,server_ip,period
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set tcp client
  writeEsp("+TRTC", NULL); // server ip, port, local port
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set mqtt tls
  writeEsp("+NWMQTLS", NULL); // tls
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif
  
#if 0
  // set udp socket
  writeEsp("+TRUSE", NULL); // local port
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set mqtt tls
  writeEsp("+NWMQTLS", NULL); // tls
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif
  
#if 0
  // set udp client
  writeEsp("+TRUR", NULL); // remote ip, remote port
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

#if 0
  // set mqtt tls
  writeEsp("+NWMQTLS", NULL); // tls
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif
  
#if 0
  // display all sessions
  writeEsp("+TRPALL", NULL); // 
  rv = readEsp(5000);
  ttyout(rv);
  delay(500);
#endif

  delay(10000);
}
