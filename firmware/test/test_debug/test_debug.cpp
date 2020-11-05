#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <inttypes.h>
#include <debug.h>

using namespace PIOC_Debug;

void test_serial(){
  Debug<const char*>("Test text debug\n");

  TEST_ASSERT_TRUE(true);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_serial);
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
