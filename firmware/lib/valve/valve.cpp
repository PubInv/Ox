#include <valve.h>
#include <stdio.h>
#include <iostream>
#include <config.h>

uint8_t valve_init()
{
  return 1;
}

bool valve_tick(uint32_t tick)
{
  std::cout << "tick " << tick ;

  // Process each valve
  for (uint8_t i = 0; i < NUM_VALVES; i++){
    if (VALVES[i].start == tick){
      std::cout << " Valve start " << VALVES[i].name;
    }
    if (VALVES[i].stop == tick){
      std::cout << " Valve stop " << VALVES[i].name;
    }
  }

  std::cout << "\n";
  return true;
}
