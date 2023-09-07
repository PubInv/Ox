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
#endif
#include <iostream>
#include <unity.h>
#include <core.h>

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

///////////////////////////////////////////////////////////
//                        Scheduler                      //
///////////////////////////////////////////////////////////

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
    MockTask mockTask;
    TaskProperties mockProps = {"mock", 1, TaskPriority::Medium, 100};
    bool success = sch.AddTask(&mockTask, &mockProps);
    TEST_ASSERT_TRUE(success);
}

void test_can_get_task() {
    Scheduler sch;
    MockTask mockTask;
    TaskProperties mockProps = {"mock", 1, TaskPriority::Medium, 100};
    bool success = sch.AddTask(&mockTask, &mockProps);
    TEST_ASSERT_TRUE(success);
    Task* t = sch.GetTaskById(1);
    TEST_ASSERT_EQUAL((void*)t, &mockTask);
}

void test_can_run_task() {
    Scheduler sch;
    MockTask mockTask;
    TaskProperties mockProps = {"mock", 1, TaskPriority::Medium, 100};
    bool success = sch.AddTask(&mockTask, &mockProps);
    TEST_ASSERT_TRUE(success);
    TaskState state = sch.RunTaskById(100, 1);
    TEST_ASSERT_EQUAL(TaskState::Running, state);
}

void test_task_didnt_init() {
    Scheduler sch;
    MockTask m;
    // sch.AddTask(&m, 1, 2);
    // TaskState state1 = sch.RunTask(100, 0);
    // TEST_ASSERT_EQUAL(TaskState::Undefined, state1);
    TEST_ASSERT_TRUE(true);
}

void test_cant_run_task() {
    Scheduler sch;
    MockTaskFails m;
    // sch.AddTask(&m, 1, 2);
    // TaskState state1 = sch.RunTask(100, 0);
    // TEST_ASSERT_EQUAL(TaskState::RunFailed, state1);
    TEST_ASSERT_TRUE(false);
}

void test_can_run_tasks() {
    Scheduler sch;
    MockTask m1;
    MockTask m2;
    // sch.AddTask(&m1, 10, 50);
    // sch.AddTask(&m2, 11, 51);
    // TaskState state1 = sch.RunTask(100, 0);
    // TEST_ASSERT_EQUAL(TaskState::RunSuccess, state1);
    // TaskState state2 = sch.RunTask(100, 1);
    // TEST_ASSERT_EQUAL(TaskState::RunSuccess, state2);
    TEST_ASSERT_TRUE(false);
}

///////////////////////////////////////////////////////////
//                         Task                          //
///////////////////////////////////////////////////////////

// void test_can_init_task() {
//     MockTask mockTask;
//     TaskState taskInited = mockTask._init(1, 2);
//     TEST_ASSERT_EQUAL(taskInited, TaskState::Initialized);
//     TEST_ASSERT_EQUAL(m.GetId(), 1);
//     TEST_ASSERT_EQUAL(m.GetPriority(), 2);
// }

void test_failed_to_init_task() {
    MockTask m;
    TEST_ASSERT_EQUAL(m.GetState(), TaskState::Undefined);
    TEST_ASSERT_EQUAL(m.GetId(), -1);
    TEST_ASSERT_EQUAL(m.GetPriority(), -1);
}


///////////////////////////////////////////////////////////
//                        Logger                         //
///////////////////////////////////////////////////////////

// const char *alpha[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

// void create_random_chars(int n) {
//     char *ptr = 0;
//     ptr = (char*) malloc(n * sizeof(char));
    
//     if (ptr == NULL) {
//         printf("Error! memory not allocated.");
//         exit(0);
//     }

//     /*
//     srand(time(NULL));
//     int s = static_cast<int>(sizeof(alpha) / sizeof(alpha[0])) - 1;
    
//     char x[n];
//     for (int i = 0; i <= n; i++) {
//         int r = rand() & s;
//         x[i] = *alpha[r];
//     }
//     puts(x);*/
// }

// void test_logger_1() {
//     OxLogger::ResetBuffer();
//     const char* msg = "Hello";
//     OxLogger::Log(msg);
//     TEST_ASSERT_EQUAL_INT(strlen(msg), OxLogger::bufferIndex);   
// }

// void test_logger_2() {
//     OxLogger::ResetBuffer();
//     puts("Reading buffer");
//     const char* msg = "Test buffer memory";
//     OxLogger::Log(msg);
//     puts(OxLogger::buffer);
//     TEST_ASSERT_TRUE(true);
// }

// void test_logger_3() {
//     OxLogger::ResetBuffer();
//     //const char* msg = create_random_chars(100);

//     puts("Creating random chars");
//     create_random_chars(1000);
//     //OxLogger::Log(msg);
//     //puts(OxLogger::buffer);
//     TEST_ASSERT_TRUE(true);
// }

// void test_logger_4() {
//     OxLogger::ResetBufferPtr();
// }


////// Run Tests //////

// void run_all_tests() {
//     UNITY_BEGIN();

//     ////// Scheduler //////
//     //RUN_TEST(test_can_add_task);
//     RUN_TEST(test_can_get_task);
//     RUN_TEST(test_can_run_task);
//     RUN_TEST(test_task_didnt_init);
//     RUN_TEST(test_cant_run_task);
//     RUN_TEST(test_can_run_tasks);

//     ////// Timer //////
//     RUN_TEST(test_can_init_task);
//     RUN_TEST(test_failed_to_init_task);

//     ////// Logger //////
//     RUN_TEST(test_logger_1);
//     RUN_TEST(test_logger_2);
//     RUN_TEST(test_logger_3);
//     RUN_TEST(test_logger_4);
    
//     UNITY_END();
// }


// #ifdef ARDUINO
// void setup() {
//     // NOTE!!! Wait for >2 secs
//     // if board doesn't support software reset via Serial.DTR/RTS
//     delay(2000);
//     run_all_tests();
// }
// void loop() {
//     //
// }
// #else
// int main(int argc, char **argv) {
//     run_all_tests();
//     return 0;
// }
// #endif