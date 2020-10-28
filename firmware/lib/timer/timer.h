#ifndef TIMER_H
#define TIMER_H

#include <chrono>
using namespace std::chrono;

namespace PIOC_Timer {

    struct Timer { 
        uint32_t tickTime = 0;
        uint32_t numCycles = 0;
        uint32_t tLast = 0;
        double tElapsed = 0;
        steady_clock::time_point tStart;
    };
    
    steady_clock::time_point startTimer();
    double checkTimer(bool print, steady_clock::time_point tStart);
    void wait(int t);

}

#endif



 /*class TimerC {
        private:
            uint32_t tickTime;
            uint32_t numCycles;
            uint32_t tLast;
            double tElapsed;
            steady_clock::time_point tStart;
            uint8_t timeStep;
            uint32_t totalCycleTime;
        public:
            bool updateTimer();
    }*/