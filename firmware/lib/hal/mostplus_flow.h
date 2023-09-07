// Copyright (C) 2021
// Robert Read.

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


// This is for a MostPlus M3037 Mass Air Flow sensor.
// We don't have good specs on this. It is a simple 12V
// hot wire sensor, that gives a voltage out. The voltage varies
// goes up as the flow increases.
// At the moment we are using this only as a safety features
// to make sure that air is flowing; we have no way to calibrate it.
// Furtheremore, to read the value with a Due we need a voltage divider
// circuit. I created a divider with a 22Kohm and a 47kohm resistor,
// which puts the zero-flow voltage at about 1.8 volts. Sadly, our
// fan only raises this a few tenths of a volt.
// We probably should get a different hot-wire sensor, and
// also the physical housing of this sensor is much larger than
// our main air flow, which creates a problem in several ways.
// But, this is a proof-of-concept. Robert L. Read

#ifndef MOSTPLUS_FLOW
#define MOSTPLUS_FLOW

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#endif

#include <model.h>

class MostPlusFlow {
public:
  MostPlusFlow();
  boolean isAirFlowing();

  virtual ~MostPlusFlow() {};
};


#endif
