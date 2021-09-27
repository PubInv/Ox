#ifndef MPR_PRESSRE
#define MPR_PRESSURE

#include <inttypes.h>

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
        MALFUNCTION
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
