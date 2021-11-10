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

bool Scheduler::AddNextTask(Task *task) {
    if (CheckArrayBounds(_numberOfTasks, MAX_TASKS)) {
        _tasks[_numberOfTasks++] = task;
        return true;
    }
    return false;
}

bool Scheduler::AddTask(Task *task, int index) {
    if (CheckArrayBounds(index, MAX_TASKS)) {
        _tasks[index] = task;
        return true;
    }
    return false;
}

TaskState Scheduler::RunNextTask(uint32_t msNow) {
    IncrementRunningTask();
    return _tasks[_currentRunningTask]->Run(msNow);
}
    
TaskState Scheduler::RunTask(uint32_t msNow, int index) {
    // TODO: bounds checking
    _currentRunningTask = index;
    return _tasks[_currentRunningTask]->Run(msNow);
}

void Scheduler::IncrementRunningTask() {
    _currentRunningTask++;
    if (_currentRunningTask > _numberOfTasks) {
        _currentRunningTask = 0;
    }
}

void Scheduler::StartSchedulerClock() {
    
}

int Scheduler::GetRunningTask() {
    return _currentRunningTask;
}

Task* Scheduler::GetTask(int index) {
    //return &_tasks[index];
    return *(_tasks + index);
}

void Scheduler::RemoveTask(int index) {
    _tasks[index] = nullptr;
}

void Scheduler::RemoveAllTasks() {
    for (int i = 0; i < MAX_TASKS; i++) {
        _tasks[i] = nullptr;
    }
}

}
