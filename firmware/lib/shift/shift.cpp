/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

#include <shift.h>
//#include <config.h>
#include <inttypes.h>

// Shift register
#define DS 13    // 747HC pin 14 - serial data
#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)
#define SH_CP 27 // 747HC pin 11 - shift register clock

shift_pins sp;

void shiftInit()
{
  sp.latch = ST_CP;
  sp.clock = SH_CP;
  sp.data = DS;

#ifdef ARDUINO
  Serial.print("shift init");
  pinMode(sp.latch, OUTPUT);
  pinMode(sp.clock, OUTPUT);
  pinMode(sp.data, OUTPUT);
#else
  std::cout << "Shift init" << std::endl;
#endif
}

void shiftOutValves(uint8_t data_out)
{
#ifdef ARDUINO
  // take the latchPin low
  digitalWrite(ST_CP, LOW);
  // shift out the bits:
  shiftOut(DS, SH_CP, MSBFIRST, data_out); //, numberToDisplay);
  //take the latch pin high so the LEDs will light up:
  digitalWrite(ST_CP, HIGH);
#endif
}
