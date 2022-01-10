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

#include <core.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace OxApp {

#define NUM_VALVES 4

  enum class OxMode {
      STARTING,
      RUNNING,
      STOPPED,
      PAUSED,
      ERROR
  };

  struct OxState {
      OxMode mode;
      OxCore::u32 totalRunTime;
  };

  struct ValveConfig {
      char name;
      OxCore::u32 num;
      OxCore::u32 state;
      OxCore::u32 pin;
      OxCore::u32 start;
      OxCore::u32 stop;
  };

  class ValveController {
    private:
      OxState _oxState;
      OxCore::u8 valveBits;
      OxCore::u32 numValves;
      ValveConfig *valves;
    public:
        ValveController(ValveConfig* v, OxCore::u32 numValves) {
        valveBits = 0;
        _oxState.mode = OxMode::STARTING;
        _oxState.totalRunTime = 0;
        valves = v;
        this->numValves = numValves;
      }
      void updateValves(OxCore::u32 &msNow);
      bool updateController(OxCore::u32 &msNow);
      bool resetValves();
      OxCore::u8 getValveBits();
  };

}
#endif
