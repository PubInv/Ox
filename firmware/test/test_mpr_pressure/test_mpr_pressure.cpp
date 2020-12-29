#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <inttypes.h>
#include <chrono>
#include <mpr_pressure.h>

using namespace PIOC_Sensor;

void test_sensor_does_read(){
  MPR_Pressure mpr;
  TEST_ASSERT_TRUE(mpr.setup());
  float p = mpr.readPressure();
  Serial.print("Pressure: ");
  Serial.println(p);
  TEST_ASSERT_TRUE(p != NAN);
}


void process() {
  UNITY_BEGIN();
  RUN_TEST(test_sensor_does_read);
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
