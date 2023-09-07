/* WatchDog_Due_Eval.ino

    By:   (Forrest) Lee Erickson
    Date: 20230904
    Date: 20230905 Make state persistant through reset. Using section (".noinit") did not work.
    Date: 20230906 Use FLASH storage for state to persist through reset.  Blink an LED. Counts resets.
    Date: 20230907 Add compile_date.  Move print program information into function. 
      - Add Serial Number. 
      - Press D12 switcd for flash update
*/
/* Copyright (C) 2023 Forrest Lee Erickson and Public Invention

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

#define BAUDRATE 115200

/* Watchdog */
const long WATCH_DOG_TIME = 1500; // mS. Verified this has period of about 1.5 seconds
char *resetTypes[] = { "general", "backup", "watchdog", "software", "user" };

void watchdogSetup() {
  watchdogEnable(WATCH_DOG_TIME);
}

/* Winking LED */
//Set LED for Uno or ESP32 Dev Kit on board blue LED.
//const int LED_BUILTIN = 2;    // ESP32 Kit
//const int LED_BUILTIN = 13;    //Not really needed for Arduino UNO it is defined in library
const int HIGH_TIME_LED = 900;
const int LOW_TIME_LED = 100;
unsigned long lastLEDtime = 0;
unsigned long nextLEDchange = 100; //time in ms.

/* Magic numbers, HARDWARE, Some input pins */
const int D12 = 12;     // An input, ???Make low to change state???
const int D7 = 7;     // An input. Make low to make watchdog time out.

/* Functions */

// Program information
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "WatchDog_Due_Eval.ino"
#define VERSION "; Rev: 0.6"  //
#define DEVICE_UNDER_TEST "Hardware: Due"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "

void printProgramInfo() {
  Serial.print(COMPANY_NAME);
  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.println(LICENSE);
  Serial.print("Compiled at: ");
  Serial.println(F(__DATE__ " " __TIME__) ); //compile date that is used for a unique identifier
}//end printProgramInfo()

// Code for Flash storage
#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;

// The struct of the configuration.
struct Configuration {
  uint8_t a;  //Reset count, from all causes.
  uint8_t b;  //Flash write count from program
  char* serial_Number; //Alpha numeric
  int32_t bigInteger;
  char* message;
  char c;
};

// initialize Flash storage struct
Configuration configuration;

// initialize global struct and index
byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
Configuration configurationFromFlash; // create a temporary struct

//If first run set up flash with defaults.  Prints state from flash
void CheckIfRunFirstTime() {
  /* Flash is erased every time new code is uploaded. Write the default configuration to flash if first time */
  // running for the first time?
  uint8_t codeRunningForTheFirstTime = dueFlashStorage.read(0); // flash bytes will be 255 at first run
  //  Serial.println(codeRunningForTheFirstTime);  //Uncomment to check the flash read.
  Serial.print("codeRunningForTheFirstTime: ");
  if (codeRunningForTheFirstTime != 0) {
    Serial.println("yes");
    /* OK first time running, set defaults */
    configuration.a = 1;
    configuration.b = 0;
    configuration.serial_Number = "NULL" ; //Default
    configuration.bigInteger = 1147483647; // my lucky number
    configuration.message = "Hello world!";
    configuration.c = 's';

    // write configuration struct to flash at adress 4
    byte b2[sizeof(Configuration)]; // create byte array to store the struct
    memcpy(b2, &configuration, sizeof(Configuration)); // copy the struct to the byte array
    dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash

    // write 0 to address 0 to indicate that it is not the first time running anymore
    dueFlashStorage.write(0, 0);
    Serial.println("Save configuration at first boot.");
    printConfigurationFromFlash();
  }
  else {
    Serial.println("no. Incrment boot and flash count.");
    //Gets the current configuration so we can increment reset count.
    memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct

    //Update only .a the boot count and .b the flash count.
    configuration.a = configurationFromFlash.a + 1;
    configuration.b = configurationFromFlash.b +1;  
    configuration.serial_Number = configurationFromFlash.serial_Number;
    configuration.bigInteger = configurationFromFlash.bigInteger;
    configuration.message = configurationFromFlash.message;
    configuration.c = configurationFromFlash.c;

    // write update configuration struct to flash at adress 4
    byte b2[sizeof(Configuration)]; // create byte array to store the struct
    memcpy(b2, &configuration, sizeof(Configuration)); // copy the struct to the byte array
    dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash

    Serial.println("Configuration at subsequent boot.");
    printConfigurationFromFlash();
  }
}// end CheckIfRunFirstTime

