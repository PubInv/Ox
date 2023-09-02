// SanyoAceB97.cpp - Copyright (C) 2021
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



#include "SanyoAceB97.h"
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
    //    refresh_tach_data(i);
  }
  void tachISR0() {
    tachISR(0);
  };

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

unsigned long SanyoAceB97::_calcRPM(uint8_t i){
  refresh_tach_data(i);
  if (DEBUG_FAN > 1) {
    Serial.print("CALC: " );
    Serial.println(i);
    Serial.println(tach_data_ocnt[i]);
    Serial.println(tach_data_duration[i]);
  }
  if (tach_data_duration[i] != 0) {
    // According to the documentation, there will be two falling
    // edges in one revolution..
    // This calculation does not match the documentaiton.
    // https://docs.rs-online.com/1c09/0900766b816e68bb.pdf
    float num_revolutions = (float) tach_data_ocnt[0] / 8.0;
    float one_revolution_time_ms = (float) tach_data_duration[0];
    long rpm = (long) (60000.0 * ( num_revolutions / one_revolution_time_ms));
    return rpm;
  } else {
    return 0;
  }
}


void SanyoAceB97::printRPMS() {
  Serial.println("RPMS:");
  for(uint8_t i = 0; i < NUMBER_OF_FANS; i++) {
    long rpm = _calcRPM(i);
    Serial.print("rpm: ");
    Serial.println(rpm);
  }
}

// Note: I don't believe this should ever be used
// other than to completely turn off the motors.
// Using less than full power for these Delta fans
// prevents the PWM signal from working properly.
// Nevertheless I'm leaving it in place in case it
// is needed for some other purpose.
void SanyoAceB97::fanSpeedPerCentage(int s)
{
  int q = map(s, SPEED_MIN, SPEED_MAX, 0, OPERATING_PWM_THROTTLE);
  if (DEBUG_FAN > 0 ) {
    Serial.print("Putting out speed to fan control board:");
    Serial.println(q);
  }
  analogWrite(PWM_PIN[0], q);
}

// This would be clearer in the the .h!!
void SanyoAceB97::_init() {

  PWM_PIN[0] = 9;
  TACH_PIN[0] = A0;

  for(int i = 0; i < NUMBER_OF_FANS; i++) {
    tach_data_ts[i] = 0;
    tach_data_cnt[i] = 0;
    tach_data_ocnt[i] = 0;
    tach_data_duration[i] = 0;
    pinMode(PWM_PIN[i], OUTPUT);
    pinMode(TACH_PIN[i],INPUT_PULLUP);
  }
  attachInterrupt(digitalPinToInterrupt(TACH_PIN[0]),tachISR0,FALLING);
}



// At present, we will use the same ratio for all fans;
// this is an oversimplification
void SanyoAceB97::update(float pwm_ratio) {

  fanSpeedPerCentage((int)( pwm_ratio * 100));

  _pwm_ratio[0] = pwm_ratio;

  if (DEBUG_FAN > 0 ) {
    Serial.print("PWM ratio:  num / ratio : ");
    Serial.println(pwm_ratio);
    printRPMS();
  }
}
