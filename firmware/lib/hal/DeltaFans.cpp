// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include "DeltaFans.h"
#include <math.h>


#define PERIOD 1000

  unsigned long volatile tach_data_ts[NUMBER_OF_FANS];
  unsigned long volatile tach_data_cnt[NUMBER_OF_FANS];
  unsigned long volatile tach_data_ocnt[NUMBER_OF_FANS];
  unsigned long volatile tach_data_duration[NUMBER_OF_FANS];


//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.
//namespace tach_data {

  void tachISR(uint8_t i) {
    tach_data_cnt[i]++;
    refresh_tach_data(i);
  }
  void tachISR0() {
    tachISR(0);
  };
  void tachISR1() {
    tachISR(1); };
  void tachISR2() {
    tachISR(2); };
  void tachISR3() {
    tachISR(3); };

  void refresh_tach_data(uint8_t i) {
    unsigned long m = millis();
    if (tach_data_ts[i] + PERIOD < m) {
      tach_data_ocnt[i] = tach_data_cnt[i];
      tach_data_duration[i] = m - tach_data_ts[i];
      tach_data_ts[i] = m;
      tach_data_cnt[i] = 0;
    }
  }
//}

// using namespace tach_data;

unsigned long DeltaFans::_calcRPM(uint8_t i){
  refresh_tach_data(i);
  if (DEBUG_FAN > 1) {
    Serial.print("CALC: " );
    Serial.print(i);
    Serial.print(tach_data_ocnt[i]);
    Serial.println(tach_data_duration[i]);
  }
  if (tach_data_duration[i] != 0) {
    // I think these are 4-pole fans
    return (long) (60000.0 * ((float) tach_data_ocnt[i] * 4.0) / ((float) tach_data_duration[i] ));
  } else {
    return 0;
  }
}


void DeltaFans::printRPMS() {
  Serial.println("RPMS:");
  for(uint8_t i = 0; i < 4; i++) {
    long rpm = _calcRPM(i);
    Serial.print("rpm: ");
     Serial.print(i);
     Serial.print(" : ");
     Serial.println(rpm);
  }
}

// Note: I don't believe this should ever be used.
// Using less than full power for these Delta fans
// prevents the PWM signal from working properly.
// Nevertheless I'm leaving it in place in case it
// is needed for some other purpose.
void DeltaFans::motorControl(int s)
{
  int q = map(s, SPEED_MIN, SPEED_MAX, 0, 255);
  if (DEBUG_FAN > 0 ) {
    Serial.print("Putting out speed to fan control board:");
    Serial.println(q);
  }
  analogWrite(MOTOR_OUT_PIN, q);
}

// m = motor -- 0 - 3
void DeltaFans::PWMMotorControl(float s, int m)
{
  int q = map(s*100, SPEED_MIN, SPEED_MAX, 0, 255);

  if (DEBUG_FAN > 0 ) {
    Serial.print("m : q");
    Serial.print(m);
    Serial.print(" : ");
    Serial.println(q);
  }
  analogWrite(PWM_PIN[m], q);
}


void DeltaFans::_init() {

  PWM_PIN[0] = 9;
  PWM_PIN[1] = 10;
  PWM_PIN[2] = 11;
  PWM_PIN[3] = 12;
  TACH_PIN[0] = A0;
  TACH_PIN[1] = A1;
  TACH_PIN[2] = A2;
  TACH_PIN[3] = A3;

  pinMode(MOTOR_OUT_PIN, OUTPUT);

  for(int i = 0; i < 4; i++) {
    tach_data_ts[i] = 0;
    tach_data_cnt[i] = 0;
    tach_data_ocnt[i] = 0;
    tach_data_duration[i] = 0;
    pinMode(PWM_PIN[i], OUTPUT);
    pinMode(TACH_PIN[i],INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[0]),tachISR0,FALLING);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[1]),tachISR1,FALLING);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[2]),tachISR2,FALLING);
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[3]),tachISR3,FALLING);

}



// At present, we will use the same ratio for all fans;
// this is an oversimplification
void DeltaFans::update(float pwm_ratio) {

  motorControl((pwm_ratio == 0.0) ?
               0 :
               100);

  // this is an experiment...
  // The 4 fans are really two powerful. We need some
  // way to selectively turn fans on and off.
  // Exactly how to do this is unclear.
  // A simple approach is that if the pwm_ration is
  // greater than 0.75, use all four, if greater than 0.5,
  // use 3, if greater than 0.25, use 2, if less than 0.25,
  // use 1.

  int num = ceil(pwm_ratio * 4.0);

  for(int i = 0; i < num; i++) {
    _pwm_ratio[i] = pwm_ratio;
    this->PWMMotorControl(_pwm_ratio[i],i);
  }
  for(int i = num; i < 4; i++) {
    _pwm_ratio[i] = 0.0;
    this->PWMMotorControl(_pwm_ratio[i],i);
  }

  if (DEBUG_FAN > 0 ) {
    Serial.print("PWM ratio:  num / ratio : ");
    Serial.print(num);
    Serial.print(" : ");
    Serial.println(pwm_ratio);
    printRPMS();
  }
}
