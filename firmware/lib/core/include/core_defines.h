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
};

}

#endif
