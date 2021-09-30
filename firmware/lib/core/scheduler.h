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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <task.h>
#include <util.h>
#include <cstdint>

namespace OxCore {

// [ Task1, Task2, Task3, Task4, Empty ]
//            ^             ^      ^
//         current        number  max

const int MAX_TASKS = 5;
Task *tasks[MAX_TASKS];
int currentRunningTask = 0;
int numberOfTasks = 0;

bool AddNextTask(Task *task);
bool AddTask(Task *task, int index);
void RunNextTask(Task task, uint32_t msNow);
void IncrementRunningTask();

}

#endif
