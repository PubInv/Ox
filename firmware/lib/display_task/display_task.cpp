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

#include <display_task.h>

namespace OxDisplay
{

    void DisplayTask::setup()
    {

#ifdef ARDUINO
        this->display = Ox_Display();
        this->display.displayInit();
        this->display.startScreen();
        delay(2000);
        this->display.debugScreen();
#endif
        /*// Test display layout and graph experiment
        this->display.drawButton();
        this->display.updateGraph();*/

        this->displayTick = 0;
    }

    void DisplayTask::action()
    {
#ifdef ARDUINO
        this->displayTick++; // TODO: make this a timer
        if (this->displayTick >= 10000)
        {
            // I dont there is a valid reference for this at the moment - BC
            //this->display.valveState(valveCycle.elapsed(), vc.getValveBits());
            this->displayTick = 0;
        }
#endif
    }

}
