#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>

namespace PIOC_Controller {

#define NUM_VALVES 4

  enum PIOCMode {
      STARTING,
      RUNNING,
      STOPPED,
      PAUSED,
      ERROR
  };

  struct PIOCState {
      PIOCMode mode;
      int totalRunTime;
  };

  struct Valve {
      char name;
      unsigned num;
      unsigned int state;
      unsigned int pin;
      unsigned int start;
      unsigned int stop;
  };

  class ValveController {
    private:
      PIOCState pioc_state;
      uint8_t valveBits;
      int numValves;
      Valve *valves;
    public:
        ValveController(Valve* v, int numValves) {
        valveBits = 0;
        pioc_state.mode = STARTING;
        pioc_state.totalRunTime = 0;
        valves = v;
        this->numValves = numValves;
      }
      void updateValves(uint32_t *msNow);
      bool updateController(unsigned int *msNow);
      bool resetValves();
      uint8_t getValveBits();
  };

}
#endif