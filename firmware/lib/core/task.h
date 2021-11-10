/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef TASK_H
#define TASK_H

//#include <cstdint>
#include <iostream>

namespace OxCore {

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

typedef unsigned long Time;
typedef int TaskPriority;
typedef int TaskId;

class Task {
    private:
        bool _initialized;
        virtual bool _init() = 0;
        virtual bool _run() = 0;
    protected:
        TaskId _id;
        TaskPriority _priority;
        TaskState _state;
        Time _last_run;
    public:
        Task() {
            _state = TaskState::Undefined;
            _initialized = false;
            _id = -1;
            _priority = -1;
            _last_run = 0;
        };
        virtual ~Task() = default;
        // Cannot copy class
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;
        // Cannot move class
        Task(Task&&) = delete;
        Task& operator=(Task&&) = delete;

        TaskState Init(TaskId id, TaskPriority priority);
        TaskState Run(Time now);
        TaskState Wait(Time now);
        bool Callback(char *message);
        int GetId() const;
        int GetPriority() const;
        TaskState GetState() const;
};

}

#endif
