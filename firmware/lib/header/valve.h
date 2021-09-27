#ifndef VALVE_H
#define VALVE_H

#include <inttypes.h>

namespace PIOC_Valve {

    enum ValveStatus {
        OK,
        MISSED,
        MALFUNCTION
    };

    struct ValveState {
        uint8_t name;
        uint8_t pin;
        uint32_t onTime; //ms the valve will be on
        uint32_t offTime; //ms the valve will be off
        uint32_t msLast;
        ValveStatus status;
        bool isOn;
    };

    class Valve {
        private:
            ValveState state;
        public:
            Valve(uint8_t name, uint8_t pin, uint32_t onTime, uint32_t offTime){
                state.name = name;
                state.pin = pin;
                state.onTime = onTime;
                state.offTime = offTime;
                state.msLast = 0;
                state.status = OK;
                state.isOn = false;
            }
            bool update(uint32_t msNow);
            ValveStatus getValveStatus();
            bool changeTiming(uint32_t onTime, uint32_t offTime);
            bool forceValveTrigger();
    };
    
}

#endif