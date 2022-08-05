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

#include "fan.h"

#define DEBUG_FAN 0
namespace OxApp {

    void Fan::update(float pwm_ratio) {
        _pwm_ratio = pwm_ratio;
        int pwm_setting = _pwm_ratio * 255;
        analogWrite(this->pin,pwm_setting);
#ifdef RIBBONFISH
        if (DEBUG_FAN) {
          OxCore::Debug<const char *>("Fan update: ");
          OxCore::Debug<int>(id);
          OxCore::Debug<const char *>("   ");
          OxCore::DebugLn<const char *>(name);
          OxCore::DebugLn<const char *>(" Voltage: ");
          // TODO: Replace this with the OUTPUT_RESOLOUTION
          OxCore::Debug<const char *>(" pwm_setting: ");
          OxCore::DebugLn<float>(pwm_setting);
        }
#else
        OxCore::Debug<const char *>("Fan update: (NOT IMPLEMENTED)");
        OxCore::Debug<const char *>("   ");
        OxCore::Debug<const char *>(name);
        OxCore::Debug<const char *>(" pwm_ratio: ");
        OxCore::DebugLn<float>(_pwm_ratio);
#endif
    }
}
