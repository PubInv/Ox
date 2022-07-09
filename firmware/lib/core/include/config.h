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

#ifndef CONFIG_H
#define CONFIG_H

//#include <cstdint>

//#include <valve.h>

// ADAFRUIT FEATHER ESP32
// RST
// 3V
// NC
// GND
// 26 DAC2 A0   BATT
// 25 DAC1 A1   EN
// 34 A2     USB
// 39 A3     13 A12
// 36 A4     12 A11
// 4 A5      27 A10
// 5 SCK     33 A9
// 18 MO     15 A8
// 19 MI     32 A7
// 16 RX     14 A6
// 17 TX     SCL 22
// 21 21     SDA 23


// Shift register (currently defined in shift.h)
//#define DS 13 // 747HC pin 14 - serial data
//#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)/
//#define SH_CP 27 // 747HC pin 11 - shift register clock


///////// VALVES //////////

// #define TOTAL_CYCLE_TIME 8000 //ms
// #define TIME_STEP 10 //ms
// #define NUM_VALVES 4 //1-8

// { name, num, state, pin, start, stop }
/*Ox_Controller::Valve valveArray[NUM_VALVES] = {
  { 'A', 0, 0, 1, 0, 6000, },
  { 'B', 1, 0, 2, 6000, 12000, },
  { 'C', 2, 0, 4, 5600, 6000, },
  { 'D', 3, 0, 8, 11600, 12000, }};
*/

/*Valve valveArray[NUM_VALVES] = {
  { 'A', 0, 0, 1, 100, 4000, },
  { 'B', 1, 0, 2, 4000, 8000, },
  { 'C', 2, 0, 4, 3700, 4000, },
  { 'D', 3, 0, 8, 7700, 8000, }};
*/

#endif


// Long format of defining valves
/*Ox_Controller::Valve valveArray[NUM_VALVES] = {
  { .name = 'A',
    .num = 0,
    .state = 0,
    .pin = 1,
    .start = 500,
    .stop = 2000, },
  { .name = 'B',
      .num = 1,
      .state = 0,
      .pin = 2,
      .start = 2000,
      .stop = 2400, },
  { .name = 'C',
      .num = 2,
      .state = 0,
      .pin = 4,
      .start = 2400,
      .stop = 4400, },
  { .name = 'D',
      .num = 3,
      .state = 0,
      .pin = 8,
      .start = 4400,
      .stop = 4900, }};*/
