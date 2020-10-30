#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <controller.h>
#include <stdio.h>
#include <iostream>
//#include <config.h>

namespace PIOC_Controller {

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


////////// HELPER FUNCTIONS //////////

  void printDebugValves(int tick, int i, int valve_bits, bool start, char name){
#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valves: ");
      Serial.print(name);
      Serial.print(" ");
      Serial.print(start);
      Serial.print(" ");
      Serial.println(tick);
#else
      printf("Valves: " BYTE_TO_BINARY_PATTERN , BYTE_TO_BINARY(valve_bits));
      printf(" %c%d t_%d\n", name, start, tick);
#endif
  }

  ////////// METHODS ///////////

  bool ValveController::tick(uint32_t tick) {
    for (uint8_t i = 0; i < numValves; i++){
      if ((*(valves+i)).start == tick){
        valveBits |= (*(valves+i)).pin;
        printDebugValves(tick, i, valveBits, true, (*(valves+i)).name);
      }

      if ((*(valves+i)).stop == tick){
        valveBits ^= (*(valves+i)).pin;
        printDebugValves(tick, i, valveBits, false, (*(valves+i)).name);
      }
    }

    return true;
  }

  bool ValveController::update(uint32_t msNow){
    // TODO: check bounds of msNow

    

    return true;
  }

  /*bool ValveController::setupValve(valve valve, uint8_t i){
    this->valves[i] = valve;
    return true;
  }

  bool ValveController::setupValves(valve valves[], uint8_t numValves){
    for (int i = 0; i < numValves; i++){
      this->valves[i] = valves[i];
    }
    return true;
  }*/

}