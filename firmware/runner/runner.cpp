#include <unity.h>

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_serial);
    RUN_TEST(test_valve_does_init);
    RUN_TEST(test_valve_does_tick);
    RUN_TEST(test_valve_does_tick);
    RUN_TEST(test_timer);
    RUN_TEST(test_sensor_does_read);
    RUN_TEST(test_setup_networking);
    RUN_TEST(test_init);
    RUN_TEST(test_timer);
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