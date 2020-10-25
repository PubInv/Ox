#ifndef VALVE_H
#define VALVE_H

#include <inttypes.h>
//#include <config.h>

// start and stop times must be a multiple of TIME_STEP!
struct valve {
  char name;
  uint8_t num;
  uint8_t status;
  uint8_t err;
  uint8_t pin;
  uint32_t start;
  uint32_t stop;
};

//uint16_t cycle_time;
  
//extern valve valves[NUM_VALVES];

uint8_t valve_init();
bool valve_tick(uint32_t step);

#endif
