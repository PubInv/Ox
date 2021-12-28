#include "core.h"
#include <iostream>
#include <exception>

using namespace OxCore;
Core core;

class MockControlTask: public Task {
    private:
        bool _init() override {
            std::cout << "Inited mock control task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Run mock task control 123\n";
            return true;
        }
};

class MockLogTask: public Task {
    private:
        bool _init() override {
            std::cout << "Inited mock log task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Run mock task log 123\n";
            return true;
        }
};

class MockDisplayTask: public Task {
    private:
        bool _init() override {
            std::cout << "Inited mock display task\n";
            return true;
        }
        bool _run() override {
            std::cout << "Run mock task display 123\n";
            return true;
        }
};

/****** Declare your HAL here *****/



/***** Declare your tasks here *****/

MockControlTask mockControlTask;
MockLogTask mockLogTask;
MockDisplayTask mockDisplayTask;

/***********************************/

int main() {
    if (core.Boot() == false) {
        // TODO: Output error message
        return EXIT_FAILURE;
    }

    /***** Configure and add your tasks here *****/

    TaskProperties logProperties;
    logProperties.id = 20;
    logProperties.period = 100;
    logProperties.priority = 50;
    logProperties.hardTiming = false;
    core.AddTask(&mockLogTask, &logProperties);

    TaskProperties controlProperties;
    controlProperties.id = 10;
    controlProperties.period = 100;
    controlProperties.priority = 10;
    controlProperties.hardTiming = true;
    core.AddTask(&mockControlTask, &controlProperties);
    
    TaskProperties displayProperties;
    displayProperties.id = 30;
    displayProperties.period = 20;
    displayProperties.priority = 100;
    displayProperties.hardTiming = true;
    core.AddTask(&mockDisplayTask, &displayProperties);

    /*********************************************/
    
    // Blocking call
    if (core.Run() == false) {
        // TODO: Output error message
        std::cout << "Failed to run core!" << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/*Task* task = core.scheduler.GetTaskById(10);
    std::cout << "TaskId: " << task->GetId() << std::endl;
    TaskState state = core.scheduler.RunTaskById(0, 10);
    std::cout << "TaskState: " << static_cast<int>(state) << std::endl;*/
