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

const int DEBUG_PID = 0;



namespace OxApp
{
  FanPIDTask::FanPIDTask() {

    /* Setting the initial parameters is always weird for PID controller..
Traditionally it is done with a guess and then tuned.
I prefer to be very clear about the type of the "process variable"
and the "control variable". In the case of the DeltaFans, we are using
PWM values (that is, a range from full-off to full-on, which we can
represent at the interval [0.0...1.0] as the control variable is
the measured flow in ml per second.

By observation, the range of flow we can acheive is approximately
500 ml/s (low side) to 2000 ml/s (high side). Sadly these fans are
not very controllable on the low side. We may be able to get more
control by turning on only one of our 4 fans; but that is future work.,

This flow will of course be mediated by the airway resistance, and
can even change dynamically, so a PID controller is reasonable,
although possibly not needed.

We want our "proportional step" to be the output range divided by
our difference from our setpoint. The setPoint caculation is in ml/S.
If we imagine taking a 10th of the total range as an initial step,
then 200 ml/S should map on to 0.1 in the range.
Therefore our initial FKp = 0.1 / 200 ml/S = 0.0005.
Technically the units of this are really V/(ml/S), but
people rarely think about that when thinking about PID controllers.
     */
    double FKp = 0.0005;
    double FKi = 0.0;
    double FKd = 0.0;
    // This is actually a slow process...it might be
    // clearer to make this 500 ms, but we will only
    // call the process so often.
    const int SAMPLE_TIME_MS = 2000;
    // fanSpeed is measured betwen 0.0 and 1.0 (and ends
    // up being a PWM duty cycle)
    this->pidControllerFlow =
      new PID(&(this->Input_mlps), &(this->fanSpeed_Output),
              &(this->Flow_Setpoint_mlps), FKp, FKi, FKd, DIRECT);
    this->pidControllerFlow->SetOutputLimits(-1.0, 1.0);
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
        getConfig()->fanPWM = 0.0;
        return true;
      } else if (ms == Cooldown) {
        getConfig()->fanPWM = 1.0;
        return true;
      } else {
        float flow_mlps = getConfig()->TARGET_FLOW_ml_per_S;

        this->Flow_Setpoint_mlps = flow_mlps;
        double flow_ml_per_s = getConfig()->hal->_flowsensor->flowIn_ml_per_s();
        if (DEBUG_PID > 0) {
          Serial.print("XXXXX Measured Flow:");
          Serial.println(flow_ml_per_s);
        }

        if (flow_ml_per_s >= 4550.0) {
          // This is the error condition for this sensor!
        } else {
          getConfig()->report.flow_ml_per_s = flow_ml_per_s;
        }
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

        // now we clamp the value to [0.001,1.0]
        // We don't let it be a hard 0.0 because that means
        // turn the whole board off, which is useful for
        // when in the "off" state!
        s = min(s, 1.0);
        s = max(s, 0.01);
        this->final_fanSpeed = s;

        getConfig()->fanPWM = s;
        getConfig()->hal->_updateFanSpeed(getConfig()->fanPWM);

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
