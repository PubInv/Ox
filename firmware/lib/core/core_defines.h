#include "types.h"

#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

namespace OxCore {

typedef u32 TimeMs;
typedef i32 TaskPriority;
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

struct TaskProperties {
    TaskId id;
    TaskPriority priority;
    TimeMs period;
    bool hardTiming;
};

}

#endif