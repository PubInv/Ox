/*
  File: Dallas_Tester.ino modified
  By: (Forrest) Lee Erickson
  Date: 20230914
  Use the serial plotter to view results
*/
#define COMPANY_NAME "pubinv.org "
#define PROG_NAME "FAST_TEMP_PLOT"
#define VERSION ";_Rev_0.1"
#define DEVICE_UNDER_TEST "Hardware: Mockup Of Maryville"  //A model number
#define LICENSE "GNU Affero General Public License, version 3 "


#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
//#define ONE_WIRE_BUS 5
#define ONE_WIRE_BUS 5  // But port 5 on Controller v1.
#define BLOWER_PWM_PIN 9

#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

int numberToPrint = 1;

DeviceAddress tempDeviceAddress[3]; // We'll use this variable to store a found device address


// I suspect this is a major problem; my goal here is to have two programs that
// demonstrate the difference of using the device id vs. the address
const bool USE_INDEX = false;

unsigned long good_reads;
unsigned long bad_reads;

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
    {
      if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
    }
}


void setup(void)
{
  // start serial port
  Serial.begin(115200);
  delay(500);
  Serial.print("TC0, TC1, TC2, Ratio");
  //  Serial.print(PROG_NAME);
  //  Serial.println(VERSION);

  pinMode(BLOWER_PWM_PIN, OUTPUT);
  analogWrite(BLOWER_PWM_PIN, 153);



  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  sensors.getAddress(tempDeviceAddress[0], 0);
  sensors.getAddress(tempDeviceAddress[1], 1);
  sensors.getAddress(tempDeviceAddress[2], 2);

  //Print some status
  //  // locate devices on the bus
  //  Serial.print("Locating devices...");
  //  Serial.print("Found ");
  //  Serial.print(numberOfDevices, DEC);
  //  Serial.println(" devices.");
  //
  //  // report parasite power requirements
  //  Serial.print("Parasite power is: ");
  //  if (sensors.isParasitePowerMode()) Serial.println("ON");
  //  else Serial.println("OFF");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
    {
      // Search the wire for address

      //    if (sensors.getAddress(tempDeviceAddress, i))

      if (false)
        {
          Serial.print("Found device ");
          Serial.print(i, DEC);
          Serial.print(" with address: ");
          printAddress(tempDeviceAddress[i]);
          Serial.println();

          Serial.print("Setting resolution to ");
          Serial.println(TEMPERATURE_PRECISION, DEC);

          // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
          sensors.setResolution(tempDeviceAddress[i], TEMPERATURE_PRECISION);

          Serial.print("Resolution actually set to: ");
          Serial.print(sensors.getResolution(tempDeviceAddress[i]), DEC);
          Serial.println();
        } else {
        //      Serial.print("Found ghost device at ");
        //      Serial.print(i, DEC);
        //      Serial.print(" but could not detect address. Check power and cabling");
      }
    }

}

// // function to print the temperature for a device
// void printTemperature(DeviceAddress deviceAddress)
// {

//   // method 2 - faster
//   float tempC = sensors.getTempC(deviceAddress);
//   if (tempC == DEVICE_DISCONNECTED_C)
//     {
//       //      Serial.println("Error: Could not read temperature data");
//       return;
//     }
//   Serial.print(tempC);
// }

// // function to print the temperature for a device
// void printTemperatureByIndex(int idx)
// {
//   float tempC = sensors.getTempCByIndex(idx);
//   if (tempC == DEVICE_DISCONNECTED_C)
//     {
//       //      Serial.println("Error: Could not read temperature data");
//       return;
//     }
//   Serial.print(tempC);
// }


float GetTemperature(int idx) {
  float tempC;
  if (USE_INDEX) {
    tempC = sensors.getTempCByIndex(idx);
  } else {
    tempC = sensors.getTempC(tempDeviceAddress[idx]);
  }
  if (tempC != DEVICE_DISCONNECTED_C)
    {
      good_reads++;
    }
  else
    {
      bad_reads++;
      //      Serial.print(F("Error: Could not read temperature data: "));
      //      Serial.println(idx);
    }
  return tempC;

}
float ReadTemperature() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  return GetTemperature(0);
}
float GetTemperature() {
  return GetTemperature(0);
}
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //  Serial.println("DONE");

  ReadTemperature();

  // Loop through each device, print out temperature data
  for (int i = 0; i < numberToPrint; i++)
    {

      if (USE_INDEX) {
        float temp = GetTemperature(i);
        Serial.print(temp);
      } else {
        float temp = GetTemperature(i);
        Serial.print(temp);
      }
      Serial.print(", ");
    }
  Serial.print(1000.0 * ((float) bad_reads/ (float) good_reads));
               Serial.print(", ");
  //else ghost device! Check your power requirements and cabling
  Serial.println(); //end of line
}//end loop()
