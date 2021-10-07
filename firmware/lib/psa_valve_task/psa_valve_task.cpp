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

#include <psa_valve_task.h>

namespace OxPSA
{

    bool PsaCycleTask::_init()
    {
        tLast = 0;
        valveCycle.Init(OxCore::TimeSinceEpochMs());
    }

    bool PsaCycleTask::_run() 
    {
        //std::cout << "Task A" << std::endl;
        valveCycle.Update();

        if (valveCycle.GetElapsed() >= tLast + TIME_STEP)
        {
            tLast = valveCycle.GetElapsed();
            vc.updateController(&tLast);
#ifdef ARDUINO
            uint8_t out = vc.getValveBits();
            shiftOutValves(out);
#endif
            _printValveState(vc.getValveBits());
        }
        else if (valveCycle.GetElapsed() >= TOTAL_CYCLE_TIME)
        {
            vc.resetValves();
            valveCycle.Init(OxCore::TimeSinceEpochMs());
            tLast = 0; // TODO: put this in the timer class
        }
    }

    void PsaCycleTask::_printValveState(uint8_t vs)
    {
#ifdef ARDUINO
        Serial.print("Valves: ");
        for (int b = 7; b >= 0; b--)
        {
            Serial.print(bitRead(vs, b));
        }
        Serial.println("");
#else
        // todo
#endif
    }

}
