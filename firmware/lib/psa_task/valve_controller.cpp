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

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

#include <valve_controller.h>

namespace OxApp {

  void ValveController::updateValves(OxCore::u32 &msNow){
    // TODO: shouldn't be using pointers here
    for (OxCore::u8 i = 0; i < numValves; i++) {
      if (((*(valves+i)).state == 0) && (msNow >= (*(valves+i)).start)) {
          (*(valves+i)).state = 1;
          valveBits |= (*(valves+i)).pin;
      } else if (((*(valves+i)).state == 1) && (msNow >= (*(valves+i)).stop)){
        (*(valves+i)).state = 2;
        valveBits ^= (*(valves+i)).pin;
      } else {
        // Do nothing - TODO: undefined state?
      }
    }
  }

  bool ValveController::resetValves(){
    for (int i = 0; i < numValves; i++){
      (*(valves+i)).state = 0;
    }
    valveBits = 0;
    return true;
  }

  bool ValveController::updateController(OxCore::u32 &msNow){
    updateValves(msNow);
    OxCore::Debug<const char *>("ValveController::updateController\n");
    return true;
  }

  OxCore::u8 ValveController::getValveBits(){
    return valveBits;
  }

}
