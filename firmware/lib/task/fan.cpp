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

#include "fan.h"

#define DEBUG_FAN 0

#define PERIOD 100
unsigned long volatile ts=0;
unsigned long volatile cnt = 0;
unsigned long volatile ocnt = 0;
unsigned long volatile duration = 0;

void refresh() {
  unsigned long m = millis();
  if (ts + PERIOD < m) {
    ocnt = cnt;
    duration = m - ts;
    ts = m;
    cnt = 0;
  }
}
//Calculates the RPM based on the timestamps of the last 2 interrupts. Can be called at any time.

namespace OxApp {
 void tachISR() {
   cnt++;
    refresh();
 }

  void Fan::_init() {
    attachInterrupt(digitalPinToInterrupt(RF_FAN_TACH),tachISR,FALLING);
  }

  unsigned long Fan::_calcRPM(){
  refresh();
  if (DEBUG_FAN) {
    Serial.println("CALC:");
    Serial.println(ocnt);
    Serial.println(duration);
  }
  if (duration != 0) {
    return (long) (60000.0 * (float) ocnt / ((float) duration * 2.0));
  } else {
    return 0;
  }
}

    void Fan::update(float pwm_ratio) {
        _pwm_ratio = pwm_ratio;
        int pwm_setting = _pwm_ratio * 255;
        analogWrite(this->pin,pwm_setting);
#ifdef RIBBONFISH
        float rpm = _calcRPM();
        if (DEBUG_FAN) {
          OxCore::Debug<const char *>("Fan RPM:");
          OxCore::DebugLn<float>(rpm);
          OxCore::Debug<const char *>("Fan update: ");
          OxCore::Debug<int>(id);
          OxCore::Debug<const char *>("   ");
          OxCore::DebugLn<const char *>(name);
          OxCore::DebugLn<const char *>(" Voltage: ");
          // TODO: Replace this with the OUTPUT_RESOLOUTION
          OxCore::Debug<const char *>(" pwm_setting: ");
          OxCore::DebugLn<float>(pwm_setting);
        }
#else
        OxCore::Debug<const char *>("Fan update: (NOT IMPLEMENTED)");
        OxCore::Debug<const char *>("   ");
        OxCore::Debug<const char *>(name);
        OxCore::Debug<const char *>(" pwm_ratio: ");
        OxCore::DebugLn<float>(_pwm_ratio);
#endif
    }
}
