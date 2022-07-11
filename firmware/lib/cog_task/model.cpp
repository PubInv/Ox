// Copyright (C) 2021 Robert Read.

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


// This is an attempt to build a model of the machien for
// the purpose of mocking it without hardware. This
// initial implementation will be trivial: it will consists
// of an input at room temperature, and an output after the
// primary heater. The temperature at the exhaust will be
// computed from the wattage of the heater (not yet defined),
// and will include some thermal inetial.
#include "cog_task.h"
#include "model.h"

namespace OxApp
{
  void Model::RunForward(float t,CogTask* ct) {
    // This is just a phony thing to show it is alive
    locations[1].temp_C += 1.0;
  }
}
