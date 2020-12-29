#include <Arduino.h>
#include <mpr_pressure.h>
#include <Wire.h>
#include <cmath>

namespace PIOC_Sensor {

    bool MPR_Pressure::setup(){
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();
        if (err == 0){
            return true;
        } else {
            // TODO: display error
            return false;
        }
    }

    float MPR_Pressure::readPressure(){
        // Reference: 6.4 of the MPR datasheet
        // Send command to cause device to exit Standby mode and enter Operating mode
        Wire.beginTransmission(addr);
        Wire.write(OUTPUT_CMD);
        Wire.write(0x00);
        Wire.write(0x00);
        Wire.endTransmission();

        // The first byte is the status flag
        // Wait for busy flag in the status byte to clear (option 1 from datasheet)
        uint8_t status = readStatus();
        while((status & BUSY_FLAG) && (status != 0xFF))
        {
            delay(1);
            status = readStatus();
        }

        // Ready to get pressure reading. Request 4 bytes (1 status, 3 reading)
        Wire.requestFrom(addr,4);

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
        return (((output - OUTPUT_MIN) * (maxPSI - minPSI)) / (OUTPUT_MAX - OUTPUT_MIN)) + minPSI;;
    }

    uint8_t MPR_Pressure::readStatus(){
        Wire.requestFrom(addr,1);
        return Wire.read();
    }

    uint32_t MPR_Pressure::getPressure(){
        return state.pressure;
    }
    
    SensorState MPR_Pressure::getState(){
        return state;
    }
    
    bool MPR_Pressure::setUnits(PressureUnits pu){
        units = pu;
        return true;
    }
    
    PressureUnits MPR_Pressure::getUnits(){
        return units;
    }


}