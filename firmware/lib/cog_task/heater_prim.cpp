// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include "heater_prim.h"

namespace OxApp {

    void Valve::update(const uint32_t &msNow) {

        OxCore::Debug<const char *>("Valve update: ");
        OxCore::Debug<int>(state.id);
        OxCore::Debug<const char *>("   ");
        OxCore::Debug<const char *>(state.name);
        // OxCore::Debug<const char *>("   ");
        // OxCore::Debug<int>(msNow);
        // OxCore::Debug<const char *>("   ");
        // OxCore::Debug<int>(state.onTime);
        // OxCore::Debug<const char *>("   ");
        // OxCore::Debug<int>(state.offTime);

        if (msNow >= state.onTime && msNow < state.offTime) {
            OxCore::DebugLn<const char *>("  On");
            state.isOn = true;
        } else {
            state.isOn = false;
            OxCore::DebugLn<const char *>("  Off");
        }
    }

    bool Valve::changeTiming(uint32_t onTime, uint32_t offTime){
        // TODO: error checking
        if (onTime < 0 || offTime < 0) {
            // Call error handler
        }

        state.onTime = onTime;
        state.offTime = offTime;
        return true;
    }

    bool Valve::forceValveTrigger() {
        return false;
    }
}
