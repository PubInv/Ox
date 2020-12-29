#ifndef MPR_PRESSURE
#define MPR_PRESSURE

#include <inttypes.h>

namespace PIOC_Sensor {
    enum SensorStatus {
        OK,
        MISSED,
        ERROR
    };

    enum PressureUnits {
        Pa,
        Bar,
        PSI
    };

    struct SensorState {
        uint8_t name;
        uint8_t pin;
        uint32_t lastMs;
        uint32_t pressure;
        SensorStatus status;
    };

    class MPR_Pressure {
        private:
            SensorState state;
            PressureUnits units;
        public:
            MPR_Pressure(uint8_t name, uint8_t pin, PressureUnits pu){
                state.name = name;
                state.pin = pin;
                state.lastMs = 0;
                state.pressure = 0;
                state.status = OK;
                units = pu;
            }
            bool update(uint32_t msNow);
            uint32_t getPressure();
            SensorStatus getStatus();
            SensorState getState();
            bool setUnits(PressureUnits pu);
            PressureUnits getUnits();
    };

}

#endif