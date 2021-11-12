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

#include "scheduler.h"

namespace OxCore {

bool Scheduler::_addToMap(int index, TaskId id) {
    if (CheckArrayBounds(index, MAX_TASKS)){
        return false;
    }
    index_id->key = index;
    index_id->value = id;
    return true;
}

int Scheduler::_idToIndex(TaskId id) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (index_id[i].value = id) {
            return index_id[i].key;
        }
    }
}

bool Scheduler::AddTask(Task *task, TaskId id, TaskPriority priority) {
    if (CheckArrayBounds(_numberOfTasks, MAX_TASKS)) {
        TaskState state = task->Init(id, priority);
        if (state == TaskState::Waiting) {
            _tasks[_numberOfTasks++] = task;
            return true;
        }
    }
    return false;
}

TaskState Scheduler::RunNextTask(uint32_t msNow) {
    //IncrementRunningTask();
    // TODO: use scheduling algorithm
    return _tasks[_currentRunningTask]->Run(msNow);
}

TaskState Scheduler::RunTaskById(uint32_t msNow, TaskId id) {
    int index = _idToIndex(id);
    _currentRunningTask = index;
    return _tasks[_currentRunningTask]->Run(msNow);
}

TaskId Scheduler::GetRunningTaskId() const {
    return _currentRunningTask;
}

Task* Scheduler::GetTaskById(TaskId id) {
    int index = _idToIndex(id);
    return *(_tasks + index);
}

void Scheduler::RemoveTaskById(TaskId id) {
    int index = _idToIndex(id);
    _tasks[index] = nullptr;
}

void Scheduler::RemoveAllTasks() {
    for (int i = 0; i < MAX_TASKS; i++) {
        _tasks[i] = nullptr;
    }
}

void Scheduler::StartSchedulerClock() {
    
}

void Scheduler::AutoRun() {
    
}

void Scheduler::RaiseInterrupt() {

}

}
