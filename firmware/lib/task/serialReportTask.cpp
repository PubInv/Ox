/*

  serialReport.cpp -- read temperatures sensors

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

#include <serialReportTask.h>


bool SerialReportTask::_run()
{
  if (DEBUG_SERIAL_REPORT > 0) {
    OxCore::Debug<const char *>("Running SerialReport\n");
  }
  getConfig()->outputReport(getConfig()->report);
}

bool SerialReportTask::_init()
{
}
SerialReportTask::SerialReportTask() {
}
