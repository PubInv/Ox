#include "sensor_read_task.h"
#ifndef ARDUINO
#include <iostream>
#endif
#include <debug.h>

namespace Ox {
namespace App {
    bool SensorReadTask::_init()
    {
        //std::cout << "Init Sensor Read task\n";
        
        return true;
    }

    bool SensorReadTask::_run() 
    {
        //OxCore::Debug<const char *>("SensorReadTask run\n");
        return true;
    }
}
}