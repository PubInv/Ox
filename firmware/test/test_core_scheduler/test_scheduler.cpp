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
#include "test_common.h"

using namespace OxCore;

class MockTask: public Task {
    private:
        bool _init() override {
            //std::cout << "Inited mock task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Run mock task\n";
            return true;
        }
};

class MockTaskFails: public Task {
    private:
        bool _init() override {
            //std::cout << "Inited mock task\n";
            return true;
        }
        bool _run() override {
            return false;
        }
};


void setUp(void) {
}

void tearDown(void) {
}

/*void test_can_add_task() {
    Scheduler sch;
    MockTask m;
    TaskState taskInited = m.Init(1, 2);
    TEST_ASSERT_EQUAL(TaskState::Waiting, taskInited);
    bool success = sch.AddTask(&m, 0);
    TEST_ASSERT_TRUE(success);
}*/

void test_can_create_task() {
    Scheduler sch;
    MockTask m;
    bool success = sch.AddTask(&m, 1, 2);
    TEST_ASSERT_TRUE(success);
}

void test_can_get_task() {
    Scheduler sch;
    MockTask m;
    bool success = sch.AddTask(&m, 1, 2);
    TEST_ASSERT_TRUE(success);
    Task* t = sch.GetTask(0);
    TEST_ASSERT_EQUAL((void*)t, &m);
}

void test_can_run_task() {
    Scheduler sch;
    MockTask m;
    int id = 1;
    int index = 0;
    bool success = sch.AddTask(&m, 1, 2);
    TEST_ASSERT_TRUE(success);
    TaskState state = sch.RunTask(100, index);
    TEST_ASSERT_EQUAL(TaskState::RunSuccess, state);
}

void test_task_didnt_init() {
    Scheduler sch;
    MockTask m;
    sch.AddTask(&m, 1, 2);
    TaskState state1 = sch.RunTask(100, 0);
    TEST_ASSERT_EQUAL(TaskState::Undefined, state1);
}

void test_cant_run_task() {
    Scheduler sch;
    MockTaskFails m;
    sch.AddTask(&m, 1, 2);
    TaskState state1 = sch.RunTask(100, 0);
    TEST_ASSERT_EQUAL(TaskState::RunFailed, state1);
}

void test_can_run_tasks() {
    Scheduler sch;
    MockTask m1;
    MockTask m2;
    sch.AddTask(&m1, 10, 50);
    sch.AddTask(&m2, 11, 51);
    TaskState state1 = sch.RunTask(100, 0);
    TEST_ASSERT_EQUAL(TaskState::RunSuccess, state1);
    TaskState state2 = sch.RunTask(100, 1);
    TEST_ASSERT_EQUAL(TaskState::RunSuccess, state2);
}

void process() {
    UNITY_BEGIN();
    //RUN_TEST(test_can_add_task);
    RUN_TEST(test_can_get_task);
    RUN_TEST(test_can_run_task);
    
    RUN_TEST(test_task_didnt_init);
    RUN_TEST(test_cant_run_task);
    RUN_TEST(test_can_run_tasks);
    
    UNITY_END();
}

TEST_MAIN

#endif
