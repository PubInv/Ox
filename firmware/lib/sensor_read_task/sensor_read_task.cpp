#include "sensor_read_task.h"
#ifndef ARDUINO
#include <iostream>
#endif
#include <debug.h>

namespace OxApp {
    bool SensorReadTask::_init()
    {
        //std::cout << "Init Sensor Read task\n";
        OxCore::Debug<const char *>("SensorReadTask init\n");
        return true;
    }

    bool SensorReadTask::_run() 
    {
        OxCore::Debug<const char *>("SensorReadTask run\n");
        return true;
    }
}
