#ifndef SENSOR_READ_TASK_H
#define SENSOR_READ_TASK_H

#include <task.h>

namespace OxApp {

class SensorReadTask : public OxCore::Task
    {
    private:
        bool _init() override;
        bool _run() override;        
    };

}

#endif