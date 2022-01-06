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

#ifndef CORE_H
#define CORE_H

// Header files must be declared here to be used by files including <core.h>
#include "scheduler.h"
//#include "logger.h"
#include "error_handler.h"
#include "timer.h"
#include "types.h"
#include "debug.h"

namespace OxCore {

enum class CoreState {
    Undefined,
    Configured,
    Starting,
    Running,
    Error,
    Terminating
};

class Core {
    public:
        bool Configure(void *config);
        bool Boot();
        void AddTask(Task *task, TaskProperties *properties);
        bool Run();

        
        
        Core(): _state(CoreState::Undefined) {};
        ~Core() = default;
        // Cannot copy class
        Core(const Core&) = delete;
        Core& operator=(const Core) = delete;
        // Cannot move class
        Core(Task&&) = delete;
        Core& operator=(Core&&) = delete;
    private:
        u32 _i;
        Timer _timer;
        Timer _watchdogTimer;
        CoreState _state;
        Scheduler _scheduler;
        void Tick();
        void WriteRegister(u32 address);
        void ReadRegister(u32 address);
        void AllocateMemory(u32 address);
        void HandleInterupt();
        void CreateWatchdog(u32 timeoutMs);
        bool ResetWatchdog();
        
};

}

#endif
