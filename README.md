# NASA-COG

## Introduction

This is a repo for a control system made for NASA to drive a Ceramic Oxygen Generation
technology they have developed. It was forked from ["The Ox"]() project of Public Invention run by Mr. Ben Coombs, which was in turn a fork of the VentOS project of Helpful Engineering let my Ben Coombs, Dr. Erich Schulz, and Robert L. Read.

However, the main reason we are starting with this fork is not that the Pressure Swing
Adsorption system of the Ox is a particularly relevant, but rather that the code
represents a nice platform of simple microcontroller control of related machines.

To wit, includes PlatformIO configuration, a state machine (finite automata),
a superloop architecutre, and a hardware abstraction layer.

This particular project of NASA will improve life support technology in space,
but may eventually greatly improve global health.

## How to run

At present, this code does little (for the NASA COG project). We intend
to build a native (that is, laptop) simulation using mock hardware components.
The fundamental way to run this is to perform:
> cd firmware

> pio run -e native && ./.pio/build/native/program

We are currently creating the [RibbonFish](https://github.com/PubInv/NASA-COG/blob/develop/RibbonFish.md) proof-of-concept.

This can be run on a Due by executing:

> make pio-run-due_ribbonfish

As this develops, it will use a specific Due hardware configuration,
please see the [documentation](https://github.com/PubInv/NASA-COG/blob/develop/RibbonFish.md) for details to build your own.

## Plans for week of July 11th, 2022

Our initial plan is to get the naitve code to run a "hello, world" version
of the state machine and Harware Abstraction Layer that will eventually grow
into a control system.

## License

In general we intend to follow the [Public Invention Licensed Guidelines](https://github.com/PubInv/PubInv-License-Guidelines) unless
NASA requests something different. If you are starting your own project, please
feel free to fork the license guidelines as a starting point if you like our
license policy.

This program includes free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
