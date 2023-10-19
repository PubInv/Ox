//test config for display, thermocouple and ethernet
//Author: Lawrence Kincheloe
//

// https://github.com/adafruit/Adafruit-MAX31855-library
// https://ryand.io/AutoPID/
// https://github.com/mprograms/SimpleRotary
// https://github.com/olikraus/u8g2
// //
#include <Arduino.h>
#include <U8g2lib.h>
#include <MUIU8g2.h>
#include <SimpleRotary.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MAX31855.h"
#include <AutoPID.h>

#include <Ethernet.h>
#include <EthernetUdp.h>

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define EXT1_MAXCS   10
#define EXT2_MAXCS   11
#define INT1_MAXCS   12
#define MAXCLK  5

#define EXT1_OUTPUT_PIN A1
#define EXT2_OUTPUT_PIN A1
#define INT1_OUTPUT_PIN A1

#define LCD_CLOCK 22 // Clock (Common), sometimes called SCK or SCL
#define LCD_MOSI 23  // MOSI (common), sometimes called SDA or DATA
#define LCD_RESET 33 // LCD reset, sometimes called RST or RSTB
#define LCD_CS 5     // LCD CS, sometimes called EN or SS
#define LCD_RS 1     // LCD RS, sometimes called A0 or DC

#define  LED_PIN_RED  10
#define  LED_PIN_GREEN 11
#define  LED_PIN_BLUE 12

#define TEMP_READ_DELAY 800 //can only read digital temp sensor every ~750ms

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP .12
#define KI .0003
#define KD 0

// SimpleRotary Library
// https://github.com/mprograms/SimpleRotary
// Pin A, Pin B, Button Pin
#define ENA_PIN 2
#define ENB_PIN 3
#define ENC_PIN 4
SimpleRotary rotary(ENA_PIN, ENB_PIN, ENC_PIN);


U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2(U8G2_R2,
                                      LCD_CLOCK,
                                      LCD_MOSI,
                                      LCD_CS,
                                      LCD_RS,
                                      LCD_RESET);

// Initialize the Thermocouple
Adafruit_MAX31855 ext1_thermocouple(MAXCLK, EXT1_MAXCS, MAXDO);
Adafruit_MAX31855 ext2_thermocouple(MAXCLK, EXT2_MAXCS, MAXDO);
Adafruit_MAX31855 int1_thermocouple(MAXCLK, INT1_MAXCS, MAXDO);

MUIU8G2 mui;

/*
  global variables which form the communication gateway between the user interface and the rest of the code
*/

double ext1_temperature = 0;
double ext2_temperature = 0;
double int1_temperature = 0;
double ext1_setPoint = 0;
double ext2_setPoint = 0;
double int1_setPoint = 0;
double ext1_outputVal = 0;
double ext2_outputVal = 0;
double int1_outputVal = 0;

uint16_t heater_idx = 0;

uint8_t num_value = 0;
uint8_t bar_value = 0;

struct ramp_rate_setpoint {
  double temperature, setPoint, outputVal;
  
};

AutoPID ext1_PID(&ext1_temperature, &ext1_setPoint, &ext1_outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPID ext2_PID(&ext2_temperature, &ext2_setPoint, &ext2_outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPID int1_PID(&int1_temperature, &int1_setPoint, &int1_outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);




unsigned long lastTempUpdate; //tracks clock time of last temp update


//call repeatedly in loop, only updates after a certain time interval
//returns true if update happened
bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    ext1_temperature = ext1_thermocouple.readCelsius(); //get temp reading
    ext2_temperature = ext2_thermocouple.readCelsius(); //get temp reading
    int1_temperature = int1_thermocouple.readCelsius(); //get temp reading
  if (isnan(ext1_temperature))
  {
     u8g2.print("ext1 T/C Problem");
  }
  if (isnan(ext2_temperature))
  {
     u8g2.print("ext2 T/C Problem");
  }
  if (isnan(int1_temperature))
  {
     u8g2.print("int1 T/C Problem");
  }
    
    lastTempUpdate = millis();

    return true;
  }
  return false;
}//void updateTemperature

/*
  list of heater names
*/
const char *heater[] = { "external 1", "External 2", "Internal 1"};
//set of operational states for the device
//
//
//
//
const char *heater_states[] = { "idle","preheat", "ramp-up", "operational" , "ramp-down" };


