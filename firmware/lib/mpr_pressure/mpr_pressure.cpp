/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#include <Arduino.h>
#include <mpr_pressure.h>
#include <Wire.h>
#include <cmath>

namespace OxSensor {

    bool MPR_Pressure::setup(){
        Wire.beginTransmission(state.addr);
        uint8_t err = Wire.endTransmission();
        if (err == 0){
            return true;
        } else {
            // TODO: display error?
            return false;
        }
    }

    float MPR_Pressure::readPressure(){
        // Reference: 6.4 of the MPR datasheet
        // Send command to cause device to exit Standby mode and enter Operating mode
        Wire.beginTransmission(state.addr);
        Wire.write(OUTPUT_CMD);
        Wire.write(0x00);
        Wire.write(0x00);
        Wire.endTransmission();

        // The first byte is the status flag
        // Wait for busy flag in the status byte to clear (option 1 from datasheet)
        uint8_t status = readStatus();
        uint8_t cnt = 0;
        while((status & BUSY_FLAG) && (status != 0xFF)) // TODO: need more checks?
        {
            delay(1);
            status = readStatus();
            if (cnt++ > state.timeout){ // Response should be about 5ms (from datasheet)
                return NAN;
            }
        }

        // Ready to get pressure reading. Request 4 bytes (1 status, 3 reading)
        Wire.requestFrom(state.addr,4);

        // Check first byte for errors:
        uint8_t err = Wire.read();
        if ((err & ERROR_FLAG) || (err & SATURATION_FLAG)){
            return NAN;
        }

        // Read the next 3 bytes as the pressure measurement (24-bit)
        uint32_t output = 0;
        for (uint8_t i = 0; i < 3; i++){
            output |= Wire.read(); // Read 1 byte
            if (i < 2){
                output = output << 8; // shift the first 2 bytes over
            }
        }

        // Using equation 2 in MPR datasheet, convert 24-bit output to float pressure
        return (((output - OUTPUT_MIN) * (state.maxPSI - state.minPSI)) / (OUTPUT_MAX - OUTPUT_MIN)) + state.minPSI;;
    }

    uint8_t MPR_Pressure::readStatus(){
        Wire.requestFrom(state.addr,1);
        return Wire.read();
    }

    uint32_t MPR_Pressure::getPressure(){
        return state.pressure;
    }

    SensorState MPR_Pressure::getState(){
        return state;
    }
}
