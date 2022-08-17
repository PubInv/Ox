// Copyright (C) 2021
// Robert Read.

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
#include <Arduino.h>
#include <SPI.h>
#endif
#include <core.h>
#include "SensirionSFM3X00.h"

#define FLOW_SENSOR_ADDRESS 0x40

#define DEBUG_FLOW_SENSOR 0

SensirionFlow::SensirionFlow() {
  flowSensor = new SFM3X00(FLOW_SENSOR_ADDRESS);
  flowSensor->begin();

  delay(1000);


  // print various sensor values
  Serial.println();
  Serial.print("sensor serial number: ");
  Serial.println(flowSensor->serialNumber, HEX);
  Serial.print("sensor article number: ");
  Serial.println(flowSensor->articleNumber, HEX);
  Serial.println();
  Serial.print("read scale factor: ");
  Serial.println(flowSensor->flowScale);
  Serial.print("read flow offset: ");
  Serial.println(flowSensor->flowOffset);

}

float SensirionFlow::flowInSLM() {
   // read flow from sensor and print
  float flow = flowSensor->readFlow();

  if(flowSensor->checkRange(flow))
  {
    Serial.print("flow exceeded sensor limits:  ");
    Serial.print(flow);
    Serial.println(" slm");
  }
  else
  {
    if (DEBUG_FLOW_SENSOR > 1) {
      Serial.print("flow : ");
      Serial.print(flow);
      Serial.println(" slm");
    }
  }
  return flow;
}

float SensirionFlow::flowIn_ml_per_s() {
  float slm = flowInSLM();
  float ml_per_s = slm * 1000.0 / 60.0;
  return ml_per_s;
}

boolean SensirionFlow::isAirFlowing() {
  return flowIn_ml_per_s() > MINIMUM_FLOW_ml_per_s;
}
