#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <chrono>
//#include <iostream>
#include <timer.h>

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

namespace PIOC_Timer {

    uint64_t timeSinceEpochMs() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    void Timer::update(){
        #ifdef ARDUINO
        this->msElapsed = (uint32_t)millis() - this->msStart;
        #else
        this->msElapsed = (uint32_t)(timeSinceEpochMs()) - this->msStart;
        #endif
    }

    uint32_t Timer::elapsed(){
        return msElapsed;
    }
}
