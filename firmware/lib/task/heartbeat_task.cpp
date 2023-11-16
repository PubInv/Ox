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

#include "heartbeat_task.h"

using namespace std;


namespace OxApp
{
    bool HeartbeatTask::_init()
    {
        OxCore::Debug<const char *>("HeartbeatTask init\n");

        pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

        return true;
    }

    bool HeartbeatTask::_run()
    {
      // Note:adding a heartbeat task
      // Serial.println("HeartbeatTask run");
        //Toggeling the LED
            if (digitalRead(LED_BUILTIN) == LOW) {
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            } else {
            digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
            }     
        return true;
    }

}
