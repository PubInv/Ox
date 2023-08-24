/*
 Copyright (C) 2022 Robert L. Read

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

#ifndef FAULT_TASK_H
#define FAULT_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/array.h"
#include "heater.h"
#include <machine.h>

#include <abstract_temperature.h>

#ifndef RIBBONFISH
#include <mock_temperature_sensor.h>
#else
#include <DS18B20_temperature.h>
#endif


#include "model.h"


namespace OxApp
{
    // Runs the Pressure Swing Adsorption cycle
    class FaultTask : public OxCore::Task
    {
    public:
    private:
      bool _init() override;
      bool _run() override;
      // This would go into the abstract class.
    };
}

#endif
