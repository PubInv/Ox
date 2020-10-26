#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "valve.h"
#include <stdio.h>
#include <iostream>
#include <config.h>
//#include <debug.h>

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

uint8_t valve_bits;

uint8_t valve_init()
{
  valve_bits = 0;
#ifdef ARDUINO
  /*for (uint8_t i = 0; i < NUM_VALVES; i++){
    pinMode(VALVES[i].pin, OUTPUT);
  }*/
  Serial.print("Valve init\n");
#else
  printf("Valve init\n");
#endif

  return 1;
}

bool valve_tick(uint32_t tick)
{
#ifdef ARDUINO
    Serial.print("Tick: ");
    Serial.print(tick);
#else
    //printf("tick %d\n", tick);
#endif
 
  // Process each valve
  for (uint8_t i = 0; i < NUM_VALVES; i++){
    if (VALVES[i].start == tick){
      valve_bits |= VALVES[i].pin;

#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, HIGH);
      Serial.print("Valve start: ");
      Serial.print(VALVES[i].name);
#else
      printf("tick %d", tick);
      printf(" Valve start %c", VALVES[i].name);
      printf(" bits " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(valve_bits));
#endif
    }

    if (VALVES[i].stop == tick){
      valve_bits ^= VALVES[i].pin;

#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valve stop: ");
      Serial.print(VALVES[i].name);
#else
      printf("tick %d", tick);
      printf(" Valve stop %c", VALVES[i].name);
      printf(" bits " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(valve_bits));
#endif
    }
  }

  return true;
}
