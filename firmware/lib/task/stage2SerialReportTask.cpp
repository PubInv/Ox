/*

  stage2SerialReport.cpp -- read temperatures sensors

  Copyright (C) 2023 Robert Read.

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

#include <stage2SerialReportTask.h>
#include <stage2_config.h>
#include <stage2_hal.h>

bool Stage2SerialReportTask::_run()
{
  if (DEBUG_SERIAL_REPORT > 0) {
    OxCore::Debug<const char *>("Running Stage2SerialReport Task\n");
  }
  return true;
  float target_temp = getConfig()->report->target_temp_C;
  Stage2HAL* hal = (Stage2HAL *)(getConfig()->hal);
  float measured_temp = hal->getTemperatureReading(
                                                   getConfig()->s2heater,
                                                   getConfig());

  Serial.println("get measure temp!");
  Serial.println(getConfig()->s2heater);
  Serial.println(measured_temp);
  Serial.println((unsigned long) getConfig());
  delay(100);
  float duty_cycle = getConfig()->fanDutyCycle;
  Serial.println("duty cycle");
  delay(100);
  Serial.println(duty_cycle);
  getConfig()->outputStage2Report(getConfig()->s2heater,
                                  getConfig()->report,
                                  target_temp,
                                  measured_temp,
                                  duty_cycle);
  Serial.println("OutputStage2Report done!");
  delay(100);
}
bool Stage2SerialReportTask::_init()
{
    OxCore::Debug<const char *>("Stage2SerialReport Task init\n");
}
Stage2SerialReportTask::Stage2SerialReportTask() {
}
