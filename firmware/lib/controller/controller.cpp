#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <controller.h>
#include <stdio.h>
#include <iostream>
//#include <config.h>

namespace PIOC_Controller {

  valve VALVES[4] = {
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

  bool ValveController::tick(uint32_t tick) {
    for (uint8_t i = 0; i < 4; i++){
      if (VALVES[i].start == tick){
        valve_bits |= VALVES[i].pin;
      }

      if (VALVES[i].stop == tick){
        valve_bits ^= VALVES[i].pin;
      }
    }

    return true;
  }

}

//https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
// printf("Some text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
/*#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

*/

/*
#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, HIGH);
      Serial.print("Valve start: ");
      Serial.print(VALVES[i].name);
#else
      printf("tick %d", tick);
      printf(" Valve start %c", VALVES[i].name);
      printf(" bits " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(valve_bits));
#endif*/

/*#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valve stop: ");
      Serial.print(VALVES[i].name);
#else
      printf("tick %d", tick);
      printf(" Valve stop %c", VALVES[i].name);
      printf(" bits " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(valve_bits));
#endif*/