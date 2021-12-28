
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

//#include "HAL/posix/hal.h"
#include "scheduler.h"
#include "util.h"
#include <iostream>
#include "error_handler.h"

namespace OxCore {

void Scheduler::setupIdleTask() {
    _idleTask._properties.priority = 0;
    _idleTask._properties.period = 0;
    _idleTask._properties.id = 0;
    _idleTask._properties.hardTiming = false;
}

void Scheduler::sortByPriority() {
    std::cout << "sortByPriority\n";
    std::cout << "Count: " << map.getCount() << std::endl;
    for (int i = 0; i < map.getCount(); ++i) {
        std::cout << "i: " << i << std::endl;
        Task *task = map.getValueByIndex(i);
        TaskPriority priority = task->GetPriority();
        std::cout << "Priority: " << priority << std::endl;
    }
}

Task* Scheduler::getNextTaskToRun(TimeMs currentTime) {
    //std::cout << "\n------ getNextTaskToRun ------\n";
    // Get state of all tasks
    for (int i = 0; i < map.getCount(); ++i) {
        //std::cout << "i: " << i << std::endl;
        Task *task = map.getValueByIndex(i);

        TimeMs lastRunTime = task->GetLastRunTime();
        //std::cout << "currentTime: " << currentTime << std::endl;
        //std::cout << "lastRunTime: " << lastRunTime << std::endl;
        TimeMs deltaRunTime = currentTime - lastRunTime;
        TimeMs period = task->GetPeriod();

        task->_modifiedPriority = task->GetPriority();
        // Too long since the task was run so increase priority.
        //std::cout << "deltaRunTime: " << deltaRunTime << std::endl;
        //std::cout << "period: " << period << std::endl;
        if (deltaRunTime > period) {
            if (task->IsHardTiming()) {
                task->_modifiedPriority = TaskPriorityTimeExceededHard;
                ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::TaskPriorityTimeExceededHard);
            } else {
                task->_modifiedPriority = TaskPriorityTimeExceededSoft;
                ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::TaskPriorityTimeExceededSoft);
            }
        }
    }
    
    /*std::cout << "Listing tasks:\n";
    for (int i = 0; i < map.getCount(); i++) {
        
        Task *task = map.getValueByIndex(i);
        std::cout << "id: " << task->GetId() 
                    << " priority: " << task->_properties.priority 
                    << " modifiedPriority: " << task->_modifiedPriority 
                    << " period: " << task->_properties.period 
                    << std::endl;
    }*/

    int lowestPriority = 99999;
    TaskId lowestId = 0;
    Task* nextTask = nullptr;
    for (int i = 0; i < map.getCount(); i++) {
        Task* task = map.getValueByIndex(i);
        if (task->_modifiedPriority < lowestPriority) {            
            lowestPriority = task->_modifiedPriority;
            lowestId = task->_properties.id;
            nextTask = task;
        }
    }
    std::cout << "Task to run next: " << nextTask->_properties.id << " priority: " << lowestPriority << std::endl;
    return nextTask;
}

bool Scheduler::AddTask(Task *task, TaskProperties *properties) {
    if (WithinArrayBounds(_numberOfTasks, MAX_TASKS)) {
        TaskState state = task->Init(properties);
        if (state == TaskState::Ready) {
            map.add(properties->id, task);
            return true;
        } else {
            std::cout << "Failed to add task!\n";
        }
    }
    // Out of bounds
    return false;
}

bool Scheduler::Init() {
    
    switch (_properties.mode) {
        case SchedulerMode::RoundRobin:
            
            //sortByPriority();

            return true;
        break;
        default:
            return false;
        break;
    }
}

TaskState Scheduler::RunNextTask(u32 msNow) {
    std::cout << "RunNextTask\n";
    Task* task = getNextTaskToRun(msNow);
    task->Run(msNow);
    return task->GetState();
}

TaskState Scheduler::RunTaskById(uint32_t msNow, TaskId id) {
    std::cout << "Running task! id: " << id << std::endl;
    Task* tp = map.getValue(id);
    if (tp == nullptr) {
        return TaskState::Error;
    } else {
        _currentRunningTaskId = id;
        tp->Run(msNow);
        return TaskState::Running;
    }
}

TaskId Scheduler::GetRunningTaskId() const {
    return _currentRunningTaskId;
}

Task* Scheduler::GetTaskById(TaskId id) {
    Task* tp = map.getValue(id);
    return tp;
}

void Scheduler::RemoveTaskById(TaskId id) {    
    //_tasks[index] = nullptr;
}

void Scheduler::SetProperties(SchedulerProperties properties) {
    _properties = properties;
}

SchedulerProperties Scheduler::GetProperties() {
    return _properties;
}



}
