#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "valve.h"
#include <stdio.h>
#include <iostream>
#include <config.h>
//#include <debug.h>

uint8_t bits;

uint8_t valve_init()
{

  bits = 0;
#ifdef ARDUINO
  /*for (uint8_t i = 0; i < NUM_VALVES; i++){
    pinMode(VALVES[i].pin, OUTPUT);
  }*/
  Serial.print("Valve init\n");
#else
  std::cout << "Valve init\n";
#endif

  return 1;
}

bool valve_tick(uint32_t tick)
{

    //uint8_t bits = 0;

#ifdef ARDUINO
    Serial.print("Tick: ");
    Serial.print(tick);
#else
    std::cout << "tick " << tick << "\n" ;
#endif
 
  // Process each valve
  for (uint8_t i = 0; i < NUM_VALVES; i++){
    if (VALVES[i].start == tick){
      bits &= VALVES[i].pin;

#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, HIGH);
      Serial.print("Valve start: ");
      Serial.print(VALVES[i].name);
#else
      std::cout << "tick " << tick ;
      std::cout << " Valve start " << VALVES[i].name;
      std::cout << " bits: " << bits;
      std::cout << " \n";
#endif
    }
    if (VALVES[i].stop == tick){
      uint8_t x = ~VALVES[i].pin;
      bits &= x;
#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valve stop: ");
      Serial.print(VALVES[i].name);
#else
      std::cout << "tick " << tick ;
      std::cout << " Valve stop " << VALVES[i].name;
      std::cout << " bits: " << bits;
      std::cout << " \n";
#endif
    }
  }

  //std::cout << "\n";
  return true;
}
