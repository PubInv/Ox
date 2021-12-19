#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

typedef unsigned long Time;
typedef int TaskPriority;
typedef int TaskId;

enum class TaskState {
    Undefined = 0,
    Initializing,
    InitFailed,
    Waiting,
    Running,
    RunSuccess,
    RunFailed,
    Terminating,
    Terminated,
    Failed,
};

#endif