uint8_t show_temp_data(mui_t *ui, uint8_t msg) {
  if ( msg == MUIF_MSG_DRAW ) {
    u8g2_uint_t x = mui_get_x(ui);
    u8g2_uint_t y = mui_get_y(ui);
    u8g2.setCursor(x+5, y);
    u8g2.print("ext1:");
    u8g2.setCursor(x+50, y);
    u8g2.print(ext1_temperature);

    u8g2.setCursor(x+5, y+12);
    u8g2.print("ext2:");
    u8g2.setCursor(x+50, y+12);
    u8g2.print(ext2_temperature);

    u8g2.setCursor(x+5, y+24);
    u8g2.print("int1:");
    u8g2.setCursor(x+50, y+24);
    u8g2.print(int1_temperature);  
    //u8g2.print("=");  
    //u8g2.print(animals[animal_idx]);  
  }
  return 0;
}

uint16_t heater_states_get_param(void *data) {
  return sizeof(heater)/sizeof(*heater);    /* number of heaters */
}

const char *heater_states_get_str(void *data, uint16_t index) {
  return heater_states[index];
}

uint8_t mui_hrule(mui_t *ui, uint8_t msg) {
  if ( msg == MUIF_MSG_DRAW ) {
      u8g2.drawHLine(0, mui_get_y(ui), u8g2.getDisplayWidth());
  }
  return 0;
}

muif_t muif_list[] = {
  MUIF_U8G2_FONT_STYLE(0, u8g2_font_helvR08_tr),        /* regular font */
  MUIF_U8G2_FONT_STYLE(1, u8g2_font_helvB08_tr),        /* bold font */

  MUIF_RO("HR", mui_hrule),
  MUIF_U8G2_LABEL(),
  MUIF_RO("GP",mui_u8g2_goto_data),  
  MUIF_BUTTON("GC", mui_u8g2_goto_form_w1_pi),

  MUIF_U8G2_U8_MIN_MAX("NV", &num_value, 0, 99, mui_u8g2_u8_min_max_wm_mud_pi),
  MUIF_U8G2_U8_MIN_MAX_STEP("NB", &bar_value, 0, 16, 1, MUI_MMS_2X_BAR, mui_u8g2_u8_bar_wm_mud_pf),
  MUIF_U8G2_U16_LIST("NA", &heater_idx, NULL, heater_states_get_str, heater_states_get_param, mui_u8g2_u16_list_line_wa_mud_pi),

  /* register custom function to show the data */
  MUIF_RO("SH", show_temp_data), 

  /* a button for the menu... */
  MUIF_BUTTON("GO", mui_u8g2_btn_goto_wm_fi)  
};

fds_t fds_data[] = 

MUI_FORM(1)
MUI_STYLE(1)
MUI_LABEL(5, 8, "Stage2 Heater")
MUI_STYLE(0)
MUI_XY("HR", 0,11)
MUI_DATA("GP", 
    MUI_10 "Enter Data|"
    MUI_12 "Show Data")
MUI_XYA("GC", 5, 24, 0) 
MUI_XYA("GC", 5, 36, 1) 

MUI_FORM(10)
MUI_STYLE(1)
MUI_LABEL(5, 8, "Config Ramp Rates")
MUI_XY("HR", 0,11)
MUI_STYLE(0)
MUI_LABEL(5,23, "Num:")
MUI_LABEL(5,36, "Bar:")
MUI_LABEL(5,49, "Animal:")
MUI_XY("NV", 50, 23)
MUI_XY("NB", 50, 36)
MUI_XYA("NA", 50, 49, 44)
MUI_XYAT("GO", 114, 60, 1, " Ok ") 

MUI_FORM(12)
MUI_STYLE(1)
MUI_LABEL(5, 8, "Show Data")
MUI_XY("HR", 0,11)
MUI_STYLE(0)
MUI_XY("SH", 0, 23)
MUI_XYAT("GO", 114, 60, 1, " Ok ") 

;


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
    // Changing on a supposition that this line is an error - rlr
    // Udp.read(buffer, packetSize);
    Udp.read(packetBuffer, packetSize);
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

