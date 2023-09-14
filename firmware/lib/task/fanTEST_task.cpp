/*
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

#include "fanTEST_task.h"
#include <core.h>
#include <machine.h>


using namespace std;

namespace OxApp
{
  FanTESTTask::FanTESTTask() {
  }

  bool FanTESTTask::_init()
  {
    OxCore::Debug<const char *>("FanTESTTask init\n");
    fan = DeltaFans("FIRST_FAN",0,RF_FAN,1.0);

    fan._init();
    fan.DEBUG_FAN = 2;
    return true;
  }

  bool FanTESTTask::_run()
  {
    Serial.println("RUN TEST TASK:");
    fan.printRPMS();
    float f = 0.0;
    int n = 10;
    f = ((float) test_count) / (float) n;

    if ((test_count % 2) == 0) {
      f = 0.0;
    } else {
      f = 1.0;
    }

    f = min(f, 1.0);
    f = max(f, 0.00);
    fan.update(f);
    test_count++;
    test_count = test_count % n;
    return true;
  }

}
