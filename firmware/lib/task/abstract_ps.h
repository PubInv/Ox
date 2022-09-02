// Copyright (C) 2022 Robert Read

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

#ifndef ABSTRACT_PS_H
#define ABSTRACT_PS_H

#include <core.h>
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <math.h>
#endif


class AbstractPS {
public:
  const char * name;
  uint8_t id;
  float _voltage = 0;
  float _amperage = 0;
public:
  AbstractPS(){};
  AbstractPS(const char * name, uint8_t id){
    this->name = name;
    this->id = id;
  };
  virtual int init() { return -1; };
  virtual ~AbstractPS(){};
  virtual void updateVoltage(float voltage);
  virtual void updateAmperage(float amperage);
};


#endif
