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
    SensorStatus st ;
    ValveStatus vs = ValveStatus::OK;
    ValveState vt;
    PIOCMode mod = PIOCMode::STARTING;
    float pres;
    float endpres;
    //st->pressure;
    int minpres = 1;
    //st->minPSI;
    int stat = 1;
    //st->status;
    //pstate->mode;
    int maxpres = 25;
    //st->maxPSI;
    int onT[] = {int(valveArray[0].stop - valveArray[0].start), int(valveArray[1].stop - valveArray[1].start)};
    int offT[] = {int(8000 - (valveArray[0].stop - valveArray[0].start)), int(8000 - (valveArray[1].stop - valveArray[1].start))};
    bool isOn = 1;
    //vt->isOn;
    float integral = 0;
    float deriv = 0;
    PIOC_MockSimulation::MockSim m;
    float error[] = {}; //PIOC_MockSimulation::MockSim::mockpressuresim(SELECTFUNCTION f, PIDController::PIDControl p, float on[], float end[]); //Initialized as dupe array
    float pressure[8000] = {};
    double* err;
    float sum = 0;
    ControlGains c;
    
    void PIDControl::checkifSystemisOn(SensorStatus st, PIOCMode mod)
    {
        if (st == SensorStatus::OK && mod == PIOCMode::RUNNING)
            printf("PID Controller can be initialized");

        return;
    }
    void PIDControl::initGains(float a, float b, float y)
    {
        c.kp = a;
        c.ki = b;
        c.kd = y;
        return;
    }

    void PIDControl::startingGains(float kp, float ki, float kd, ValveStatus vs)
    {
        if (vs == ValveStatus::OK)
        {
            initGains(0.75, 0.87, 0.56);
        }
        return;
    }
    void PIDControl::InitialControlGainsSensor(SensorStatus st, PIOCMode mod)
    {
        if ((st == SensorStatus::OK) && (endpres >= (1.17 * minpres)) && (mod == PIOCMode::STARTING)) //If the desired pressure is too high and the current pressure is well below the desired pressure.
        {
            initGains(0.65, 0.76, 0.34); // kp is increased to reach the desired pressure quickly. ki and kd are still low.
        }
         else if (st == SensorStatus::OK && (endpres <= 0.85 * maxpres) && mod == PIOCMode::STARTING) // If the desired pressure is high and the current pressure is close to the desired pressure.
        {
            initGains(0.54, 0.63, 0.57); // kp is reduced since we are close to reaching the desired pressure. ki and kd are increased since the pressure is already near the desired pressure at the starting phase.
        }
        //If the desired pressure is too low and the current pressure limits is well above the desired pressure.
        else if (st == SensorStatus::OK && (endpres <= (1.27 * minpres)) && mod == PIOCMode::STARTING)
        {
            initGains(0.59, 0.465, 0.623); //kp is slightly increased since we need to reach the desired pressure quickly. ki and kd are slightly increased too to reduce oscillations when we are nearing the desired pressure.
        }
        return;
    }
    void PIDControl::InitialControlGainsValve(ValveStatus vs, PIOCMode mod, SensorStatus st)
    {
        if (vs == ValveStatus::OK && mod == PIOCMode::STARTING)
        { //Checks for error at the start.
            if ((onT[0] < 0.4 * offT[0]))
            {
                initGains(0.56, 0.45, 0.43);
                //Integral gain is slightly raised to compensate for the state error.
            }
            else if ((onT[1] < 0.4 * offT[1]))
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
    void PIDControl::multiplyGains(float x, float y, float z)
    {
        if (x >= 1)
        c.kp =  c.kp + (0.000001*x);
        if (x < 1)
        c.kp =  c.kp - (0.000001*x);
        if (y >= 1)
        c.ki =  c.ki + (0.000001*y);
        if (y< 1)
        c.ki =  c.ki - (0.000001* y);
        if (z >= 1)
        c.kd =  c.kd + (0.000001* z);
        if (z < 1)
        c.kd =  c.kd - (0.000001*z);
        
        return;
    }
    float PIDControl::computeSum(int i, double* err)
    {
        int t = 5; // Checks for every 10 milliseconds.
        if (i > 0)
        {
            deriv = (*(err + i) - *(err + (i - 1))) / t; //derivative controller
            integral =  (*(err + i) * t);
            sum =  (c.kp * (*(err +i)) + c.kd * deriv + c.ki * integral);
        }
        else
        {
            deriv = 0;
            integral = (*(err + i)) * t;
            sum = (c.kd * deriv + c.ki * integral + c.kp * (*(err +i)));
        }
        //cout<<"Derivative Gain "<<*(err+i)<<endl;
        return sum;
    }
    void PIDControl::changeTiming(int i, float a)
    {
        valveArray[i].start = a * valveArray[i].start + valveArray[i].start;
        valveArray[i + 2].start = a * valveArray[i].stop + valveArray[i].stop;
        return;
    }
    void PIDControl::immediateChange(int j, PIOC_Controller::Valve *valve)
    {
        valveArray[j].stop = valveArray[j].stop + 700; //Making sure the stop time of the current valve is increased and the start time of the valve at the outlet is decreased to increase oxygen flow.
        valveArray[j + 2].start = valveArray[j + 2].start - 700;
        if (((error[j] - error[j - 1]) > 0.06))
        {
            valveArray[j].state = false; // Closing the current valve and opening the next valve to make sure that the O2 goes through the sink and not through the outlet.
            valveArray[j].stop = valveArray[j].stop - 700;
        }
        //cout << "Hello\n";
        return;
    }
    float PIDControl::ControllerComp(SensorStatus st, ValveStatus vs, double* err)
    {
       
        float pressuredup = 1.2;
        int on[] = {3,4};
        int end[] = {1,3};

        //double* duppres = m.mockpressuresim(f, on, end);
        //cout<<*(duppres + 2)<<endl;
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
                    //error[arr] = pressure[arr] - *(duppres + arr); //Checks for error at the start of OnTime in all the valves.
                    
                    if (error[arr] > 0.9 * error[arr - 1])
                    {
                        multiplyGains(0.9305, 0.77, 1.401); //Adjusts the gains if the error at the next time step is higher than the error at the prev time step. // kd is increased to avoid overshoot. kp remains the same more or less. // ki is slightly decreased to avoid overshoot and reduce cumulative error.
                    }
                    else if (error[arr] > 0.587)
                    {                                   //TODO: See what this error is after testing and alter the set value.
                        multiplyGains(0.555, 0.591, 0.533); //Reduce the steady state error as much as possible since we are reaching the end of OnTime.
                    }
                    else
                    {
                        multiplyGains(0.555, 0.591, 1.44); //If the error at the next time step is smaller than or equal to error at the previous time step, kd is decreased to avoid unnecessary error accumulations. // ki remains the same.
                    }
                }
                if ((k - valveArray[j].start) > (valveArray[j].stop - k))
                {
                    if (error[arr] > error[arr - 1])
                    {                                   //Increase the gains and make the error as small as possible as we are reaching the end of OnTime.
                        multiplyGains(10.878, 10.0767, 10.9); //kp is increased if the error at the next time step is higher than the error at the prev time step.// kd is increased to avoid overshoot.   // ki is slightly increased to avoid overshoot and reduce cumulative error.
                    }
                    else
                    {
                        multiplyGains(0.134, 0.12111, 0.54); // kd is reduced to make sure there is no overshoot and the error stays close to 0.// if error is lesser than that of previous state. We are increasing ki again when compared to the previous case to reduce oscillations.
                    }
                    
                }    
                    //Computing Controller
                sum = computeSum(arr, err);
                k = k + 1;
                
                cout<<"Error: "<<*(err + arr)<<endl;
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

    //Intended pressure for Mock (XPLUSLOGARITHMX): y = 0.0035008*x + 1.996499
    //Intended pressure for Mock (XPLUSEXPONENTX): y = 0.0017504*x + 4.498249
    void PIDControl::ImplementController(SensorStatus st,  ValveStatus vs, PIOCMode mod, double* err)
    {
        uint16_t ms = 1500;
        InitialControlGainsSensor(st, mod);
        InitialControlGainsValve(vs, mod, st);
        float con = ControllerComp(st, vs, err);
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