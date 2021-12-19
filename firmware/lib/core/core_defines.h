#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

typedef unsigned long Time;
typedef int TaskPriority;
typedef int TaskId;

enum class TaskState {
    Undefined = 0,
    Ready,
    Running,
    Blocked,
    Suspended,
    Error
};

#endif