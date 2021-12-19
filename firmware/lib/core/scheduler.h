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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "collections/map.h"
#include "types.h"
#include "task.h"

namespace OxCore {

// This is a lightweight co-operative scheduler.
// There is no overhead in context switching,
// there are some conditions:
// - Tasks must 'yield' by returning 
//   true (success) or false (failure).
// - Tasks should be small, fast and non-blocking.
// - Interrupts are handled by the scheduler.

// [ Task1, Task2, Task3, Task4, Empty ]
//            ^             ^      ^
//         current        number  max

const i32 MAX_TASKS = 5;

class Scheduler {
    private:
        OxCollections::Map<TaskId, Task*, MAX_TASKS> map;
        TaskId _currentRunningTaskId = 0;
        i32 _numberOfTasks = 0;
    public:
        bool AddTask(Task *task, TaskId id, TaskPriority priority);
        TaskState RunNextTask(u32 msNow);
        TaskState RunTaskById(u32 msNow, TaskId id);
        TaskId GetRunningTaskId() const;
        Task *GetTaskById(TaskId id);
        void RemoveTaskById(TaskId id);
        void RemoveAllTasks();
};

}

#endif
