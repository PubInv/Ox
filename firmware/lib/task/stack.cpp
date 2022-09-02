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

#include "stack.h"

#define DEBUG_LEVEL 0

#define USE_DROK_200626 1


// return the specific DAC output level corresponding
// as closely as possible to the voltage we want the DROK
// power supply to produce
static int DROK_PWM_from_voltage(float o_voltage) {
  // When used from the DAC, we can only acheive voltages
  // of 1/6 * 3.3 V = 0.55 V and 5/6 * 3.3 V = 2.75 Volts.
  const float minimumV = 0.55;
  const float maximumV = 2.75;
  const int PWM_RANGE = 255; // This is resolution dependent!
  // The DROK 200626 seems to map:
  // 0.0 V input = 0.0 V output
  // 5.0 V input = 15.9 V output
  // We will first map the output_voltage into
  // the input_voltage, then limit it as per the DAC
  float i_voltage = (5.0 / 15.9) * o_voltage;
  // Now we have a desired i_voltage that the DROK
  // would like to see, we do our best to produce a digital
  // signal to produce it:
  int i_digital = (int)
    ((i_voltage - minimumV) * ((float) PWM_RANGE))
    /  (maximumV - minimumV);

  // limit us to the legal range
  i_digital = max(0,i_digital);
  i_digital = min(PWM_RANGE,i_digital);


  if (DEBUG_LEVEL > 1) {
    OxCore::Debug<const char *>("STACK i_digital :");

    float probable_voltage = 15.9 * ((minimumV + (((float) i_digital / (float) PWM_RANGE) * (maximumV - minimumV)))/ 5.0);
    OxCore::DebugLn<int>(i_digital);    OxCore::Debug<const char *>("STACK probable voltage :");
    OxCore::DebugLn<float>(probable_voltage);
  }
  return i_digital;

}
void Stack::updateAmperage(float amperage) {

}
void Stack::updateVoltage(float voltage) {
  // TODO: in general this is a little complicated
  // and depends on the power supply. In this case,
  // at the moment, we are using DROK power supply
  // with specific settings.
  int pwm_setting;
#ifdef USE_DROK_200626
  pwm_setting = DROK_PWM_from_voltage(voltage);
#else
  OxCore::DebugLn<const char *>("INTERNAL ERROR: No Stack Power Supply Model Defined ");
  // we can do nothing so we return,
  // though we should probably signal an internal error!
  // TODO: Add a way to signal a fault here
  return;
#endif
  analogWrite(this->pin,pwm_setting);

#ifdef RIBBONFISH
  if (DEBUG_LEVEL > 0) {
    OxCore::Debug<const char *>(name);
    OxCore::Debug<const char *>(" Voltage: ");
    OxCore::DebugLn<float>(voltage);
  }
#else
  OxCore::Debug<const char *>("STACK update: (NOT IMPLEMENTED)");
  OxCore::Debug<const char *>("   ");
  OxCore::Debug<const char *>(name);
  OxCore::Debug<const char *>(" pwm_ratio: ");
  OxCore::DebugLn<float>(_pwm_ratio);
#endif
}
