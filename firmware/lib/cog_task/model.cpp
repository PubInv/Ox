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
#include<cmath>

using namespace std;
namespace OxApp
{
  void Model::RunForward(float t,CogTask* ct) {
    Heater heater = ct->_heaters[0];
    float watts = pow(heater._voltage,2) / heater._resistance;
    float degrees_delta = watts / watts_per_degree;
    // now really need to know the airflow

    OxCore::Debug<const char *>("Heater 1 air flow degrees delta ");
    OxCore::DebugLn<float>(degrees_delta);

    locations[1].temp_C += degrees_delta;
  }
}
