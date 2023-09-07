
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
#include "debug.h"
#include <error_handler.h>

namespace OxCore {


void Scheduler::setupIdleTask() {
    _idleTask._properties.priority = static_cast<TaskPriority>(TaskPriorityOS::Idle);
    _idleTask._properties.period = 1;
    _idleTask._properties.id = 0;
}

Task* Scheduler::getNextTaskToRun(TimeMs currentTime) {
    // Record how long the previous task took to run
  if (DEBUG_SCHEDULER > 1) {
    Serial.println("getNextTask");
  }

    if (_lastTaskRan != nullptr) {
        _lastTaskRan->_lastRunDuration = currentTime - _lastTaskRan->_lastRun;
    }

    Task* nextTask = nullptr;
    TimeMs maxTimeUntilDeadline = -99999;
    for (int i = 0; i < map.getCount(); i++) {
        Task *task = map.getValueByIndex(i);

        TimeMs lastRunTime = task->GetLastRunTime();
        TimeMs period = task->GetPeriod();

        // The next task to run is the most positive one
        // < 0 time remaining
        // = 0 due now
        // > 0 time overrun
        task->_timeUntilDeadline = currentTime - (lastRunTime + period);

        if (DEBUG_SCHEDULER > 1) {
          Serial.println(task->_timeUntilDeadline);
        }


        if (task->_timeUntilDeadline > maxTimeUntilDeadline) {
            maxTimeUntilDeadline = task->_timeUntilDeadline;
            nextTask = task;
        }

        if ((-1 * nextTask->_timeUntilDeadline) > nextTask->_lastRunDuration) {
            nextTask = nullptr;
        }
    }
  if (DEBUG_SCHEDULER > 1) {
    if (nextTask == nullptr) {
    } else {
      Serial.println("nexTask");
      Serial.println(nextTask->_properties.name);
    }
  }

    if (nextTask == nullptr) {
        nextTask = &_idleTask;
    }
    return nextTask;
}

bool Scheduler::AddTask(Task *task, TaskProperties *properties) {
    if (WithinArrayBounds(_numberOfTasks, MAX_TASKS)) {
        TaskState state = task->Init(properties);
        if (state == TaskState::Ready) {
            map.add(properties->id, task);
            _numberOfTasks++;
            return true;
        } else {
            ErrorHandler::Log(ErrorLevel::Error, ErrorCode::CoreFailedToAddTask);
        }
    }
    // Out of bounds

    return false;
}

bool Scheduler::Init() {
    switch (_properties.mode) {
        case SchedulerMode::RoundRobin:
            //
            return true;
        break;
        case SchedulerMode::RealTime:
            //
            return true;
        break;
        default:
            return false;
        break;
    }
}

TaskState Scheduler::RunNextTask(uint32_t msNow) {
    Task* nextTask = nullptr;

    switch (_properties.mode) {
        case SchedulerMode::RoundRobin:
            ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::NotImplemented);
        break;
        case SchedulerMode::RealTime:
            nextTask = getNextTaskToRun(msNow);
        break;
        default:
            ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::NotImplemented);
        break;
    }

  if (DEBUG_SCHEDULER > 1) {
    Serial.println("About to Run task!");
    Serial.println(nextTask->_properties.name);
  }
    nextTask->Run(msNow);
  if (DEBUG_SCHEDULER > 1) {
    Serial.println("Finished Run!");
  }
    _lastTaskRan = nextTask;
    return nextTask->GetState();
}

TaskState Scheduler::RunTaskById(uint32_t msNow, TaskId id) {
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

void Scheduler::SetProperties(SchedulerProperties properties) {
    _properties = properties;
}

SchedulerProperties Scheduler::GetProperties() {
    return _properties;
}

uint32_t Scheduler::GetTickPeriod() {
    return _properties.tickPeriodMs;
}

}
