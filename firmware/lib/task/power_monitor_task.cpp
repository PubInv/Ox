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
#include <machine.h>


using namespace std;


namespace OxApp
{
    bool PowerMonitorTask::_init()
    {
        OxCore::Debug<const char *>("PowerMonitorTask init\n");
        Serial.print("First raw analogRead(SENSE_24V)= ");
        Serial.println(analogRead(SENSE_24V ));
        Serial.print("lowThreshold24V= ");
        Serial.println(lowThreshold24V);
        return true;
    }

    bool PowerMonitorTask::_run()
    {
      // Note:adding a task
       Serial.println("PowerMonitorTask run");

        //Analog read of the +24V expected about 3.25V at ADC input.
        // SENSE_24V on A1.
        // Full scale is 1023, ten bits for 3.3V.
        //30K into 4K7 
        const long R1=30000;
        const long R2=4700;
        const float Vcc = 3.3;

        Serial.print("analogRead(SENSE_24V)= ");
        Serial.println(analogRead(SENSE_24V) * ((Vcc * (R1+R2))/(1023.0 * R2))); 

        if (analogRead(A1) > lowThreshold24V) {
            powerIsGood = true;
            Serial.println("+24V power monitor reports good.");
            return true;
        }else{
            powerIsGood = false;
            Serial.println("+24V power monitor reports bad.");
            return false;
        }
    }

        bool PowerMonitorTask::setThreshold24V(int setThreshold)
    {
        lowThreshold24V = setThreshold;
        OxCore::Debug<const char *>("Setting 24V threshold to: ");
        OxCore::Debug<int >(lowThreshold24V);
        // Serial.print("Setting 24V threshold to: ");
        // Serial.println(lowThreshold24V);
        return true;
    }

}
