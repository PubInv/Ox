#ifndef VALVE_H
#define VALVE_H

#include <inttypes.h>

struct valve {
  char name;
  uint8_t num;
  uint8_t status;
  uint8_t err;
  uint8_t pin;
  uint32_t start;
  uint32_t stop;
};
  
///// FUNCTIONS /////

uint8_t valve_init();
bool valve_tick(uint32_t step);

#endif
