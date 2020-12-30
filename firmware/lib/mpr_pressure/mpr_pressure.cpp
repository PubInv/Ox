#include <Arduino.h>
#include <mpr_pressure.h>
#include <Wire.h>
#include <cmath>

namespace PIOC_Sensor {

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