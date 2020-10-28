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

#define NUM_VALVES 4

  valve VALVES[NUM_VALVES] = {
  { .name = 'A',
    .num = 0,
    .status = 0,
    .err = 0,
    .pin = 1,//CONTROL_VALVE_A,
    .start = 100,
    .stop = 6000 },
  { .name = 'B',
      .num = 1,
      .status = 0,
      .err = 0,
      .pin = 2,//CONTROL_VALVE_B,
      .start = 6000,
      .stop = 6400 },
  { .name = 'C',
      .num = 2,
      .status = 0,
      .err = 0,
      .pin = 4,//BALANCE_VALVE_A,
      .start = 6400,
      .stop = 12400 },
  { .name = 'D',
      .num = 3,
      .status = 0,
      .err = 0,
      .pin = 8,//BALANCE_VALVE_B,
      .start = 12400,
      .stop = 12900 }};

  ///////// HELPER FUNCTIONS //////////

  void printDebugValves(int tick, int i, int valve_bits, bool start){
#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valves: ");
      Serial.print(VALVES[i].name);
      Serial.print(" ");
      Serial.print(start);
      Serial.print(" ");
      Serial.println(tick);
#else
      printf("Valves: " BYTE_TO_BINARY_PATTERN , BYTE_TO_BINARY(valve_bits));
      printf(" %c%d t_%d\n", VALVES[i].name, start, tick);
#endif
  }

  ////////// METHODS ///////////

  bool ValveController::tick(uint32_t tick) {
    for (uint8_t i = 0; i < NUM_VALVES; i++){
      if (VALVES[i].start == tick){
        valveBits |= VALVES[i].pin;
        printDebugValves(tick, i, valveBits, true);
      }

      if (VALVES[i].stop == tick){
        valveBits ^= VALVES[i].pin;
        printDebugValves(tick, i, valveBits, false);
      }
    }

    return true;
  }

}