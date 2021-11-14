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

#include "task.h"
#include "util.h"
#include <cstdint>

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

const int MAX_TASKS = 5;

// TODO: abstract this
template<typename T, typename U>
struct Map {
    T key;
    U value;
};

class Scheduler {
    private:
        Task *_tasks[MAX_TASKS];
        Map<int, TaskId> index_id[MAX_TASKS];
        TaskId _currentRunningTaskId = 0;
        int _numberOfTasks = 0;
        bool _addToMap(int index, TaskId id);
        int _idToIndex(TaskId id);
    public:
        bool AddTask(Task *task, TaskId id, TaskPriority priority);
        TaskState RunNextTask(uint32_t msNow);
        TaskState RunTaskById(uint32_t msNow, TaskId id);
        TaskId GetRunningTaskId() const;
        Task *GetTaskById(TaskId id);
        void RemoveTaskById(TaskId id);
        void RemoveAllTasks();
        
        
        void ClockTick();
        void RaiseInterrupt();
        void AnalyzeTasks();
        void AutoRun();
};

}

#endif
