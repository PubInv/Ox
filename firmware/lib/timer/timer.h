#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace PIOC_Timer {

using namespace std::chrono;

    uint64_t timeSinceEpochMs();

    class Timer {
        private:
            uint32_t msElapsed;
            uint32_t msStart;
        public:
            Timer(){
                msElapsed = 0;
                msStart = 0;
            }
            Timer(uint32_t start){
                msElapsed = 0;
                msStart = start;
            }
            void update();
            uint32_t elapsed();
    };

}

#endif