/*
test_SanyoAceB97 -- A program to test the SanyoAceB97 blower

Copyright 2023, Robert L. Read

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <unity.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstdint>
#include <debug.h>

using namespace OxCore;

/*
  We have already done live testing of our DC power supply
  at high resistances. But we want a way to "smoke test"
  our power supply and wiring at high amperage. We have
  made a custom resistor of low ohmage (maybe about 0.3 or 0.4 ohms)
  which can survive high temperature because it is made or
  nichrome wire. Our goal here is remarkably simple, though
  it doesn't really corresponde to a typical "unit" test.
  Our basic goal is to read from the serial port a simple
  number and apply that many volts. Then we can walk the
  voltage up by hand, reporting the amperage, and checking
  that we can acheive high amperage without anything going
  wrong in our wiring (like getting too hot.)
 */

#include "core_defines.h"
#include <machine.h>
#include <SanyoAceB97.h>

const unsigned long REPORT_PERIOD_MS = 3000;

MachineConfig *machineConfig;
TaskProperties _properties;


void report(MachineConfig *machineConfig) {
  Serial.print("F : ");
  Serial.println(machineConfig->report->fan_speed);
}
unsigned long time_of_last_report = 0;
void test_fan_speed(){

  Serial.println("Started test_fan_speed()!");
  Serial.println("Enter numbers between 0 and 100!");
  delay(100);

  SanyoAceB97 *fan = new SanyoAceB97("FIRST_FAN",0,RF_FAN,1.0);
  fan->_init();
  fan->DEBUG_FAN = 0;

  pinMode(fan->PWM_PIN[0], OUTPUT);
  // It is safer to be completely off until we are ready to start!
  analogWrite(fan->PWM_PIN[0],0);

  char myBuff[64];
  int len = sizeof(myBuff)-1;
  float speed = 0.0;

  while(true) {
    delay(3000);
    if(Serial.available( ) > 0)
    {
        size_t read = Serial.readBytesUntil('\n', myBuff, len);
        myBuff[read] = '\0';
        Serial.println(myBuff);
        speed = atoi(myBuff);
        Serial.print("read speeed: ");
        Serial.println(speed);
         fan->fanSpeedPerCentage(speed);

        // Possibly I should push the config into the HAL
        // so that this kind of reporting can be automatic,
        // but that makes teh HAL less clean
        machineConfig->report->fan_speed = speed;

    } else {
      Serial.println("No input.");

    }
    unsigned long now_ms = millis();
    if ((now_ms - time_of_last_report) > REPORT_PERIOD_MS) {
      report(machineConfig);
      time_of_last_report = now_ms;
      fan->printRPMS();
    }
  };

  Serial.println("terminating test!");
  delay(100);
  TEST_ASSERT_TRUE(true);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_fan_speed);
  UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>

void setup() {
  OxCore::serialBegin(115200UL);
  delay(1000); // delay to make sure it's ready

  machineConfig = new MachineConfig();
  machineConfig->hal = new MachineHAL();

  // bool initSuccess  = machineConfig->hal->init();
  bool initSuccess = true;
  if (initSuccess) {
    OxCore::TaskProperties cogProperties;
    cogProperties.name = "cog";
    cogProperties.id = 20;
    cogProperties.state_and_config = (void *) &machineConfig;
    delay(1000);
    Serial.println("About to run test!");
    delay(100);
    process();
  } else {
    Serial.println("HAL failed to init!");
    delay(1000);
  }
}

void loop() {
    //
    Serial.println("Erroneously got to loop");
    abort();
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif
