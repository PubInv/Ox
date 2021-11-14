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
    if (WithinArrayBounds(index, MAX_TASKS)) {
        index_id->key = index;
        index_id->value = id;
        return true;
    } else {
        return false;
    }
}

// TODO: this needs to be optimised?
int Scheduler::_idToIndex(TaskId id) {
    std::cout << "id: " << id << std::endl;
    for (int i = 0; i < MAX_TASKS; i++) {
        int x = index_id[i].value;
        std::cout << i << ": " << x << std::endl;
        if (index_id[i].value == id) {
            return index_id[i].key;
        }
    }
    return -1;
}

bool Scheduler::AddTask(Task *task, TaskId id, TaskPriority priority) {
    if (WithinArrayBounds(_numberOfTasks, MAX_TASKS)) {
        TaskState state = task->Init(id, priority);
        if (state == TaskState::Waiting) {
            _addToMap(_numberOfTasks, task->GetId());
            _tasks[_numberOfTasks++] = task;
            return true;
        } else {
            // Something went wrong
        }
    }
    // Out of bounds
    return false;
}

TaskState Scheduler::RunTaskById(uint32_t msNow, TaskId id) {
    int index = _idToIndex(id);
    if (index < 0) {
        // Failed to get task
        // TODO: inappropriate use of TaskState...
        return TaskState::Undefined;
    } else {
        _currentRunningTaskId = id;
        return _tasks[index]->Run(msNow);
    }
}

TaskId Scheduler::GetRunningTaskId() const {
    return _currentRunningTaskId;
}

Task* Scheduler::GetTaskById(TaskId id) {
    int index = _idToIndex(id);
    std::cout << "Index: " << index;
    if (index < 0) {
        // Failed to get task
        std::cout << "Failed to get task\n";
        return nullptr;
    }
    return *(_tasks + index);
}

void Scheduler::RemoveTaskById(TaskId id) {
    int index = _idToIndex(id);
    if (index < 0) {
        // Failed to get task
    }
    _tasks[index] = nullptr;
}

void Scheduler::RemoveAllTasks() {
    for (int i = 0; i < MAX_TASKS; i++) {
        _tasks[i] = nullptr;
    }
}

void Scheduler::ClockTick() {
    // Interrupt called every xHz, eg. from hardware timer
}

void Scheduler::RaiseInterrupt() {
    // Other interrupts
}

void Scheduler::AnalyzeTasks() {
    // Task with the highest priority
    for (int i = 0; i < _numberOfTasks; i++) {

    }

    // Some algorithm to meet deadlines
}

void Scheduler::AutoRun() {
    // Entrypoint to 
}

}
