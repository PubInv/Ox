// Copyright (C) 2022 Robert Read.

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

/*

  This file exists to be an interface to the Sanyo Ace B97 24-volt
  4-wire fan, document here:
  https://docs.rs-online.com/1c09/0900766b816e68bb.pdf

  This file was copypastaed from DeltaFans, a slightly different fan.

  We probably should create a generic class for these fans but
  they very enough that it may not be worth it at present.

  Tom Taylor of AmOx sent in this statement about the CFC:

  "The fan should be set to 60% during ramp up and ramp down,
  but it can be throttled back at operating temperature to
  increase power efficiency of the system. We have found blower
  values higher than 60% too high for the 1,000W heater
  present in the system. A setting of 100% is too much for the heater
  to keep up with and it will never make it to operating
  temperature. Lower values can make the stack delta too high during ramp up."

  This raises the questions should we encode in this file the 60% limit,
  and have interpret the scripted value to be 0-100% of 60%. I am
  inclined to do this for safety, but it means the fan will be
  less contollable than it would be. I will try to make that
  clear in the code.

 */


#ifndef SANYOACEB97_H
#define SANYOACEB97_H

#ifdef ARDUINO
#include <Arduino.h>
#else
//#include <cstdint>
#include <math.h>
#endif

// This may seem odd, but you can use two fans in serious
// for redundancy or great flow against pressure.
#define NUMBER_OF_FANS 1

// because these are interrupts, they cannot be members,
// and making them static is complicated...

void refresh_tach_data(uint8_t i);
void tachISR(uint8_t i);
void tachISR0();


class SanyoAceB97 {
public:
  const char *name;
  uint8_t id;
  uint8_t pin;
  // This is a ration from 0.0 to 1.0
  float _pwm_ratio[NUMBER_OF_FANS] = {0.0};

  int PWM_PIN[NUMBER_OF_FANS];
  int TACH_PIN[NUMBER_OF_FANS];

  int SPEED_MAX = 100;// in %
  int SPEED_MIN = 0;//in %
  // This is 60% of full power (255) as recommended for this configuration
  int OPERATING_PWM_THROTTLE = 153;
  int DEBUG_FAN = 2;
public:
  void _init();
  unsigned long _calcRPM(uint8_t i);
  SanyoAceB97() {
    _init();
  };
  SanyoAceB97(const char * name, uint8_t id, uint8_t pin, float pwm_ratio){
    _init();
    this->name = name;
    this->id = id;
    this->pin = pin;
  };
  ~SanyoAceB97(){};

  void printRPMS();
  void fanSpeedPerCentage(int s);

  void update(float pwm_ratio);
};


#endif
