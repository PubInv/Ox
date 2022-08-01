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

#include "fault_task.h"
#include<cmath>
#include <abstract_temperature.h>

using namespace std;


namespace OxApp
{
    bool FaultTask::_init()
    {
        OxCore::Debug<const char *>("FaultTask init\n");

        return true;
    }

    bool FaultTask::_run()
    {
      // Note: I image the fault tolerance for this
      // should be carefully split into those
      // conditions which can be computed from
      // a single sensor, and those conditions which
      // can only be algorithmically detected from
      // the entire machine model.
        return true;
    }

}
