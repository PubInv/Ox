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
      uint32_t run_time;
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

    public:
      ValveController() {
        valveBits = 0;
      }
      bool tick(uint32_t tick);
  };

}
#endif
