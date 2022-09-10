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

#include "fanPID_task.h"
#include <PID_v1.h>
#include <core.h>

using namespace std;

const int DEBUG_PID = 1;

namespace OxApp
{
  FanPIDTask::FanPIDTask() {
    double FKp = 0.005;
    double FKi = 0.0;
    double FKd = 1.0;
    // This is actually a slow process...it might be
    // clearer to make this 500 ms, but we will only
    // call the process so often.
    const int SAMPLE_TIME_MS = 2000;
    // fanSpeed is measured betwen 0.0 and 1.0 (and ends
    // up being a PWM duty cycle)
    this->pidControllerFlow =
      new PID(&(this->Input_mlps), &(this->fanSpeed_Output),
              &(this->Flow_Setpoint_mlps), FKp, FKi, FKd, DIRECT);
    this->pidControllerFlow->SetOutputLimits(-100.0, 100.0);
    this->pidControllerFlow->SetSampleTime(SAMPLE_TIME_MS);
    this->pidControllerFlow->SetMode(AUTOMATIC);
  }

    bool FanPIDTask::_init()
    {
        OxCore::Debug<const char *>("FanPIDTask init\n");
        return true;
    }

    bool FanPIDTask::_run()
    {

      // This is actually state dependent;
      // in some cases we will set the setpoint to the max
      // and in other cases (off) it will be zero
      // however, for now, my goal is just to get the PID
      // working

      MachineState ms = getConfig()->ms;
      if (ms == Off) {
        getConfig()->fanPWM = 0;
        return true;
      } else if (ms == Cooldown) {
        getConfig()->fanPWM = 100;
        return true;
      } else {
        float flow_mlps = getConfig()->TARGET_FLOW_ml_per_S;

        this->Flow_Setpoint_mlps = flow_mlps;

        // This is currently not available;
        // I suppose it is acceptable to read it from the
        // the report...

        // bool flowing = _flowsensor->isAirFlowing();
        // if (!flowing) {
        //   OxCore::Debug<const char *>("POTENTIAL ERROR, AIR FLOW MAY BE INSUFFICIENT ");
        // }
        // getConfig()->report.air_flow_sufficient = flowing;

        float f_mlps = getConfig()->report.flow_ml_per_s;

        this->Input_mlps = f_mlps;
        if (DEBUG_PID > 0) {
          OxCore::Debug<const char *>("Target flow (ml per second): ");
          OxCore::DebugLn<double>(flow_mlps);
          OxCore::Debug<const char *>("Input flow (ml per second): ");
          OxCore::DebugLn<double>(this->Input_mlps);
        }

        double previousInput = this->Input_mlps;

        pidControllerFlow->Compute();

        double s = this->fanSpeed_Output + this->final_fanSpeed;

        // now we clamp the value to [0.0,1.0]
        s = min(s, 100.0);
        s = max(s, 1.0);
        this->final_fanSpeed = s;

        getConfig()->fanPWM = s/ 100.0;

        if (DEBUG_PID > 0) {
          OxCore::Debug<const char *>("previous input ");
          OxCore::DebugLn<double>(previousInput);
          OxCore::Debug<const char *>("Final fanSpeed_Output ");
          OxCore::DebugLn<double>(this->fanSpeed_Output);
          OxCore::Debug<const char *>("Final fan Speed (PWM) ");
          OxCore::DebugLn<double>(this->final_fanSpeed);
        }

        return true;
      }
    }

}
