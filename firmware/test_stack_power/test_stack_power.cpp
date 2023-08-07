/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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
#include <TF800A12K.h>

const unsigned long REPORT_PERIOD_MS = 3000;
const float MAX_VOLTS = 12.0;

MachineConfig *machineConfig;
TaskProperties _properties;


void report(MachineConfig *machineConfig) {
  Serial.print("V : ");
  Serial.println(machineConfig->report.stack_voltage);
  Serial.print("A : ");
  Serial.println(machineConfig->report.stack_amps);
  Serial.print("O : ");
  Serial.println(machineConfig->report.stack_ohms);
}
unsigned long time_of_last_report = 0;
void test_stack_high_amperage(){

  Serial.println("Started Test_Stack_High_Amperage()!");
  delay(100);

  AbstractPS* stack = new SL_PS("FIRST_STACK",0);
  Serial.println("About to init!");
  delay(100);
  stack->init();
  ((SL_PS *) stack)->DEBUG_SL_PS = 1;
  Serial.println("stack initialized!");
  delay(1000);

  float amps = 0.0;
  stack->updateAmperage(amps,machineConfig);
  // set the max volts!
  stack->updateVoltage(MAX_VOLTS,machineConfig);

  Serial.println("amps set!");
  char myBuff[64];
  int len = sizeof(myBuff)-1;

  while(true) {
    delay(1000);
    if(Serial.available( ) > 0)
      {
        size_t read = Serial.readBytesUntil('\n', myBuff, len);
        myBuff[read] = '\0';
        Serial.println(myBuff);
        amps = atof(myBuff);
        Serial.print("received: ");
        Serial.println(amps);
        stack->updateAmperage(amps,machineConfig);
      };
    unsigned long now_ms = millis();
    if ((now_ms - time_of_last_report) > REPORT_PERIOD_MS) {
      report(machineConfig);
      time_of_last_report = now_ms;
    }
  };

  Serial.println("terminating test!");
  delay(100);
  TEST_ASSERT_TRUE(true);
}

void process() {
  UNITY_BEGIN();
  RUN_TEST(test_stack_high_amperage);
  UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>

void setup() {
#ifdef RIBBONFISH
  OxCore::serialBegin(115200UL);

  pinMode(RF_STACK, OUTPUT);

  machineConfig = new MachineConfig();
  machineConfig->hal = new MachineHAL();

  bool initSuccess  = machineConfig->hal->init();
  if (!initSuccess) {
    Serial.println("Could not init Hardware Abastraction Layer Properly!");

#endif
    OxCore::TaskProperties cogProperties;
    cogProperties.name = "cog";
    cogProperties.id = 20;
    cogProperties.state_and_config = (void *) &machineConfig;
    Serial.println("About to run test!");


    delay(1000);
    Serial.println("About to run test!");
    delay(100);
    process();
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
