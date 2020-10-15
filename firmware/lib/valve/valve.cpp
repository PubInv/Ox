#include <valve.h>
#include <stdio.h>
#include <iostream>

valve VALVES[NUM_VALVES] = {
  { .name = 'A',
    .num = 0,
    .status = 0,
    .err = 0,
    .pin = 12,
    .start = 0,
    .stop = 6000 },
  { .name = 'B',
      .num = 0,
      .status = 0,
      .err = 0,
      .pin = 12,
      .start = 6000,
      .stop = 6400 },
  { .name = 'C',
      .num = 0,
      .status = 0,
      .err = 0,
      .pin = 12,
      .start = 6400,
      .stop = 12400 },
  { .name = 'D',
      .num = 0,
      .status = 0,
      .err = 0,
      .pin = 12,
      .start = 12400,
      .stop = 12900 }};

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
