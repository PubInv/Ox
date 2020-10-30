#ifndef CONTROLLER_H
#define CONTROLLER_H

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
    uint8_t name;
    uint8_t num;
    uint8_t state;
    uint8_t pin;
    uint32_t start;
    uint32_t stop;
  };

  class ValveController {
    private:
      PIOCState pioc_state;
      uint8_t valveBits;
      uint8_t numValves;
      valve *valves;
    public:
      ValveController(valve *v, int numValves) {
        valveBits = 0;
        pioc_state.mode = STARTING;
        pioc_state.totalRunTime = 0;
        valves = v;
        this->numValves = numValves;
      }
      bool update(uint32_t *msNow);
      //bool tick(uint32_t tick);
      //uint8_t updateValves(uint32_t msNow);
      
      //bool setupValve(valve valve, uint8_t i);
      //bool setupValves(valve valves[], uint8_t numValves);
  };

}
#endif
