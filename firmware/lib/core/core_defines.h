#include "types.h"

#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

namespace OxCore {

typedef u32 TimeMs;
//typedef u8 TaskPriority;
typedef i32 TaskId;
//typedef 32 TaskPeriod;

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
    ExceededHigh = 5,
    ExceededLow = 10,
    Idle = 200,
    TimeRemaining = 250
};

enum class TaskPriority {
    High = 60,
    Medium = 70,
    Low = 80,
    Undefined = 255
};

struct TaskProperties {
    TaskId id;
    TaskPriority priority;
    TimeMs period;
};

}

#endif