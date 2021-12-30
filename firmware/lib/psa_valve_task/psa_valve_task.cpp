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
#include <types.h>
#include <iostream>

using namespace OxCore;

namespace OxPSA
{

    OxPSA::ValveConfig valveArray[NUM_VALVES] = {
        { 'A', 0, 0, 1, 100, 4000, },
        { 'B', 1, 0, 2, 4000, 8000, },
        { 'C', 2, 0, 4, 3700, 4000, },
        { 'D', 3, 0, 8, 7700, 8000, }};

    ValveController vc(&valveArray[0], NUM_VALVES);

    bool PsaValveTask::_init()
    {
        std::cout << "Init PSA valve task\n";
        tLast = 0;
        valveCycleTimer.Init(OxCore::Timer::TimeSinceEpochMs());
        return true;
    }

    bool PsaValveTask::_run() 
    {
        //std::cout << "Task A" << std::endl;
        u32 elapsed = valveCycleTimer.Update();

        if (elapsed >= tLast + TIME_STEP)
        {
            tLast = valveCycleTimer.GetElapsed();
            vc.updateController(&tLast);
#ifdef ARDUINO
            uint8_t out = vc.getValveBits();
            shiftOutValves(out);
#endif
            _printValveState(vc.getValveBits());
        }
        else if (valveCycleTimer.GetElapsed() >= TOTAL_CYCLE_TIME)
        {
            vc.resetValves();
            valveCycleTimer.Init(OxCore::Timer::TimeSinceEpochMs());
            tLast = 0; // TODO: put this in the timer class
        }

        return true;
    }

    void PsaValveTask::_printValveState(uint8_t vs)
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
