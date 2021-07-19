#ifndef MPR_PRESSURE
#define MPR_PRESSURE

#include <inttypes.h>

/*
Public Invention's Ox Project is an open source hardware design for an oxygen concentrator for use by field hospitals around the world. This design concept aims to design an oxygen concentrator that can be manufactured locally while overcoming challenges posed by human resources, hospital location (geographically), infrastructure and logistics; in addition, this project attempts minimum documentation of their design standards necessary for local approval whilst tackling regulatory requirements for medical devices. Copyright (C) 2021 Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free Firmware/Hardware designs: you can redistribute, use, study it and/or modify it under the terms of the CERN Open Hardware License Version 2 as published here: https://ohwr.org/cern_ohl_s_v2.txt

See the CERN Open Hardware License Version 2 for more details.
You should have received a copy of the CERN Open Hardware License Version 2 along with this program.  If not, see https://ohwr.org/cern_ohl_s_v2.txt.

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

namespace PIOC_Sensor {

    // Status byte flags from Table 14 MPR datasheet
    #define POWER_FLAG      0x40 //b01000000
    #define BUSY_FLAG       0x20 //b00100000
    #define ERROR_FLAG      0x04 //b00000100
    #define SATURATION_FLAG 0x01 //b00000001

    #define OUTPUT_CMD 0xAA

    // For transfer function A from MPR datasheet
    #define OUTPUT_MAX 0xE66666
    #define OUTPUT_MIN 0x19999A

    enum SensorStatus {
        OK,
        MISSED,
        ERROR
    };

    struct SensorState {
        uint8_t name;
        uint8_t addr;
        uint8_t minPSI;
        uint8_t maxPSI;
        uint8_t timeout;
        uint32_t pressure;
        SensorStatus status;
    };

    class MPR_Pressure {
        private:
            SensorState state;
        public:
            MPR_Pressure(uint8_t name = 0, uint8_t i2cAddress = 0x18, uint8_t minimumPSI = 0, uint8_t maximumPSI = 25, uint8_t readTimeout = 15){
                state.name = name;
                state.addr = i2cAddress; //0x18; // default I2C address
                state.minPSI = minimumPSI;
                state.maxPSI = maximumPSI;
                state.timeout = readTimeout;
                state.pressure = 0;
                state.status = OK;
            }
            bool setup();
            float readPressure();
            uint8_t readStatus();
            uint32_t getPressure();
            SensorState getState();
    };

}

#endif