void setup() {


  
  
 // ext1_myPID.setBangBang(4);
 // ext1_myPID.setTimeStep(4000);
  
 // ext2_myPID.setBangBang(4);
 // ext2_myPID.setTimeStep(4000);
  
  //int1_myPID.setBangBang(4);
  //int1_myPID.setTimeStep(4000);
  
  
  //define rotary encoder pinout
  pinMode(ENA_PIN, INPUT);
  pinMode(ENB_PIN, INPUT);
  pinMode(ENC_PIN, INPUT);

  //define SSR configuration
  pinMode(EXT1_OUTPUT_PIN, OUTPUT);
  pinMode(EXT2_OUTPUT_PIN, OUTPUT);
  pinMode(INT1_OUTPUT_PIN, OUTPUT);
  //define SSR initial output
  digitalWrite(EXT1_OUTPUT_PIN, LOW);
  digitalWrite(EXT2_OUTPUT_PIN, LOW);
  digitalWrite(INT1_OUTPUT_PIN, LOW);
  //define display pinout
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);

  
  u8g2.begin();
  mui.begin(u8g2, fds_data, muif_list, sizeof(muif_list)/sizeof(muif_t));
  mui.gotoForm(/* form_id= */ 1, /* initial_cursor_position= */ 0);
 delay(500);
 
  if (!ext1_thermocouple.begin()) {
    u8g2.print("ext1 ERROR.");
    while (1) delay(10);
  }
  if (!ext2_thermocouple.begin()) {
    u8g2.print("ext2 ERROR.");
    while (1) delay(10);
  }
  if (!int1_thermocouple.begin()) {
    u8g2.print("int1 ERROR.");
    while (1) delay(10);
  }
  u8g2.print("DONE.");


    // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("starting"));

   // start the Ethernet
  if(!Ethernet.begin(mac))
  {
      u8g2.print("ext1 ERROR.");
    while (1) delay(10);
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
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
  

  // start UDP
  Udp.begin(localPort);
}

uint8_t is_redraw = 1;
uint8_t rotary_event = 0; // 0 = not turning, 1 = CW, 2 = CCW
uint8_t push_event = 0; // 0 = not pushed, 1 = pushed

void detect_events(void) {
  uint8_t tmp;
  
  // 0 = not pushed, 1 = pushed  
  tmp = rotary.push();
  if ( tmp != 0 )         // only assign the push event, never clear the event here
    push_event = tmp;
    
  // 0 = not turning, 1 = CW, 2 = CCW
  tmp = rotary.rotate();
  if ( tmp != 0 )       // only assign the rotation event, never clear the event here
    rotary_event = tmp;    
  
  
}
void handle_events(void) {
  // 0 = not pushed, 1 = pushed  
  if ( push_event == 1 ) {
      mui.sendSelect();
      is_redraw = 1;
      push_event = 0;
  }
  
  // 0 = not turning, 1 = CW, 2 = CCW
  if ( rotary_event == 1 ) {
    mui.nextField();
    is_redraw = 1;
    rotary_event = 0;
  }
  
  if ( rotary_event == 2 ) {
    mui.prevField();
    is_redraw = 1;
    rotary_event = 0;
  }    
}

void loop() {
  // put your main code here, to run repeatedly:

  updateTemperature();
 // setPoint = analogRead(POT_PIN);
  ext1_PID.run(); //call every loop, updates automatically at certain time interval
  ext2_PID.run();
  int1_PID.run();
  
  analogWrite(EXT1_OUTPUT_PIN, ext1_outputVal);
  analogWrite(EXT2_OUTPUT_PIN, ext2_outputVal);
  analogWrite(INT1_OUTPUT_PIN, int1_outputVal);
  
  //digitalWrite(LED_PIN, myPID.atSetPoint(1)); //light up LED when we're at setpoint +-1 degree
  if( ext1_PID.atSetPoint(1))
  {
  }
  if( ext2_PID.atSetPoint(1))
  {
  }
  if( int1_PID.atSetPoint(1))
  {
  }


    // send a reply to the IP address and port that sent us the packet we received
    //  Serial.println("calling getConfig()!");
  getConfig();
  sendData(NULL);
  //delay(20000);
  
  
  
  /* check whether the menu is active */
  if ( mui.isFormActive() ) {

    /* update the display content, if the redraw flag is set */
    if ( is_redraw ) {
      u8g2.firstPage();
      do {
          detect_events();
          mui.draw();
          detect_events();
      } while( u8g2.nextPage() );
      is_redraw = 0;                    /* clear the redraw flag */
    }

    detect_events();
    handle_events();
      
  } else {
      /* the menu should never become inactive, but if so, then restart the menu system */
      mui.gotoForm(/* form_id= */ 1, /* initial_cursor_position= */ 0);
  }
}
