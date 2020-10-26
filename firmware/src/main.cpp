#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif
//#include <Wire.h>
//#include "SPI.h"
//#include "gfx_extensions.h"
//#include "Adafruit_ILI9341.h"
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
//#include <display.h>
#include <debug.h>
//#include <shift.h>
#include <inttypes.h>
#include <state.h>
#include <valve.h>
#include <config.h>
#include <chrono>

////////// TIMER //////////

using namespace std::chrono;

struct timer { 
  uint32_t tick_time;
  uint32_t num_cycles;
  uint32_t t_last;
  double t_elapsed;
  steady_clock::time_point t_start;
};

timer valve_timer;

steady_clock::time_point start_timer(){
  return steady_clock::now();
}

double check_timer(bool print, steady_clock::time_point t_start){
  steady_clock::time_point now = steady_clock::now();
  duration<double> t = now - t_start;
  double t_elapsed = t.count();
#ifdef ARDUINO
  if (print){
    Serial.print("\nTotal time: ");
    Serial.print(t_elapsed);
    Serial.print("\n");
  }
#else
  if (print)
    std::cout << "\nTotal time: " << t_elapsed << "\n";
#endif
  return t_elapsed;
}

void wait(int t){
#ifdef ARDUINO
  delay(t);
#else
  steady_clock::time_point start = steady_clock::now();
  while (1){
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

////////// END OF TIMER //////////

void setup() {
  serial_init(115200);
  serial_out_s("Starting PIOC\n");

  pioc_state ps = { .mode = STARTING,
    .run_time = 0
  };

  valve_init();
  valve_timer.tick_time = 0;
  valve_timer.num_cycles = 0;
  valve_timer.t_last = 0;
  valve_timer.t_elapsed = 0;
  valve_timer.t_start = start_timer();
  
  /*pinMode(ST_CP, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(DS, OUTPUT);
 */

 //display_init();
 //valve_init();

}



void loop(void) {
  //testText();
  //Serial.println(F("Loop"));
  //testFillScreen();
  //delay(3000);

  // VALVES
  valve_timer.tick_time = valve_timer.t_elapsed * 1000; //convert double seconds to int milliseconds
  
  // Update valves every timestep
  if (valve_timer.tick_time >= valve_timer.t_last + TIME_STEP){  // BUG: doesnt trigger on t=0!
    valve_tick(valve_timer.tick_time);
    valve_timer.t_last = valve_timer.tick_time;
  }

  // End of one cycle has been reached
  if (valve_timer.tick_time >= TOTAL_CYCLE_TIME){
    valve_timer.t_start = start_timer();
    valve_timer.t_last = 0;
    valve_timer.num_cycles++;
    std::cout << "end of cycle \n";
  }

  valve_timer.t_elapsed = check_timer(false, valve_timer.t_start);

  if (valve_timer.num_cycles > 5)
    exit(0);
}


#ifndef ARDUINO
int main(int argc, char **argv) {
  setup();
  while(1) loop();
}
#endif
