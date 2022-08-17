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

#ifndef STACK_H
#define STACK_H

#include <core.h>
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <math.h>
#endif


// NOTE: AT present, this is really a transistory in front
// of a twelve-volt fan. So we will control it with a PWM signal.
// Different Fans may require different drivers in the future.
// In particular, we hope to use a 4-wire fan with a tachometer.

namespace OxApp {

  class Stack {
  public:
    const char * name;
    uint8_t id;
    uint8_t pin;
    float _voltage = 0; // Between 0.0 and 1.0
  public:
    Stack(){};
    Stack(const char * name, uint8_t id, uint8_t pin, float voltage){
      this->name = name;
      this->id = id;
      this->pin = pin;
      this->_voltage = voltage;
    };
    ~Stack(){};
    void update(float voltage);
  };

}

#endif
