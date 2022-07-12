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


#ifndef MODEL_H
#define MODEL_H
#include "cog_task.h"


// we need to change this!
namespace OxApp {
  // forward declaration
  class CogTask;

  struct Location {
    const char *name;
    float temp_C;
  };

  class Model {
  public:
    float air_flow_slm = 340.0;
    float flow_l_per_s = air_flow_slm/60.0;
    float heat_capacity_air = 1.115; // kJ/(kg * degree K)
    float air_kg_per_l =  0.001293;
    float watts_per_degree = 1000.0 * air_kg_per_l * heat_capacity_air * flow_l_per_s;

    // locations
    Location locations[2];

    // Here we can build in some aspects of the machine
    // which represent physical reality
    Model(){
      locations[0].name = "INPUT";
      // we'll just initialize to room temperature
      locations[0].temp_C = 25.0;
      locations[1].name = "OUTPUT";
      locations[1].temp_C = 25.0;
    }
    // This is the heart of the heat simulation,
    // if the heater has a known wattage, we can move forward
    void RunForward(float t,CogTask* ct);
  };
}




#endif
