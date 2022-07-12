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

#include "heater.h"

namespace OxApp {

    void Heater::update(float voltage) {
        _voltage = voltage;

        OxCore::Debug<const char *>("Heater update: ");
        OxCore::Debug<int>(id);
        OxCore::Debug<const char *>("   ");
        OxCore::Debug<const char *>(name);
        OxCore::Debug<const char *>(" Voltage: ");
        OxCore::DebugLn<float>(_voltage);

    }

}
