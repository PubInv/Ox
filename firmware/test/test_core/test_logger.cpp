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

#include <unity.h>
#include <stdio.h>
#include <logger.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdlib.h>


const char *alpha[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

void create_random_chars(int n) {
    char *ptr = 0;
    ptr = (char*) malloc(n * sizeof(char));
    
    if (ptr == NULL) {
        printf("Error! memory not allocated.");
        exit(0);
    }

    /*
    srand(time(NULL));
    int s = static_cast<int>(sizeof(alpha) / sizeof(alpha[0])) - 1;
    
    char x[n];
    for (int i = 0; i <= n; i++) {
        int r = rand() & s;
        x[i] = *alpha[r];
    }
    puts(x);*/
}

void test_logger_1() {
    OxLogger::ResetBuffer();
    const char* msg = "Hello";
    OxLogger::Log(msg);
    TEST_ASSERT_EQUAL_INT(strlen(msg), OxLogger::bufferIndex);   
}

void test_logger_2() {
    OxLogger::ResetBuffer();
    puts("Reading buffer");
    const char* msg = "Test buffer memory";
    OxLogger::Log(msg);
    puts(OxLogger::buffer);
    TEST_ASSERT_TRUE(true);
}

void test_logger_3() {
    OxLogger::ResetBuffer();
    //const char* msg = create_random_chars(100);

    puts("Creating random chars");
    create_random_chars(1000);
    //OxLogger::Log(msg);
    //puts(OxLogger::buffer);
    TEST_ASSERT_TRUE(true);
}

void test_logger_4() {
    OxLogger::ResetBufferPtr();
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_logger_1);
    RUN_TEST(test_logger_2);
    RUN_TEST(test_logger_3);
    RUN_TEST(test_logger_4);
    UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    process();
}
void loop() {
    //
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif
