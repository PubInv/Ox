/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#include <psa_task.h>
#include "psa_config.h"
#ifndef ARDUINO
#include <iostream>
#endif

//using namespace OxCore;

namespace OxApp
{
    // Helper MACRO to display bit pattern for debugging only
    //https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
    // printf("Some text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
    #define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
    #define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

    ValveController vc(&psaValveConfig[0], NUM_VALVES);

    bool PsaTask::_init()
    {
        OxCore::Debug<const char *>("PsaTask init\n");
        valveCycleTimer.Init(OxCore::Timer::TimeSinceEpochMs());
        return true;
    }

    bool PsaTask::_run() 
    {
        OxCore::Debug<const char *>("PsaTask run\n");
        OxCore::u32 elapsed = valveCycleTimer.Update();

        if (elapsed >= TOTAL_CYCLE_TIME)
        {
            valveCycleTimer.Reset();
            vc.resetValves();
        } else {
            vc.updateController(elapsed);
            _printValveState(vc.getValveBits());
        }

        return true;
    }

    void PsaTask::_printValveState(OxCore::u8 vs)
    {
#ifdef ARDUINO
        Serial.print("Valves: ");
        for (int b = 7; b >= 0; b--)
        {
            Serial.print(bitRead(vs, b));
        }
        Serial.println("");
#else
        printf("Valves: " BYTE_TO_BINARY_PATTERN , BYTE_TO_BINARY(vs));
#endif
    }

}
