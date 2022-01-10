// Public Invention's Ox Project is an open source hardware design for an oxygen
// concentrator for use by field hospitals around the world. This team aims to
// design an oxygen concentrator that can be manufactured locally while overcoming
// challenges posed by human resources, hospital location (geographically),
// infrastructure and logistics; in addition, this project attempts the minimum
// documentation expected of their design for international approval whilst
// tackling regulatory requirements for medical devices. Copyright (C) 2021
// Robert Read, Ben Coombs, and Darío Hereñú.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include "solenoid_valve.h"
#include <core/error_handler.h>

namespace OxPSA {

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

    bool Valve::changeTiming(OxCore::u32 onTime, OxCore::u32 offTime){
        // TODO: error checking
        if (onTime < 0 || offTime < 0) {
            // Call error handler
        }

        state.onTime = onTime;
        state.offTime = offTime;
        return true;
    }

    bool Valve::forceValveTrigger(){
        return false;
    }
}
