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
#include "HAL/posix/hal.h"

namespace OxCore {

#define TICK_PERIOD 100
#define WATCHDOG_TIMEOUT_MS 250

bool Core::Boot() {
    bool result = false;
    
    ErrorHandler::SetErrorMode(OxCore::ErrorMode::StdOut);
    // TODO: configure/validate HAL

    SchedulerProperties properties;
    properties.mode = SchedulerMode::Dynamic;
    properties.tickPeriodMs = TICK_PERIOD;
    _scheduler.SetProperties(properties);

    CreateWatchdog(WATCHDOG_TIMEOUT_MS);

    _state = CoreState::Configured;

    if (_state == CoreState::Configured) {
        std::cout << "Boot\n";
        //start();
        result = true;
    } else {
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    }
    return result;
}

void Core::AddTask(Task *task, TaskProperties *properties) {
    bool taskAdded = _scheduler.AddTask(task, properties);
    if (taskAdded) {
        std::cout << "Task Added!\n";
    } else {
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToAddTask);
    }
}

// 1. Set properties
// 2. Add tasks
// 3. Setup scheduler
// 4. Run scheduler

bool Core::Run() {
    std::cout << "Core::Run!" << std::endl;
    bool success = _scheduler.Init();
    if (success == false) {
        return false;
    }

    _timer.Init();

    int i = 0;
    while (true) {
        std::cout << "-------------------------\n";
        u32 elapsed = _timer.Update();
        TaskState state = _scheduler.RunNextTask(elapsed);
        std::cout << "State: " << static_cast<int>(state) << std::endl;

        for (int i = 0; i < 10000000; i++) {
            // waste time
        }

        // For testing:
        i++;
        if (i > 3) {
            return true;
        }

        bool reset = ResetWatchdog();
        if (reset == false) {
            return false;
        }

        // intentional shutdown
        bool shutdown = false;
        if (shutdown) {
            return true;
        }
    }
    return false;
}
    
// Private //

void Core::ClockTick() {

}

void Core::WriteRegister(u32 address) {

}

void Core::ReadRegister(u32 address) {

}

void Core::AllocateMemory(u32 address) {

}

void Core::HandleInterupt() {
    
}

void Core::CreateWatchdog(u32 timeoutMs) {
    std::cout << "Create watchdog (todo)\n";
    _watchdogTimer.Init();
}

bool Core::ResetWatchdog() {
    std::cout << "Reset watchdog (todo)\n";
    u32 elapsed = _watchdogTimer.Update();
    if (elapsed > WATCHDOG_TIMEOUT_MS) {
        std::cout << "elapsed: " << elapsed << std::endl;
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::WatchdogExceeeded);
        return false;
    }
    return true;
}




}
