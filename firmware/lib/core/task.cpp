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

#include "task.h"
#include <iostream>

namespace OxCore
{

    TaskState Task::Init(TaskId id, TaskPriority priority)
    {
        if (static_cast<i32>(_state) < static_cast<i32>(TaskState::Undefined))
        {
            _id = id;
            _priority = priority;
            _state = _init() ? TaskState::Ready : TaskState::Error;
            std::cout << "Initialised\n";
        }
        return _state;
    }

    void Task::Run(Time now)
    {
        if (_state == TaskState::Ready) {
            _state = TaskState::Running;
            _lastRun = now;
            //_state = _run() ? TaskState::RunSuccess : TaskState::RunFailed;
            //std::cout << "Ran\n";
            _run();
        }
        //return _state;
    }

    TaskState Task::Wait(Time now)
    {
        if (_state == TaskState::Running) {
            // TODO: do waiting stuff
            _state = TaskState::Ready;
        }
        return _state;
    }

    i32 Task::GetId() const
    {
        return _id;
    }

    i32 Task::GetPriority() const
    {
        return _priority;
    }

    TaskState Task::GetState() const
    {
        return _state;
    }

    /*bool Task::Callback(char *message)
    {
        std::cout << "Task Callback: " << message << std::endl;
        return true;
    }*/

}
