// Copyright (C) 2021
// Robert Read, Ben Coombs.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any 4ater version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#ifdef ARDUINO
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <DS3502_digital_pot.h>

#include <core.h>

#include <Adafruit_DS3502.h>

  DS3502DigitalPot::DS3502DigitalPot() {
    ds3502 = Adafruit_DS3502();
    // DIGITAL_POT_HACK
    if (!ds3502.begin()) {
      Serial.println("Couldn't find DS3502 chip");
    } else {
      Serial.println("Found DS3502 chip");
      foundPot = true;
      OxCore::DebugLn<int>(this->foundPot);
    }
  }
void DS3502DigitalPot::setWiper(float fraction) {
  if (foundPot) {
    delay(200);
    int n = (int) (fraction * 127.0);

    Serial.print("PRE Wiper voltage with wiper set to N: ");
    Serial.print(n);
    delay(200);
    ds3502.setWiper(n);
    Serial.print("POST Wiper voltage with wiper set to N: ");
    Serial.print(n);
    delay(200);
    return;
  } else {
    Serial.println("No Digital Pot Found");
  }
  }
#endif
