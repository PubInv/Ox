#ifndef STATE_H
#define STATE_H

#include <inttypes.h>

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

#endif