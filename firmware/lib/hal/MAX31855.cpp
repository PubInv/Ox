// Copyright (C) 2021
// Lawrence Kincheloe, Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any 4ater version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifdef ARDUINO
#include <Arduino.h>
#endif 

#include <SPI.h>

#include <Adafruit_MAX31855.h>

#include <core.h>


#define TEMPERATURE_PRECISION 9

namespace Temperature {

  // TODO: Turn this into a Loop to support any number of thermocouples.
  // Print out an ordering so we can match ID against index
  // arrays to hold device Chip Select
  int8_t  postHeaterThermometer = 0;
  int8_t  postGetterThermometer = 0;
  int8_t  postStackThermometer  = 0;

  MAX31855Temperature::MAX31855Temperature(SensorConfig &config) {
	
    thermocouple.begin();
    // Pass our oneWire reference to Dallas Temperature.

   // sensors.setOneWire(&oneWire);

      // Start up the library
  //sensors.begin();

  // locate devices on the bus
  //Serial.print("Locating devices...");
  //Serial.print("Found ");
 // Serial.print(sensors.getDeviceCount(), DEC);
 // Serial.println(" devices.");

  // report parasite power requirements
  //Serial.print("Parasite power is: ");
  //if (sensors.isParasitePowerMode()) Serial.println("ON");
  //else Serial.println("OFF");

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //postHeaterThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //postGetterThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
 // if (!sensors.getAddress(postHeaterThermometer, 0)) Serial.println("Unable to find address for Device 0");
 // if (!sensors.getAddress(postGetterThermometer, 1)) Serial.println("Unable to find address for Device 1");
  //  if (!sensors.getAddress(postStackThermometer, 2)) Serial.println("Unable to find address for Device 2");

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them.  It might be a good idea to
  // check the CRC to make sure you didn't get garbage.  The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to postHeaterThermometer
  //if (!oneWire.search(postHeaterThermometer)) Serial.println("Unable to find address for postHeaterThermometer");
  // assigns the seconds address found to postGetterThermometer
  //if (!oneWire.search(postGetterThermometer)) Serial.println("Unable to find address for postGetterThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(postHeaterThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(postGetterThermometer);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(postStackThermometer);
  Serial.println();

  // set the resolution to 9 bit
  sensors.setResolution(postHeaterThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(postGetterThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(postStackThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(postHeaterThermometer), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(postGetterThermometer), DEC);
  Serial.println();

  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(postStackThermometer), DEC);
  Serial.println();
  }

  MAX31855Temperature::MAX31855Temperature(SensorConfig &config) {
	  
	// Initialize the Thermocouple
	Adafruit_MAX31855 thermocouple(MAXCLK, config->pin, MAXDO);
    
  }



  // function to print a device address
  void MAX31855Temperature::printAddress(DeviceAddress deviceAddress)
  {
    for (uint8_t i = 0; i < 8; i++)
      {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
      }
  }

  float MAX31855Temperature::ReadTemperature() {
    this->sensors.requestTemperatures(); // Send the command to get temperatures
    return GetTemperature(0);
  }
  float MAX31855Temperature::GetTemperature() {
    return GetTemperature(0);
  }
  float MAX31855Temperature::GetTemperature(int idx) {
    float tempC = this->sensors.getTempCByIndex(idx);

    if (tempC != DEVICE_DISCONNECTED_C)
      {
      }
    else
      {
        Serial.print(F("Error: Could not read temperature data: "));
        Serial.println(idx);
      }
    return tempC;

  }
  SensorConfig MAX31855Temperature::GetConfig() const {
    return _config;
  }
}
#endif