void printConfigurationFromFlash() {
  /* read configuration struct from flash */
  memcpy(&configurationFromFlash, b, sizeof(Configuration)); // copy byte array to temporary struct
  // print the content
  Serial.print("Reset #:");
  Serial.print(configurationFromFlash.a);
  Serial.print(" Flash write:");
  Serial.print(configurationFromFlash.b);
  Serial.print(" serial_Number:");
  Serial.print(configurationFromFlash.serial_Number);
  Serial.print(" bigInteger:");
  Serial.print(configurationFromFlash.bigInteger);
  Serial.print(" message:");
  Serial.print(configurationFromFlash.message);
  Serial.print(" c:");
  Serial.print(configurationFromFlash.c);
  Serial.println();
  Serial.println();
}// end readConfigurationFromFlash

void changeConfigurationInFlash() {
  /* change some values in the struct and write them back */
  configurationFromFlash.b = (configurationFromFlash.b + 1) ;
  configurationFromFlash.serial_Number = configurationFromFlash.serial_Number;  //NO change
  configurationFromFlash.bigInteger = configurationFromFlash.bigInteger + 1 ;

  // Change the message. Message alternates
  String message = configurationFromFlash.message;
  if (configurationFromFlash.message == "Hello world!")
    configurationFromFlash.message = "Hello Arduino Due!";
  else
    configurationFromFlash.message = "Hello world!";

  // write configuration struct to flash at adress 4
  byte b2[sizeof(Configuration)]; // create byte array to store the struct
  memcpy(b2, &configurationFromFlash, sizeof(Configuration)); // copy the struct to the byte array
  dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash
}//end changeConfigurationInFlash

void checkD7() {
  while (!digitalRead(D7)); //It's a trap!
}//end checkD7

void checkD12() {
  while (!digitalRead(D12)); //It's a trap!
}//end checkD12

// Checks if time to wink and update the system state and save to flash.
void wink_N_displayFromFlash() {
  //Wink the LED
  if (((millis() - lastLEDtime) > nextLEDchange) || (millis() < lastLEDtime)) {
    if (digitalRead(LED_BUILTIN) == LOW) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = HIGH_TIME_LED;
    } else {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      nextLEDchange = LOW_TIME_LED;
      if (!digitalRead(D12)) {  // Update flash only if D12 low.
        changeConfigurationInFlash();
        printConfigurationFromFlash(); //Display the new configuration
      }
    }
    lastLEDtime = millis();
  }//end LED wink
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  //Signal start of setup.

  Serial.begin(BAUDRATE);
  delay(500);  //CAUTION This delay was important for correct read of flash.
  printProgramInfo();

  //Configure Hardware
  pinMode(D7, INPUT_PULLUP);
  pinMode(D12, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);

  //Print out the reset reason
  uint32_t resetCause = rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos;
  Serial.print("ResetCause: ");
  Serial.println(resetTypes[resetCause]);

  //TODO make update FLASH for reset type counts.
  CheckIfRunFirstTime(); //If so set up flash with defaults.  Prints state from flash

  //  watchdogEnable(WATCH_DOG_TIME);  //Start the watchdog.
  watchdogSetup();

  digitalWrite(LED_BUILTIN, LOW);   //Signal end of setup.
}//end setup()

void loop() {
  // put your main code here, to run repeatedly:
  checkD7(); //Closing switch on D7 to GND will starve the dog.
  watchdogReset();
  wink_N_displayFromFlash(); // Checks if time to wink and update the system state and save to flash.
}//end loop()
