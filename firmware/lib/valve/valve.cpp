#include <valve.h>

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
