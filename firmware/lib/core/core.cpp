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
#include "debug.h"
#ifdef ARDUINO

#else
#include <iostream>
#include "HAL/posix/hal.h"
#endif

namespace OxCore {

#define SW_TICK 1
#define TICK_PERIOD 2
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
        #ifndef ARDUINO
        std::cout << "Boot\n";
        #endif
        //start();
        result = true;
    } else {
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToBoot);
    }

    bool success = _scheduler.Init();
    if (success == false) {
        return false;
    }
    _timer.Init();

    return result;
}

void Core::AddTask(Task *task, TaskProperties *properties) {
    bool taskAdded = _scheduler.AddTask(task, properties);
    if (taskAdded) {
        #ifndef ARDUINO
        std::cout << "Task Added!\n";
        #endif
    } else {
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::CoreFailedToAddTask);
    }
}

// 1. Set properties
// 2. Add tasks
// 3. Setup scheduler
// 4. Run scheduler

#define DEV 1
#define DEV_LOOPS 30

bool Core::Run() {
    OxCore::Debug<const char *>("Core::Run!\n");

#ifdef DEV
    _i = 0;
#endif

#ifdef SW_TICK
    while (true) {
        u32 elapsed = _timer.Update();
#endif
        Tick();
        bool reset = ResetWatchdog();
        if (reset == false) {
            return false;
        }
#ifdef SW_TICK
        // Wait to simulate tick interrupt
        bool wait_timer = true;
        while (wait_timer) {
            u32 elapsed_wait = _timer.Update() - elapsed;
            if (elapsed_wait >= _scheduler.GetTickPeriod()) {
                wait_timer = false;
            }
        }
        // For testing:
        // _i++;
        // if (_i > DEV_LOOPS) {
        //     return true;
        // }
    }
#endif
    OxCore::Debug<const char *>("Exiting Core::Run!\n");
    
    return false;
}
    
// Private //

// Called by interrupt in embedded
// Or by timer in simulation
void Core::Tick() {
    // Debug<const char*>("-----------------------------------------\n");
    u32 elapsed = _timer.Update();
    TaskState state = _scheduler.RunNextTask(elapsed);
    #ifndef ARDUINO
    std::cout << "State: " << static_cast<int>(state) << std::endl;
    #endif
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
    Debug<const char*>("Create watchdog (todo)\n");
    _watchdogTimer.Init();
}

bool Core::ResetWatchdog() {
    //::cout << "Reset watchdog (todo)\n";
    u32 elapsed = _watchdogTimer.Update();
    if (elapsed > WATCHDOG_TIMEOUT_MS) {
        #ifndef ARDUINO
        std::cout << "Watchdog timed out! Elapsed: " << elapsed << std::endl;
        #endif
        ErrorHandler::Log(ErrorLevel::Critical, ErrorCode::WatchdogExceeeded);
        return false;
    } else {
        #ifndef ARDUINO
        std::cout << "Watchdog reset. Elapsed: " << elapsed << std::endl;
        #endif
        _watchdogTimer.Reset();
        return true;
    }
}




}
