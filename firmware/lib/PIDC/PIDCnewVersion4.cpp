/*=====================================================================================
 *       Filename:  PIDController.cpp
 *
 *    Description:  
 *
 *        Version:  3.0
 *        Created:  07/09/2021
 *       Revision:  Revised after Version 2.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#include <stdio.h>
#include <controller.h>
#include <mpr_pressure.h>
#include <timer.h>
#include <cmath>
#include <config.h>
#include <valve.h>
#include <PIDCnewVersion4.h>
#include <mocksimulation.h>

#define NUM_VALVES 4 //Referenced from config.h
using namespace PIOC_MockSimulation;
using namespace PIDController;
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;

namespace PIDController
{
    SensorState *st;
    PIOCState *pstate;
    ValveStatus *vs;
    ValveState *vt;
    PIOCState *pstate;
    int pres = st->pressure;
    int minpres = st->minPSI;
    int stat = st->status;
    int mod = pstate->mode;
    int maxpres = st->maxPSI;
    int onT = vt->onTime;
    int offT = vt->offTime;
    bool isOn = vt->isOn;
    int msL = vt->msLast;
    int pin = vt->pin;
    float sum = 0;
    float integral = 0;
    float deriv = 0;
    MockSim m;
    float error[] = {1.1, 2.3};    //PIOC_MockSimulation::MockSim::mockpressuresim(SELECTFUNCTION f, PIDController::PIDControl p, float on[], float end[]); //Initialized as dupe array
    float pressure[] = {1.4, 5.6}; //Initialized as dupe array
    PIOC_Controller::Valve *valve = valveArray;
    ControlGains c;
    PIDControl p;
    PIOC_MockSimulation::SELECTFUNCTION f;
    float on[] = {1, 2};
    float end[] = {2.2, 3.4};
    void checkifSystemisOn(SensorState *st, PIOCState *pstate)
    {
        if (stat == 0 && mod == RUNNING)
            printf("PID Controller can be initialized");
        return;
    }
    void initGains(float a, float b, float y)
    {
        c.kp = a;
        c.ki = b;
        c.kd = y;
        return;
    }

    void startingGains(float kp, float ki, float kd, ValveState vt)
    {
        if (vt.status == 0)
        {
            initGains(0.5, 0.5, 0.5);
        }
        return;
    }
    void InitialControlGainsSensor(SensorState *st, PIOCState *pstate)
    {
        if (stat == 0 && (pres >= (1.17 * minpres)) && mod == RUNNING) //If the desired pressure is too high and the current pressure is well below the desired pressure.
        {
            initGains(0.65, 0.56, 0.34); // kp is increased to reach the desired pressure quickly. ki and kd are still low.
        }
        else if (stat == 0 && (pres <= 0.85 * maxpres) && mod == RUNNING) // If the desired pressure is high and the current pressure is close to the desired pressure.
        {
            initGains(0.54, 0.63, 0.57); // kp is reduced since we are close to reaching the desired pressure. ki and kd are increased since the pressure is already near the desired pressure at the starting phase.
        }
        //If the desired pressure is too low and the current pressure limits is well above the desired pressure.
        else if (stat == 0 && (pres <= (1.27 * minpres)) && mod == RUNNING)
        {
            initGains(0.59, 0.367, 0.623); //kp is slightly increased since we need to reach the desired pressure quickly. ki and kd are slightly increased too to reduce oscillations when we are nearing the desired pressure.
        }
        return;
    }
    void InitialControlGainsValve(ValveStatus *vs, ValveState *vt, PIOCState *pstate, SensorState *st)
    {
        if (vs == 0 && mod == STARTING)
        { //Checks for error at the start.
            if ((onT < 0.4 * offT))
            {
                initGains(0.56, 0.45, 0.43);
                //Integral gain is slightly raised to compensate for the state error.
            }
            else
            {
                initGains(0.55, 0.65, 0.63);
                //Integral gain is slightly raised to compensate for the state error.
            }
        }
        return;
    }
    //Need to check about tackling MISSED and ERROR
    //Need to shiftOutValves to alter the timing of the oxygen concentrator.
    void multiplyGains(float x, float y, float z)
    {
        c.kp = c.kp * x;
        c.ki = c.ki * y;
        c.kd = c.kd * z;
        return;
    }
    float computeSum(int i, float error[])
    {
        uint16_t t = 1; // Checks for every 1 millisecond.
        if (i > 0)
        {
            deriv = (error[i] - error[i - 1]) / t; //derivative controller
            integral = integral + (error[i] * t);
            sum = sum + (c.kp * error[i] + c.kd * deriv + c.ki * integral);
        }
        else
        {
            deriv = 0;
            error[i] = 0;
            integral = error[i] * t;
            sum = sum + (c.kd * deriv + c.ki * integral + c.kp * error[i]);
        }
        return sum;
    }
    void changeTiming(int i, float a)
    {
        valve[i].start = a * valve[i].start + valve[i].start;
        valve[i + 2].start = a * valve[i].stop + valve[i].stop;
        return;
    }
    void immediateChange(int j, PIOC_Controller::Valve *valve)
    {
        valve[j].stop = valve[j].stop + 700; //Making sure the stop time of the current valve is increased and the start time of the valve at the outlet is decreased to increase oxygen flow.
        valve[j + 2].start = valve[j + 2].start - 700;
        if (((error[j] - error[j - 1]) > 0.06))
        {
            valve[j].state = false; // Closing the current valve and opening the next valve to make sure that the O2 goes through the sink and not through the outlet.
            valve[j].stop = valve[j].stop - 700;
        }
        return;
    }
    float ControllerComp(SensorState *st, ValveState *vt)
    {
        uint64_t ms = PIOC_Timer::timeSinceEpochMs();
        for (int j = 0; j++; j <= 3)
        {
            if (valve[j].state == 1)
            { //checking state of valve
                if (valve[j].start <= ms < valve[j].stop)
                {
                    int modulation = pres;
                    for (int k = 0; k++; k <= onT)
                    {
                        if ((pin == valve[j].pin) && ((ms - valve[j].start) <= (valve[j].stop - ms)))
                        {
                            if (k > 0)
                            {
                                pressure[k] = pres;
                                //PIDControl *v ;
                                float duppres = m.mockpressuresim(f, v, on, end);
                                error[k] = pressure[k] - duppres; //Checks for error at the start of OnTime in all the valves.
                                if (error[k] > 0.65 * error[k - 1])
                                {
                                    multiplyGains(0.93, 0.7, 1.04); //Adjusts the gains if the error at the next time step is higher than the error at the prev time step. // kd is increased to avoid overshoot. kp remains the same more or less. // ki is slightly decreased to avoid overshoot and reduce cumulative error.
                                }
                                else if (error[k] > 0.057)
                                {                                  //TODO: See what this error is after testing and alter the set value.
                                    multiplyGains(1.0, 1.21, 1.0); //Reduce the steady state error as much as possible since we are reaching the end of OnTime.
                                }
                                else
                                {
                                    multiplyGains(1.0, 1.21, 0.94); //If the error at the next time step is smaller than or equal to error at the previous time step, kd is decreased to avoid unnecessary error accumulations. // ki remains the same.
                                }
                                //Computing Controller
                                int sum = computeSum(k, error);
                            }
                            else
                            {
                                uint16_t t = 1;
                                pressure[k] = pres;
                                int sum = computeSum(k, error); //Computing Controller
                            }
                        }
                        if ((pin == valve[j].pin) && ((ms - valve[j].start) > (valve[j].stop - ms)))
                        {

                            if (error[k] > error[k - 1])
                            {                                   //Increase the gains and make the error as small as possible as we are reaching the end of OnTime.
                                multiplyGains(1.1, 1.08, 1.05); //kp is increased if the error at the next time step is higher than the error at the prev time step.// kd is increased to avoid overshoot.   // ki is slightly increased to avoid overshoot and reduce cumulative error.
                            }
                            else
                            {
                                multiplyGains(1.0, 1.13, 0.54); // kd is reduced to make sure there is no overshoot and the error stays close to 0.// if error is lesser than that of previous state. We are increasing ki again when compared to the previous case to reduce oscillations.
                            }
                        }
                    }
                }
            }
            if (valve[j].state == 0)
            {
                for (int i = 0; i++; i <= offT)
                {
                    if (i > 0)
                    {
                        int t = 1;
                        pressure[i] = pres;
                        error[i] = pressure[i] - pressure[i - 1];
                        if (((error[i] - error[i - 1]) > 0.06) && (offT - i) <= 1500)
                        /*
                            If there is a significant error during the last few time states, we have to check if the pressure 
				            at the pressure sensor is above/below the desired pressure. 
				            We have to close/open the subsequent valves accordingly.
                            */
                        { //We are checking the last one second and we are checking if there is an error. We have to check if the pressure at the pressure sensor is above/below the desired pressure. We have to close/open the subsequent valves accordingly.
                            if (j == 0)
                            {
                                immediateChange(j, valve);
                                valve[j + 1].state = true;
                                valve[j + 1].start = valve[j + 1].start - 700;
                                multiplyGains(1.17, 1.19, 1.0);
                            }
                            else if (j == 1)
                            {
                                immediateChange(j, valve);
                                valve[j - 1].state = true;
                                valve[j - 1].stop = valve[j - 1].stop + 700;
                                multiplyGains(1.17, 1.19, 1.0);
                            }
                        }
                        else if (((error[i] - error[i - 1]) < -0.06) && (offT - i) <= 1500)
                        {
                            /*If the error accumulation decreases, we are making sure that the pressure rate increases 
				              by increasing the onTime duration of valves (1,3) and decreasing the 
				              onTime duration of valves (2,4) or vice versa*/

                            //We are checking the last one second and we are checking if there is an error. We have to check if the pressure at the pressure sensor is above/below the desired pressure. We have to close/open the subsequent valves accordingly.
                            //Change valve Timing
                            if (j == 0)
                            {
                                if (valve[j].state == 1)
                                {
                                    valve[j + 1].state = 0; //Close the immediate valve to make sure O2 does not go to sink.
                                    immediateChange(j, valve);
                                    multiplyGains(1.17, 0.85, 1.12);
                                }
                            }
                            if (j == 1)
                            {
                                if (valve[j].state == 1)
                                {
                                    valve[j - 1].state = 0; //Close the immediate valve to make sure O2 does not go to sink.
                                    immediateChange(j, valve);
                                    multiplyGains(1.17, 0.85, 1.12);
                                }
                            }
                        }
                        if (error[i] >= 1.3 * error[i - 1])
                        {
                            multiplyGains(1.15, 1.19, 1.2); // TODO: Check how the alternate set of valve states can be changed for each change in error state.
                        }
                        sum = computeSum(i, error);
                    }
                    else
                    {
                        pressure[i] = pres;
                        error[i] = 0;
                        int sum = computeSum(i, error);
                    }
                }
            }
        }
        return sum;
    }
    void ImplementController(SensorState *st, ValveState *vt, ValveStatus *vs, PIOCState *pstate)
    {
        uint16_t ms = PIOC_Timer::timeSinceEpochMs();
        InitialControlGainsSensor(st, pstate);
        InitialControlGainsValve(vs, vt, pstate, st);
        float con = ControllerComp(st, vt);
        for (int i = 0; i++; i <= ms)
        {
            for (int j = 0; j++; j <= 3)
            {
                if (valve[j].start <= ms <= valve[j].stop)
                    if (valve[j].state == 1 && (i == 0 || i == 1))
                    {
                        changeTiming(i, con);
                    }
            }
        }
        return;
    };
}
