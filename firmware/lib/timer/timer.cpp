#include <chrono>
#include <iostream>
#include <debug.h>
#include <timer.h>

using namespace std::chrono;
using namespace PIOC_Debug;

// Timer References:
//https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
//https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
//https://randomnerdtutorials.com/getting-started-with-esp32/
//https://github.com/espressif/arduino-esp32

namespace PIOC_Timer {

    /*bool TimerC::updateTimer(){
        // VALVES
        tickTime = tElapsed * 1000; //convert double seconds to int milliseconds
        
        // Update valves every timestep
        if (tickTime >= tLast + timeStep){  // BUG: doesnt trigger on t=0!
            vc.tick(tickTime); // This is the valve controller....
            tLast = tickTime;
        }

        // End of one cycle has been reached
        if (tickTime >= totalCycleTime){
            
            std::cout << "End of cycle " << tLast << "\n";
            tStart = startTimer();
            tLast = 0;
            numCycles++;
        }

        tElapsed = checkTimer(false, tStart);
    }*/

    steady_clock::time_point startTimer(){
        return steady_clock::now();
    }

    double checkTimer(bool print, steady_clock::time_point tStart){
        steady_clock::time_point now = steady_clock::now();
        duration<double> t = now - tStart;
        double tElapsed = t.count();

        if (print){
            Debug<const char*>("\nTotal time: ");
            Debug<double>(tElapsed);
            Debug<const char*>("\n");
        }
        return tElapsed;
    }

    void wait(int t){
#ifdef ARDUINO
        delay(t);
#else
        steady_clock::time_point start = steady_clock::now();
        while (1) {
            steady_clock::time_point current = steady_clock::now();
            duration<double> elapsed = current - start;
            double x = elapsed.count();
            double y = t*0.001; // same as TIME_STEP
            if (x > y){
            break;
            }
        }
#endif
    }

}