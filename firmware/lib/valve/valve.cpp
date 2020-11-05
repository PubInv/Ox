#include <valve.h>

namespace PIOC_Valve {

    bool Valve::update(uint32_t msNow){
        // Bistable timing
        //
        //          ___off___
        //         |         |
        // ___on___|         |
        //

        if (state.isOn && (msNow - state.msLast >= state.onTime)){
            // Turn off
            state.isOn = false;
            state.msLast = msNow;
            return true;
        }
        else if (!state.isOn && (msNow - state.msLast >= state.offTime)){
            // Turn on
            state.isOn = true;
            state.msLast = msNow;
            return true;
        }
        else {
            return false;
        }
    }

    ValveStatus Valve::getValveStatus(){
        return state.status;
    }

    bool Valve::changeTiming(uint32_t onTime, uint32_t offTime){
        // TODO: error checking

        state.onTime = onTime;
        state.offTime = offTime;
        return true;
    }

    bool Valve::forceValveTrigger(){
        return false;
    }
}