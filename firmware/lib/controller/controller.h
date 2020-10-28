#ifndef VALVE_H
#define VALVE_H

#include <inttypes.h>

namespace PIOC_Controller {
  
  enum pioc_mode {
      STARTING,
      RUNNING,
      STOPPED,
      PAUSED,
      ERROR
  };

  struct pioc_state {
      pioc_mode mode;
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
      uint8_t valve_bits;

    public:
      ValveController() {
        valve_bits = 0;
      }
      bool tick(uint32_t tick);
  };

}
#endif
