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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>

namespace PIOC_Controller {

#define NUM_VALVES 4

  enum PIOCMode {
      STARTING,
      RUNNING,
      STOPPED,
      PAUSED,
      ERROR
  };

  struct PIOCState {
      PIOCMode mode;
      int totalRunTime;
  };

  struct Valve {
      char name;
      unsigned num;
      unsigned int state;
      unsigned int pin;
      unsigned int start;
      unsigned int stop;
  };

  class ValveController {
    private:
      PIOCState pioc_state;
      uint8_t valveBits;
      int numValves;
      Valve *valves;
    public:
        ValveController(Valve* v, int numValves) {
        valveBits = 0;
        pioc_state.mode = STARTING;
        pioc_state.totalRunTime = 0;
        valves = v;
        this->numValves = numValves;
      }
      void updateValves(uint32_t *msNow);
      bool updateController(unsigned int *msNow);
      bool resetValves();
      uint8_t getValveBits();
  };

}
#endif
