#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <inttypes.h>
#include <debug.h>

/*void test_valve_does_init() {
  int success = valve_init();
  TEST_ASSERT_TRUE(success == 1);
}*/

void test_serial(){
  serial_out_s("Testing");
  serial_out_b(false);
  serial_out_b(true);
  uint8_t x = 50;
  serial_out_uint8(x);

  //TEST_ASSERT_TRUE(success);
}

void process() {
  UNITY_BEGIN();
  //RUN_TEST(test_valve_does_init);
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
