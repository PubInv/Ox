// Copyright (C) 2022 Robert L. Read

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

#include "abstract_ps.h"

#define DEBUG_LEVEL 0

// void Stack::update(float voltage) {
// #ifdef RIBBONFISH
//   if (DEBUG_LEVEL > 0) {
//     OxCore::Debug<const char *>(name);
//     OxCore::Debug<const char *>(" Voltage: ");
//     OxCore::DebugLn<float>(voltage);
//   }
// #else
//   OxCore::Debug<const char *>("STACK update: (NOT IMPLEMENTED)");
//   OxCore::Debug<const char *>("   ");
//   OxCore::Debug<const char *>(name);
//   OxCore::Debug<const char *>(" pwm_ratio: ");
//   OxCore::DebugLn<float>(_pwm_ratio);
// #endif
// }
