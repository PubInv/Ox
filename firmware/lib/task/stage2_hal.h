#ifndef STAGE2_HAL_H
#define STAGE2_HAL_H

#include <machine.h>

class Stage2HAL : MachineHAL {
public:
  const int NUM_HEATERS = 3;
  const int HEATER_PINS[] = {51,50,49};
  bool init() override;
}

#endif
