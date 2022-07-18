/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

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

#ifndef COG_TASK_H
#define COG_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <core.h>
#include "../collections/array.h"
#include "heater.h"


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
    class CogTask : public OxCore::Task
    {
    public:

      // TODO: This should probably be done dynamically, not here...

#if BUILD_ENV_NAME != due_ribbonfish
      const static int NUM_HEATERS = 2;
      const static int NUM_TEMPERATURE_SENSORS = 3;
#else // RIBBONFISH
      const static int NUM_HEATERS = 2;
      // There are really several senosrs, but they are indexed!
      const static int NUM_TEMPERATURE_SENSORS = 1;
#endif

      Temperature::AbstractTemperature* _temperatureSensors;
      Heater _heaters[NUM_HEATERS];
    private:
        bool _init() override;
        bool _run() override;
        void _updatePowerComponents();
        void _printValveState(uint8_t vs);
        void _configTemperatureSensors();
        void _readTemperatureSensors();
      // We will use a model for mocking, which may grow into
      // something...
        Model model;
      void RunForward(float t,Model& m);
    };
}

#endif
