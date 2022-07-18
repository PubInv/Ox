// Copyright (C) 2021
// Robert Read, Ben Coombs.

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
#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif
#include <DS18B20_temperature.h>

#include <core.h>


namespace Temperature {
  DS18B20Temperature::DS18B20Temperature() {
    oneWire.begin(THERMOCOUPLE_PIN);
    // Pass our oneWire reference to Dallas Temperature.

    sensors.setOneWire(&oneWire);
  }

  DS18B20Temperature::DS18B20Temperature(SensorConfig &config) {
    Serial.print(F("Debug DS18B20 Constructed! "));
    oneWire.begin(THERMOCOUPLE_PIN);
    // Pass our oneWire reference to Dallas Temperature.

    sensors.setOneWire(&oneWire);
  }


  float DS18B20Temperature::ReadTemperature() {
        OxCore::Debug<const char *>("BBB: ");
    this->sensors.requestTemperatures(); // Send the command to get temperatures
        OxCore::Debug<const char *>("CCC: ");
    return GetTemperature(0);
  }
  float DS18B20Temperature::GetTemperature() {
    return GetTemperature(0);
  }
  float DS18B20Temperature::GetTemperature(int idx) {
    OxCore::Debug<const char *>("DS18B20 XXX ");
    Serial.print(F("Debug DS18B20 Get! "));
    float tempC = this->sensors.getTempCByIndex(idx);

    if (tempC != DEVICE_DISCONNECTED_C)
      {
        Serial.print(F("Debug tempC: "));
        Serial.println(idx);
        Serial.println(tempC);
      }
    else
      {
        Serial.print(F("Error: Could not read temperature data: "));
        Serial.println(idx);
      }
    return tempC;

  }
  SensorConfig DS18B20Temperature::GetConfig() const {
    return _config;
  }
}
#endif
