/*
  cog_task.cpp -- main control algorithm or ceramic oxygen generator

  Copyright 2023, Robert L. Read

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

#include "cog_task.h"
#include <cmath>
#include <abstract_temperature.h>
#include <TF800A12K.h>

// from: https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
// This should be made into a separte task,
// this is just for debugging...
// TODO: Move this into the core, and invoke it within a DEBUG_LEVEL guard.
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}


using namespace std;


namespace OxApp
{
  // TODO: This class should probably be rewritten with specific state transition functions.
  // This would allow us to log transition events elegantly. As it is, we have to be idempotent...
  // in the "off" state we have to keep turning everything off because we don't know if we were
  // just turned off or have been off for 24 hours.

  // TODO: Most of this should be moved into the machine definition
  bool CogTask::_init()
  {
    OxCore::Debug<const char *>("CogTask init\n");
    getConfig()->fanDutyCycle = 0.0;
    wattagePIDObject = new WattagePIDObject();
    return true;
  }

  void CogTask::printGenericInstructions() {
    Serial.println("Enter s:1 to Turn On, s:0 to Turn Off.");
    Serial.println("Enter a:XX.X to set (a)mperage, (w)attage, (f)an speed (h)eater set p., and (r)amp rate.");

  }

  COG_HAL* CogTask::getHAL() {
    return (COG_HAL *) (getConfig()->hal);
  }

  float CogTask::getTemperatureReadingA_C() {
    return getConfig()->report->post_heater_C;
  }
  float CogTask::getTemperatureReadingB_C() {
    return getConfig()->report->post_getter_C;
  }
  float CogTask::getTemperatureReadingC_C() {
    return getConfig()->report->post_stack_C;
  }

  // I am currently in the process of implementing the "One-Button Algorithm".
  // I will first implement the helper functions. -rlr
  float CogTask::computeTotalWattage(float controlTemp) {
    // This has to use a PID controller, which will require some thought
    // to determine how we add this in.


    // somehow we need to make sure this functionality is accomplished.
    // Possibly it is already done in the various state machines.
    // MachineState ms = getConfig()->ms;
    // if ((ms == Off) || (ms == EmergencyShutdown) || (ms == OffUserAck)) {
    //   // in this case, we do nothing...but we will put the set point
    //   // to room temperature.
    //   this->temperatureSetPoint_C = 25.0;
    //   getConfig()->report->heater_duty_cycle = 0.0;
    //   dutyCycleTask->dutyCycle = 0.0;
    //   getConfig()->report->heater_duty_cycle = dutyCycleTask->dutyCycle;
    //  return true;
    // }
    float totalWattage = wattagePIDObject->compute(controlTemp);
    return totalWattage;
  }
  float CogTask::computeTargetStackWattage(float targetTotalWattage, float heaterWatts, float currentTemp, float B, float C, float targetStackWatts) {
    float BC = (B + C) / 2.0;
    // if the operating temp is higher than the current setpoint temp and and the heater is off,
    // we ahve not choice but to decrease the stack watts...this is a bit of "magic"
    // that has no good rationale.
    if ((BC > currentTemp) && (heaterWatts <= 0.0)) {
      return targetStackWatts - getConfig()->DECREASE_STACK_WATTAGE_INCREMENT_W;
    }
    // here we implement a straight-line decrease in statck wattage proportional
    // to the difference C - B
    float y;
    float M = getConfig()->M_OBA_W;
    if (C > B) {
      float d = C - B;
      float Q = getConfig()->Q_OBA_C;
      y = d * M / Q + M;
    } else {
      y = M;
    }
    float L = getConfig()->L_OBA_W;
    float w = max(0.0,min(L,y));
    return min(w,targetTotalWattage);
  }
  float CogTask::computeFanSpeedTargetFromSchedule(float temp) {
    float t = max(0,temp);
    float a = getConfig()->FAN_SPEED_MIN_p;
    float z = getConfig()->FAN_SPEED_MAX_p;
    float f = z - a;
    float ambient = getConfig()->NOMINAL_AMBIENT_c;
    float min_speed_temp = getConfig()->FAN_SPEED_TEMP_FOR_MIN_SPEED_c;
    float r = max(0,min_speed_temp - temp)/
      (min_speed_temp - ambient);
    return r*f + a;
  }
  bool CogTask::heaterWattsAtFullPowerPred(float watts) {
    return watts > getConfig()->HEATER_MAXIMUM_WATTAGE_MEASURED_DEFINITON;
  }
  float CogTask::computeFanSpeedTarget(float currentTargetTemp, float temp, float heaterWatts) {
    float fs_p = computeFanSpeedTargetFromSchedule(temp);
    float diff = currentTargetTemp - temp;
    float final_c = getConfig()->FAN_SPEED_ADJUSTMENT_FINAL_THRESHOLD_c;
    float init_c = getConfig()->FAN_SPEED_ADJUSTMENT_INITIAL_THRESHOLD_c;
    if (heaterWattsAtFullPowerPred(heaterWatts) && ((diff > init_c))) {
      float min_p = getConfig()->FAN_SPEED_MIN_p;
      // m is literally the slope in our linear equation
      float m =  -(fs_p - min_p)/(init_c - final_c);
      float nfs_p =  max(getConfig()->FAN_SPEED_MIN_p,m * diff + fs_p);
      return nfs_p;
    } else {
      return fs_p;
    }
  }

  void CogTask::oneButtonAlgorithm(float &totalWattage_w,float &stackWattage_w,float &heaterWattage_w,float &fanSpeed_p) {
    const float A = getTemperatureReadingA_C();
    const float B = getTemperatureReadingB_C();
    const float C = getTemperatureReadingC_C();

    totalWattage_w = computeTotalWattage(A);
    const float cur_heater_w = getConfig()->CURRENT_HEATER_WATTAGE_W;
    const float sw = computeTargetStackWattage(totalWattage_w,
                                               cur_heater_w,
                                               A,B,C,
                                               getConfig()->CURRENT_STACK_WATTAGE_W);
    const float T = (B+C) / 2.0;

    // We could simulate the stack wattage as a function of T,
    // or we could just use the most recently measured stack wattage...
    // I believe doing the latter is more accurate.
    // Possibly we need to fudge this by adding 1 watt to it
    // so that we don't get stick at the present value.
    const float FUDGE_STACK_WATTS = 1.0;
    stackWattage_w = min(getConfig()->report->stack_watts + FUDGE_STACK_WATTS,
                              sw);
    heaterWattage_w = max(0,
                          min(totalWattage_w - stackWattage_w,
                              getConfig()->HEATER_MAXIMUM_WATTAGE));

    fanSpeed_p = computeFanSpeedTarget(getConfig()->SETPOINT_TEMP_C, T,heaterWattage_w);
  }


  bool CogTask::_run()
  {
    //Check for AC power, ie for +24V
    bool powerIsOK = updatePowerMonitor();
    if (!powerIsOK){
      Serial.println("AC Power (+24V) FAIL.");
    }
    // Report fan speed
    getConfig()->report->fan_rpm =
      getHAL()->_fans[0]._calcRPM(0);


    this->StateMachineManager::run_generic();

    if (DEBUG_LEVEL > 0) {
      Serial.print("Free Memory: ");
      Serial.println(freeMemory());
    }
  }

  // We believe someday an automatic algorithm will be needed here.
  float CogTask::getFanSpeed(float t) {
    return getConfig()->FAN_SPEED;
  }

  // Here is where we attempt to bring in both the amperage
  // and the wattage limitation (but amperage is the "plant"
  // variable that we can control.
  float CogTask::computeAmperage(float t) {
    float max_a_from_raw = getConfig()->MAX_AMPERAGE;
    float max_a_from_wattage =
      sqrt(
           getConfig()->MAX_STACK_WATTAGE /
           getConfig()->report->stack_ohms);

    if (DEBUG_LEVEL > 0) {
      Serial.print("max_a_from_raw , max_a_from_wattage");
      Serial.print(max_a_from_raw);
      Serial.print(" ");
      Serial.println(max_a_from_wattage);
    }
    return min(max_a_from_raw,max_a_from_wattage);
  }


  void CogTask::turnOff() {
    float fs = 0.0;
    getConfig()->fanDutyCycle = fs;
    getConfig()->FAN_SPEED = 0.0;
    getHAL()->_updateFanPWM(fs);
    getConfig()->report->fan_pwm = fs;
    _updateStackVoltage(getConfig()->MIN_OPERATING_STACK_VOLTAGE);
    // Although after a minute this should turn off, we want
    // to do it immediately
    StateMachineManager::turnOff();
  }

  MachineState CogTask::_updatePowerComponentsOff() {
    turnOff();
    return Off;
  }

bool CogTask::updatePowerMonitor()
    {
      // Note:adding a task
       if (DEBUG_LEVEL >0 ) Serial.println("PowerMonitorTask run");

        //Analog read of the +24V expected about 3.25V at ADC input.
        // SENSE_24V on A1.
        // Full scale is 1023, ten bits for 3.3V.
        //30K into 4K7
        const long R1=30000;
        const long R2=4700;
        const float Vcc = 3.3;
        bool powerIsGood = false;
        int lowThreshold24V = 1023 * 3 / 4;

        if (DEBUG_LEVEL >0 )  Serial.print("analogRead(SENSE_24V)= ");
        if (DEBUG_LEVEL >0 )  Serial.println(analogRead(SENSE_24V) * ((Vcc * (R1+R2))/(1023.0 * R2)));

        if (analogRead(A1) > lowThreshold24V) {
            powerIsGood = true;
            if (DEBUG_LEVEL >0 )  Serial.println("+24V power monitor reports good.");
            return true;
        }else{
            powerIsGood = false;
            if (DEBUG_LEVEL >0 ) Serial.println("+24V power monitor reports bad.");
            return false;
        }
    }

  float CogTask::computeHeaterDutyCycleFromWattage(float heaterWattage_w) {
    return (heaterWattage_w < 0.0) ?
      0.0 :
      min(1.0,heaterWattage_w/getConfig()->HEATER_MAXIMUM_WATTAGE);
  }
  void CogTask::runOneButtonAlgorithm() {
      float totalWattage_w;
      float stackWattage_w;
      float heaterWattage_w;
      float fanSpeed_p;

      oneButtonAlgorithm(totalWattage_w,stackWattage_w,heaterWattage_w,fanSpeed_p);

      getConfig()->report->total_wattage_W = totalWattage_w;

      getConfig()->CURRENT_TOTAL_WATTAGE_W = totalWattage_w;

      getConfig()->CURRENT_STACK_WATTAGE_W = stackWattage_w;
      _updateStackWattage(stackWattage_w);
      _updateFanSpeed(fanSpeed_p);
      getConfig()->CURRENT_HEATER_WATTAGE_W = heaterWattage_w;

      float dc = computeHeaterDutyCycleFromWattage(heaterWattage_w);
      dutyCycleTask->dutyCycle = dc;
      dutyCycleTask->reset_duty_cycle();
  }

  void CogTask::_updateCOGSpecificComponents() {
      bool powerIsGood = updatePowerMonitor();

      float t = getTemperatureReadingA_C();
      float fs = getFanSpeed(t);
      float a = computeAmperage(t);

      if (DEBUG_LEVEL > 0) {
        OxCore::Debug<const char *>("fan speed, amperage\n");
        OxCore::Debug<float>(fs);
        OxCore::Debug<const char *>(" ");
        OxCore::DebugLn<float>(a);
      }

      getHAL()->_updateFanPWM(fs);
      getConfig()->report->fan_pwm = fs;
      _updateStackAmperage(a);
      _updateStackVoltage(getConfig()->MAX_STACK_VOLTAGE);
  }
  MachineState CogTask::_updatePowerComponentsWarmup() {
    MachineState new_ms = Warmup;
    new_ms = StateMachineManager::_updatePowerComponentsWarmup();
    if (new_ms == Warmup) {
      _updateCOGSpecificComponents();
    }
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsCooldown() {
    MachineState new_ms = Cooldown;
    new_ms = StateMachineManager::_updatePowerComponentsCooldown();
    if (new_ms == Cooldown) {
      _updateCOGSpecificComponents();
    }
    return new_ms;
  }

  MachineState CogTask::_updatePowerComponentsIdle() {
    OxCore::Debug<const char *>("IN IDLE FUNCTION ");
    MachineState new_ms = NormalOperation;
    getConfig()->idleOrOperate = Idle;
    _updateStackVoltage(MachineConfig::IDLE_STACK_VOLTAGE);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsCritialFault() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
	logRecorderTask->SetPeriod(MachineConfig::INIT_LOG_RECORDER_SHORT_PERIOD_MS);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsEmergencyShutdown() {
    Serial.println("GOT EMERGENCY SHUTDOWN!");
    MachineState new_ms = OffUserAck;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    // In an emergency shutdown, we do NOT run the fan!
    getHAL()->_updateFanPWM(0.0);
    getConfig()->report->fan_pwm = 0.0;
	logRecorderTask->SetPeriod(MachineConfig::INIT_LOG_RECORDER_SHORT_PERIOD_MS);
    return new_ms;
  }
  MachineState CogTask::_updatePowerComponentsOffUserAck() {
    MachineState new_ms = CriticalFault;
    _updateStackVoltage(MachineConfig::MIN_OPERATING_STACK_VOLTAGE);
    return new_ms;
  }

  // TODO: These would go better in the HAL
  void CogTask::_updateFanSpeed(float percentage) {
    float pwm = percentage / 100.0;
    getHAL()->_updateFanPWM(pwm);
    getConfig()->report->fan_pwm = pwm;
  }
  void CogTask::_updateStackWattage(float wattage) {
    // We will set the amperage based on the resistance that we measure
    // via the formula A = sqrt(W/R)
    float r = getConfig()->report->stack_ohms;
    float w = wattage;
    _updateStackAmperage((r <= 0.0) ? 0.0 : sqrt(w/r));
  }
  void CogTask::_updateStackVoltage(float voltage) {
    for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
      getHAL()->_stacks[i]->updateVoltage(voltage,getConfig());
    }
  }

  void CogTask::_updateStackAmperage(float amperage) {
    if (amperage < 0.0) {
      // This is an internal error which should not occur..
       OxCore::Debug<const char *>("Internal Error, negative amperage");
      return;
    }
    for (int i = 0; i < getHAL()->NUM_STACKS; i++) {
      getHAL()->_stacks[i]->updateAmperage(amperage,getConfig());
    }
  }

  MachineState CogTask::_updatePowerComponentsOperation(IdleOrOperateSubState i_or_o) {
    MachineState new_ms = NormalOperation;
    StateMachineManager::_updatePowerComponentsOperation(i_or_o);
    _updateCOGSpecificComponents();

    return new_ms;
  }
}
