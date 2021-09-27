#ifndef TASK_H
#define TASK_H

#include <inttypes.h>

namespace VOS_Task {

    class Task {
        private:
            uint32_t msLast;

        public:
            Task(){}
            bool run(uint32_t ms);

    };

}

#endif