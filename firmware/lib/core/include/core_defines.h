#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#endif

namespace OxCore {

typedef int32_t TimeMs;
typedef int32_t TaskId;

enum class TaskState {
    Undefined = 0,
    Ready,
    Running,
    Blocked,
    Suspended,
    Error
};

enum class TaskPriorityOS {
    Kernal = 0,
    Idle = 250,
};

enum class TaskPriority {
    High = 60,
    Medium = 70,
    Low = 80,
    Undefined = 255
};

struct TaskProperties {
    const char * name;
    TaskId id;
    TaskPriority priority;
    TimeMs period;
  // We place a void pointer here to hold a pointer
  // to the state and configuration of the machine under control.
  // In gneral, tasks will need to know this. However,
  // I don't want to go so far as the create type correctness for this.
  // so a task that uses this point is responsible for correctly
  // casting it to the correct type. This let's this code remain
  // absolutely "core" functionality, and locallize all code
  // into specific task overrides.
  void* state_and_config;
};

}

#endif
