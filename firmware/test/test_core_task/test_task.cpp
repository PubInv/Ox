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

#include <iostream>
#include <unity.h>
#include <core.h>

using namespace OxCore;

class MockTask: public Task {
    private:
        bool _init() override {
            std::cout << "Inited mock task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Ran mock task\n";
            return true;
        }
};

void setUp(void) {
}

void tearDown(void) {
}

void test_can_init_task() {
    MockTask m;
    TaskState taskInited = m.Init(1, 2);
    TEST_ASSERT_EQUAL(taskInited, TaskState::Initialized);
    TEST_ASSERT_EQUAL(m.GetId(), 1);
    TEST_ASSERT_EQUAL(m.GetPriority(), 2);
}

void test_failed_to_init_task() {
    MockTask m;
    TEST_ASSERT_EQUAL(m.GetState(), TaskState::Undefined);
    TEST_ASSERT_EQUAL(m.GetId(), -1);
    TEST_ASSERT_EQUAL(m.GetPriority(), -1);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_can_init_task);
    RUN_TEST(test_failed_to_init_task);
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
