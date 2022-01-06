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

#ifndef TYPES_H
#define TYPES_H

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif
//#include <cstdint>

namespace OxCore {

// Rust style types
#ifdef FLEXIBLE_WIDTH_TYPES
typedef short i8;
typedef short i16;
typedef long i32;
typedef long long i64;
typedef unsigned short u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;
typedef long double f128;
typedef unsigned char uchar;
typedef signed char schar;
#ifdef ARCH_32
typedef long isize;
typedef unsigned long usize;
#elif ARCH_64
typedef long long isize;
typedef unsigned long long usize;
#endif
#else // FIXED_WIDTH_TYPES
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
//typedef __int128_t i128;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
//typedef __uint128_t u128;
typedef float f32;
typedef double f64;
typedef long double f128;
typedef unsigned char uchar;
typedef signed char schar;
#ifdef ARCH_32
typedef int32_t isize;
typedef uint32_t usize;
#elif ARCH_64
typedef int64_t isize;
typedef uint64_t usize;
#endif
#endif

}

#endif