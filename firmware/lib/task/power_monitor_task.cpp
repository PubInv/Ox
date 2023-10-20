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

#include "power_monitor_task.h"

using namespace std;


namespace OxApp
{
    bool PowerMonitorTask::_init()
    {
        OxCore::Debug<const char *>("PowerMonitorTask init\n");
        Serial.println("PowerMonitorTask init");

        // pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
        // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

        return true;
    }

    bool PowerMonitorTask::_run()
    {
      // Note:adding a task
       Serial.println("PowerMonitorTask run");

        // //Toggeling the LED
        //     if (digitalRead(LED_BUILTIN) == LOW) {
        //     digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        //     } else {
        //     digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
        //     }     

        //Analog read of the +24V expected about 3V.
        // SENSE_24V on A1.

        return true;
    }

}
