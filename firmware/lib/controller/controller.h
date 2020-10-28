#ifndef VALVE_H
#define VALVE_H

#include <inttypes.h>

namespace PIOC_Controller {
  
  enum PIOCMode {
      STARTING,
      RUNNING,
      STOPPED,
      PAUSED,
      ERROR
  };

  struct PIOCState {
      PIOCMode mode;
      uint32_t totalRunTime;
  };

  struct valve {
    char name;
    uint8_t num;
    uint8_t status;
    uint8_t err;
    char pin;
    uint32_t start;
    uint32_t stop;
  };

  class ValveController {
    private:
      uint8_t valveBits;
      //uint8_t timeStep;
      PIOCState state;
      valve *valves;
      int numValves;
    public:
      ValveController(valve *v, int numValves) {
        valveBits = 0;
        state.mode = STARTING;
        state.totalRunTime = 0;
        valves = v;
        this->numValves = numValves;
      }
      bool tick(uint32_t tick);
      //bool setupValve(valve valve, uint8_t i);
      //bool setupValves(valve valves[], uint8_t numValves);
  };

}
#endif
