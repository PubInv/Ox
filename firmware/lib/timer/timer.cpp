#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <chrono>
//#include <iostream>
#include <timer.h>

namespace PIOC_Timer {

    uint64_t timeSinceEpochMs() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    void Timer::update(){
        #ifdef ARDUINO
        this->msElapsed = (uint32_t)millis() - this->msStart;
        #else
        this->msElapsed = (uint32_t)(timeSinceEpochMs()) - this->msStart;
        #endif
    }

    uint32_t Timer::elapsed(){
        return msElapsed;
    }
}