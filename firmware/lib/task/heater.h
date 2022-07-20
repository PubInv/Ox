// Copyright (C) 2021 Robert Read, Ben Coombs.

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

#ifndef HEATER_H
#define HEATER_H

#include <core.h>
#ifdef ARDUINO
#include <Arduino.h>
#else
//#include <cstdint>
#include <math.h>
#endif

namespace OxApp {

  class Heater {
  public:
    const char * name;
    uint8_t id;
    uint8_t pin;
    // A joule heater has a fixed resistance (in Ohms)
    // (this actually varies with temperature but we will ignore that.)
    float _resistance = 2.0;
    // The instantaneous state of an ideal Joule heater is described
    // by only voltage and resistance. Wattage = V^2/R.
    float _voltage = 5.0;
    bool isOn;
  public:
    Heater(){};
    Heater(const char * name, uint8_t id, uint8_t pin, float voltage, float resistance){
      this->name = name;
      this->id = id;
      this->pin = pin;
      this->_voltage = voltage;
      this->_resistance = resistance;
      this->isOn = false;
    };
    ~Heater(){};
    void update(float voltage);
    float compute_change_in_voltage(float current_C,float current_V,float desired_C, float watts_per_degree);
  };

}

#endif
