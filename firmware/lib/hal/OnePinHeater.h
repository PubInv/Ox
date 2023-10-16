// Copyright (C) 2021
// Robert Read.

// OnePinHeater.h --- a class to represent an AC Heater controllable by a single pin.

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


// This is an interface for a Solid-state Relay which is controllable by single
// signal similar to the GPIO pin.
// This is our attempt to control 110VAC, in particular to control a heater.
// This is a very easy board to use; it has two channels and lets us
// control them directly from a GPIO pin

#ifndef ONEPINHEATER
#define ONEPINHEATER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <model.h>


class OnePinHeater {
public:
  OnePinHeater();
  int heater_pin;
  void init();

  bool setHeater(int heaterNum, bool onIfTrue);
  virtual ~OnePinHeater() {};
};


#endif
