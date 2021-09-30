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

#include <logger.h>
#include <string.h>
#include <stdio.h>
#
namespace OxLogger {

const int BUFFER_SIZE = 1024;
char buffer[BUFFER_SIZE];
int bufferIndex;
char *bufferPointer;

void Log(const char* message) {    
    if (bufferIndex >= BUFFER_SIZE) {
        bufferIndex = 0;
    }

    for (long unsigned i = 0; i < strlen(message); i++) {
        buffer[bufferIndex++] = message[i];
    }    
}

void ResetBuffer() {
    memset(buffer, '\0', BUFFER_SIZE);
    bufferIndex = 0;
}

/////
void LogPtr(const char* message) {    
    if (bufferIndex >= BUFFER_SIZE) {
        bufferIndex = 0;
    }

    for (long unsigned i = 0; i < strlen(message); i++) {
        buffer[bufferIndex++] = message[i];
    }    
}

void ResetBufferPtr() {
    memset(buffer, '\0', BUFFER_SIZE);
    bufferPointer = &buffer[0];
    //printf("%p\n", bufferPointer);
    bufferPointer++;
    //printf("%p\n", bufferPointer);

    for (int i = 0; i < 500; i++) {
        bufferPointer++;
        printf("%p\n", bufferPointer);
    }
}

}
