/*

  stage2SerialReport.h -- read temperatures sensors

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

#ifndef STAGE2_SERIAL_REPORT_H
#define STAGE2_SERIAL_REPORT_H

#include <core_defines.h>
#include <core.h>

using namespace OxCore;

class Stage2SerialReportTask : public OxCore::Task
{
public:
  Stage2SerialReportTask();

  MachineConfig *machineConfigs[3];

  int DEBUG_SERIAL_REPORT = 0;
  // since we have three, we report them a little less often to keep the
  // serial traffic down
  int PERIOD_MS = 30*1000;
private:
  bool _init() override;
  bool _run() override;
};


#endif
