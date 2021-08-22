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
#endif

#include <controller.h>
#include <stdio.h>
#include <iostream>
#include <inttypes.h>

namespace Ox_Controller {

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


////////// FUNCTIONS //////////

  void printDebugValves(uint32_t tick, int valve_bits, bool start, char name){
#ifdef ARDUINO
      //Serial.print(tick);
#else
      printf("Valves: " BYTE_TO_BINARY_PATTERN , BYTE_TO_BINARY(valve_bits));
      printf(" %c%d t_%d\n", name, start, tick);
#endif
  }

  void ValveController::updateValves(uint32_t *msNow){
    for (uint8_t i = 0; i < numValves; i++){
      if (((*(valves+i)).state == 0) && (*msNow >= (*(valves+i)).start)) {
          (*(valves+i)).state = 1;
          valveBits |= (*(valves+i)).pin;
      } else if (((*(valves+i)).state == 1) && (*msNow >= (*(valves+i)).stop)){
        (*(valves+i)).state = 2;
        valveBits ^= (*(valves+i)).pin;
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

  ////////// METHODS ///////////

  bool ValveController::updateController(uint32_t *msNow){
    // TODO: error checking: msNow > msLast, msNow < msLast + tolerance

    updateValves(msNow);

    printDebugValves(*msNow, valveBits, false, 'V');

    return true;
  }

  uint8_t ValveController::getValveBits(){
    return valveBits;
  }

}
