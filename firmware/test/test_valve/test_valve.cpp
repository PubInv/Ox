#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <inttypes.h>
#include <chrono>
#include <valve.h>
#include <config.h>

/*void test_valve_does_init() {
  int success = valve_init();
  TEST_ASSERT_TRUE(success == 1);
}*/

void test_valve_does_tick(){
  //using namespace std;
  using namespace std::chrono;

  // Record start time
    
  steady_clock::time_point a = steady_clock::now();
  int success = valve_init();
  TEST_ASSERT_TRUE(success == 1);

  for (int i = 0; i < 3; i++){
    std::cout << "Cycle: " << i << "\n";
    for (uint32_t i = 0; i < TOTAL_CYCLE_TIME; i += TIME_STEP){

      bool success = valve_tick(i);

      // Wait 100ms during the valve sim
      steady_clock::time_point start = steady_clock::now();
      while (1){
        steady_clock::time_point current = steady_clock::now();
        duration<double> elapsed = current - start;

        double x = elapsed.count();
        std::cout << " ";// this needs to be there to show the couts!
        double y = 0.1; // same as TIME_STEP
        if (x > y){
          break;
        }
      }
    // end of waiting
    }

    steady_clock::time_point b = steady_clock::now();
    duration<double> e = b - a;
    std::cout << "\nTotal time: " << e.count() << "\n";
  }
  //TEST_ASSERT_TRUE(success);
}

void process() {
  UNITY_BEGIN();
  //RUN_TEST(test_valve_does_init);
  RUN_TEST(test_valve_does_tick);
  UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    process();
}
void loop() {
    //
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif
