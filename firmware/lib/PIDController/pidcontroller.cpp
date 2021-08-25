/*=====================================================================================
 *       Filename:  PIDController.cpp
 *
 *    Description:  
 *
 *        Version:  4.0
 *        Created:  07/26/2021
 *       Revision:  Revised after Version 3.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#include <iostream>
#include <cmath>
#include "pidcontroller.h"
#include "mocksimulation.h"
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;
using namespace std;
using namespace PIOC_MockSimulation;
namespace PIDController
{
    SensorStatus st;
    ValveStatus vs = ValveStatus::OK;
    ValveState vt;
    PIOCMode mod = PIOCMode::STARTING;
    float pres;
    float endpres;
    int minpres = 1;
    int stat = 1;
    int maxpres = 25;
    int onT[] = {int(valveArray[0].stop - valveArray[0].start), int(valveArray[1].stop - valveArray[1].start)};
    int offT[] = {int(8000 - (valveArray[0].stop - valveArray[0].start)), int(8000 - (valveArray[1].stop - valveArray[1].start))};
    bool isOn = 1;
    float integral;
    float deriv;
    float prop;
    PIOC_MockSimulation::MockSim m;
    float error[] = {};
    float pressure[8000] = {};
    double *err;
    float sum = 0;
    ControlGains c;

    void PIDControl::checkifSystemisOn(SensorStatus st, PIOCMode mod)
    {
        /*
        Checks if the PID controller can be initialized by taking into account of the Senssor Status 
        and the controller mode
        */
        if (st == SensorStatus::OK && mod == PIOCMode::RUNNING)
            printf("PID Controller can be initialized");

        return;
    }
    void PIDControl::initGains(float a, float b, float y)
    {
        /*
        Initializes the gains of the PID controller
        */
        c.kp = a;
        c.ki = b;
        c.kd = y;
        return;
    }

    void PIDControl::startingGains(float kp, float ki, float kd, ValveStatus vs)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the valve status. Checks if the valve status is 'OK' and initializes 
            the controller. 
        */
        if (vs == ValveStatus::OK)
        {
            initGains(0.75, 0.87, 0.56);
        }
        return;
    }
    void PIDControl::InitialControlGainsSensor(SensorStatus st, PIOCMode mod)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the state of the sensor and the condition (OK, RUNNING) of the system. 
        */
        if ((st == SensorStatus::OK) && (endpres >= (1.17 * minpres)) && (mod == PIOCMode::STARTING))
        //If the desired pressure is too high and the current pressure is well below the desired pressure.
        {
            // kp is increased to reach the desired pressure quickly. ki and kd are still low.
            initGains(1.005, 0.76, 0.65);
        }
        else if (st == SensorStatus::OK && (endpres <= 0.85 * maxpres) && mod == PIOCMode::STARTING)
        // If the desired pressure is high and the current pressure is close to the desired pressure.
        {
            /* kp is reduced since we are close to reaching the desired pressure. 
              ki and kd are increased since the pressure is already near the desired pressure at the starting phase.
            */
            initGains(0.54, 1.23, 1.34);
        }

        else if (st == SensorStatus::OK && (endpres <= (1.27 * minpres)) && mod == PIOCMode::STARTING)
        //If the desired pressure is too low and the current pressure limits is well above the desired pressure.
        {
            /*
            kp is slightly increased since we need to reach the desired pressure quickly. 
            ki and kd are slightly increased too to reduce oscillations when we are nearing the desired pressure.
            */
            initGains(1.059, 1.0465, 1.0623);
        }
        return;
    }
    void PIDControl::InitialControlGainsValve(ValveStatus vs, PIOCMode mod, SensorStatus st)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the state of the valve and the condition (OK, RUNNING) of the system. 
        */
        if (vs == ValveStatus::OK && mod == PIOCMode::STARTING)
        {
            //Checks for error at the start.
            // I need to check the different test cases.
            if ((onT[0] < 0.4 * offT[0]))
            {
                initGains(0.56, 0.45, 0.043);
            }
            else if ((onT[1] < 0.4 * offT[1]))
            {
                initGains(0.56, 0.45, 0.043);
            }
            else
            {
                initGains(0.55, 0.65, 0.063);
            }
        }
        return;
    }
    //Need to shiftOutValves to alter the timing of the oxygen concentrator.
    void PIDControl::multiplyGains(float x, float y, float z)
    {
        /*
            Adjusts the gains of the controller with respect to the error at
            each state.
        */
        if (x >= 1)
            c.kp = c.kp + (0.0001 * x);
        if (x < 1)
            c.kp = c.kp - (0.0001 * x);
        if (y >= 1)
            c.ki = c.ki + (0.0001 * y);
        if (y < 1)
            c.ki = c.ki - (0.0001 * y);
        if (z >= 1)
            c.kd = c.kd + (0.0001 *z);
        if (z < 1)
            c.kd = c.kd - (0.0001 *z); 
        return;
    }
    float PIDControl::computeSum(int i, double *err)
    {
        /*
            Computes the PID controller by including the integral , derivative
            and proportional part of the error. Sums up the value at each 
            state.
        */
        int t = (1.5); // Checks for every 1 millisecond.
        if (i > 0)
        {
            if (*(err + i) > 0)
            {
                prop = -(*(err + i));                         //proportional error
                deriv = -(*(err + i) - *(err + (i - 1))) / t; //derivative error
                integral = -(*(err + i) * t);                 //integral error
                sum = (c.kp * prop + c.kd * deriv + c.ki * integral);
            }

            else
            {
                prop = (*(err + i));                         //proportional error
                deriv = (*(err + i) - *(err + (i - 1))) / t; //derivative error
                integral = (*(err + i) * t);                 //integral error
                sum = (c.kp * prop + c.kd * deriv + c.ki * integral);
            }
        }
        else
        {
            prop = (*(err + i));         //proportional error
            deriv = 0;                   //derivative error
            integral = (*(err + i)) * t; //integral error
            sum = (c.kd * deriv + c.ki * integral + c.kp * (*(err + i)));
        }
        return sum;
    }
    void PIDControl::changeTiming(int i, float a)
    {
        /*
            Changes the timing of the PID controller slightly by taking 
            the effect of the PID controller on the error in pressure.
        */
        valveArray[i].start = a * valveArray[i].start + valveArray[i].start;
        valveArray[i + 2].start = a * valveArray[i].stop + valveArray[i].stop;
        return;
    }
    void PIDControl::immediateChange(int j, PIOC_Controller::Valve *valve)
    {
        /*
            Changes the timing of the PID controller slightly by taking into account 
            the error in pressure reading during the valve off state.
        */
        valveArray[j].stop = valveArray[j].stop + 700; //Making sure the stop time of the current valve is increased and the start time of the valve at the outlet is decreased to increase oxygen flow.
        valveArray[j + 2].start = valveArray[j + 2].start - 700;
        if (((error[j] - error[j - 1]) > 0.06))
        {
            valveArray[j].state = false; // Closing the current valve and opening the next valve to make sure that the O2 goes through the sink and not through the outlet.
            valveArray[j].stop = valveArray[j].stop - 700;
        }
        return;
    }
    float PIDControl::ControllerComp(double *err)
    {
        /*
         Computes the controller by taking into account, the error in pressure during different time states. 
         Adjusts the gains of the controller accordingly during each time state. 
        */
        float pressuredup = 1.2;
        int on[] = {3, 4};
        int end[] = {1, 3};
        float mockpressure[] = {};
        int j = 0;
        int arr = 0;
        while (j <= 1)
        {
            int k = int(valveArray[j].start);
            while (k <= int(valveArray[j].stop))
            {
                error[arr] = *(err + arr) + sum ;
                if ((k - valveArray[j].start) <= (valveArray[j].stop - k))
                {
                    //Checks for error at the start of OnTime in all the valves.
                    
                    if (error[arr] > 0.5 * error[arr - 1])
                    {
                        //Adjusts the gains if the error at the next time step is higher than the error at the prev time step.
                        // kd is increased to avoid overshoot. kp remains the same more or less.
                        multiplyGains(2.82349, 2.0523, 1.6401);
                    }
                    if (error[arr] > 0.587)
                    //TODO: See what this error is after testing and alter the set value.
                    {
                        multiplyGains(1.345, 0.991, 2.234);
                    }
                    if (error[arr] <= error[arr - 1])
                    {
                        //If the error at the next time step is smaller than or equal to error at the previous time step,
                        // kd is decreased to avoid unnecessary error accumulations.
                        // ki remains the same.
                        multiplyGains(0.234, 0.875, 0.9974);
                    }
                }
                if ((k - valveArray[j].start) > (valveArray[j].stop - k))
                {
                    if (error[arr] > error[arr - 1])

                    //Increase the gains and make the error as small as possible as we are reaching the end of OnTime.
                    {
                        //kp is increased if the error at the next time step is higher than the error at the prev time step.
                        // kd is increased to avoid overshoot.
                        // ki is slightly increased to avoid overshoot and reduce cumulative error.
                        multiplyGains(2.678, 1.56, 0.7251);
                    }
                    else if (error[arr] > 1)
                    {
                        // kd is reduced to make sure there is no overshoot and the error stays close to 0.
                        // if error is lesser than that of previous state.
                        //We are increasing ki again when compared to the previous case to reduce oscillations.
                        multiplyGains(1.8456, 1.76211, 0.974);
                    }
                    else
                    {
                        // kd is reduced to make sure there is no overshoot and the error stays close to 0.
                        // if error is lesser than that of previous state. We are increasing ki again when compared
                        // to the previous case to reduce oscillations.
                        multiplyGains(2.678, 2.06211, 0.879);
                    }
                }
                //Computing Controller
                sum = computeSum(arr, err);
                k = k + 1;
                std::cout<< "Error: " << error[arr] << endl;
                arr = arr + 1;
            }

            /*if (valveArray[j].state == 0)
            {
                for (int i = 0; i = i +1; i <= offT[0])
                {
                    if (i > 0)
                    {
                        int t = 1;
                        pressure[i] = pres;
                        error[i] = pressure[i] - pressure[i - 1];
                        if (((error[i] - error[i - 1]) > 0.06) && (offT[0] - i) <= 1500)

                        //If there is a significant error during the last few time states, we have to check if the pressure
                        //at the pressure sensor is above/below the desired pressure.
                        //We have to close/open the subsequent valves accordingly.

                        { //We are checking the last one second and we are checking if there is an error. We have to check if the pressure at the pressure sensor is above/below the desired pressure. We have to close/open the subsequent valves accordingly.
                            if (j == 0)
                            {
                                immediateChange(j, valveArray);
                                valveArray[j + 1].state = true;
                                valveArray[j + 1].start = valveArray[j + 1].start - 700;
                                multiplyGains(1.17, 1.19, 1.0);
                            }
                            if (j == 1)
                            {
                                immediateChange(j, valveArray);
                                valveArray[j - 1].state = true;
                                valveArray[j - 1].stop = valveArray[j - 1].stop + 700;
                                multiplyGains(1.17, 1.19, 1.0);
                            }
                        }
                        else if (((error[i] - error[i - 1]) < -0.06) && (offT[0] - i) <= 1500)
                        {
                            //If the error accumulation decreases, we are making sure that the pressure rate increases
                            //by increasing the onTime duration of valves (1,3) and decreasing the
                            //onTime duration of valves (2,4) or vice versa

                            //We are checking the last one second and we are checking if there is an error. We have to check if the pressure at the pressure sensor is above/below the desired pressure. We have to close/open the subsequent valves accordingly.
                            //Change valve Timing
                            if (j == 0)
                            {
                                if (valveArray[j].state == 1)
                                {
                                    valveArray[j + 1].state = 0; //Close the immediate valve to make sure O2 does not go to sink.
                                    immediateChange(j, valveArray);
                                    multiplyGains(1.17, 0.85, 1.12);
                                }
                            }
                            if (j == 1)
                            {
                                if (valveArray[j].state == 1)
                                {
                                    valveArray[j - 1].state = 0; //Close the immediate valve to make sure O2 does not go to sink.
                                    immediateChange(j, valveArray);
                                    multiplyGains(1.17, 0.85, 1.12);
                                }
                            }
                        }
                        if (error[i] >= 1.12 * error[i - 1])
                        {
                            multiplyGains(1.10, 1.12, 1.18); // TODO: Check how the alternate set of valve states can be changed for each change in error state.
                        }
                        sum = computeSum(i, error);
                    }
                    else
                    {
                        pressure[i] = pres;
                        error[i] = 0;
                        sum = computeSum(i, error);
                    }
                }
            }*/
            j = j + 1;
        }
        return sum;
    }

    //Desired pressure for Mock (XPLUSLOGARITHMX): y = 0.0035008*x + 1.996499
    //Desired pressure for Mock (XPLUSEXPONENTX): y = 0.0017504*x + 4.498249
    void PIDControl::ImplementController(SensorStatus st, ValveStatus vs, PIOCMode mod, double *err)
    {
        /*
            Implements the controller based on the Sensor state, Valve State, the max/min pressure
            the desired pressure reading and the current pressure reading.
        */
        uint16_t ms = 1500;
        InitialControlGainsSensor(st, mod);
        InitialControlGainsValve(vs, mod, st);
        float con = ControllerComp(err);
        //cout<<"Output"<<con<<endl;
        /*float l;
        for (int i = 0; i++; i <= ms)
        {
            for (int j = 0; j++; j <= 3)
            {
                if (valveArray[j].start <= i <= valveArray[j].stop)
                    if (valveArray[j].state == 1 && (i == 0 || i == 1))
                    {
                        changeTiming(i, l);
                    }
            }
        }*/
        return;
    };
}