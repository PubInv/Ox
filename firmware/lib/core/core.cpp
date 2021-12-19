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

#include "core.h"
#include <iostream>

#include "HAL/linux/linux.h"
/*#ifdef LINUX_HAL
#include "linux.h"
#elif ARM_HAL
#include "arm.h"
#endif*/

namespace OxCore {

bool Core::Configure(Target target) {
    std::cout << "Configure: " << static_cast<int>(target) << std::endl;
    _state = CoreState::Configured;
    // TODO: use HAL
    return true;
}

bool Core::Boot() {
    if (_state == CoreState::Undefined) {
        std::cout << "Please configure before booting!\n";
        return false;
    }
    else if (_state == CoreState::Configured) {
        std::cout << "Boot\n";
        //start();
        return true;
    } else {
        //
        return false;
    }
}

void Core::AddTask(Task *task, TaskId id, TaskPriority priority) {
    bool taskAdded = scheduler.AddTask(task, id, priority);
    if (taskAdded) {
        std::cout << "Task Added!\n";
    } else {
        std::cout << "Failed to add task!\n";
    }
}

void Core::Run() {
    for (;;) {
        //scheduler.RunNextTask(10);
        Task* task = scheduler.GetTaskById(10);
        std::cout << "TaskId: " << task->GetId() << std::endl;
        TaskState state = scheduler.RunTaskById(0, 10);
        std::cout << "TaskState: " << static_cast<int>(state) << std::endl;
    }
}
    
// Private //

void Core::ClockTick() {

}

void Core::WriteRegister() {

}

void Core::ReadRegister(u32 address) {

}

void Core::AllocateMemory(u32 address) {

}

void Core::HandleInterupt() {
    
}




}
