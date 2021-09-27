/*=====================================================================================
 *       Filename:  PIDController.cpp
 *
 *    Description:  
 *
 *        Version:  5.0
 *        Created:  09/09/2021
 *       Revision:  Revised after Version 4.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#include <iostream>
#include <cmath>
#include <fstream>
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
    int onT[2] = {int(valveArray[0].stop - valveArray[0].start), int(valveArray[1].stop - valveArray[1].start)};
    int offT[2] = {int(8000 - (valveArray[0].stop - valveArray[0].start)), int(8000 - (valveArray[1].stop - valveArray[1].start))};
    float integral = 0;
    float deriv;
    float prop;
    PIOC_MockSimulation::MockSim m;
    double error[8000] = {};
    float pressure[8000] = {};
    double *err;
    float sum = 0;
    ControlGains c;
    double g[2] = {0, 0};
    double Kpchange, Kichange, Kdchange;
    double initKp, initKi, initKd;

    void PIDControl::startingGains(ValveStatus vs)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the valve status. Checks if the valve status is 'OK' and initializes 
            the controller. 
        */
        if (vs == ValveStatus::OK)
        {
            initKp = 1.1;
            initKi = 0.5;
            initKd = 0.5;
            initGains(initKp, initKi, initKd);
        }
        return;
    }

    void PIDControl::checkifSystemisOn(SensorStatus st, PIOCMode mod)
    {
        /*
        Checks if the PID controller can be initialized by taking into account of the Senssor Status 
        and the controller mode
        */
        if (st == SensorStatus::OK && mod == PIOCMode::RUNNING)
        {
            printf("PID Controller can be initialized");
            startingGains(vs);
        }
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

    void PIDControl::InitialControlGainsSensor(double d, SensorStatus st, PIOCMode mod)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the state of the sensor and the condition (OK, RUNNING) of the system. 
        */
        if ((st == SensorStatus::OK) && (d >= (1.17 * minpres)) && (mod == PIOCMode::STARTING))
        //If the desired pressure is too high and the current pressure is well below the desired pressure.
        {
            initKp = 1.5;
            initKi = 1.5;
            initKd = 1.5;
            initGains(initKp, initKi, initKd);
        }
        else if (st == SensorStatus::OK && (d <= 0.85 * maxpres) && mod == PIOCMode::STARTING)
        // If the desired pressure is high and the current pressure is close to the desired pressure.
        {
            /* kp is changed since we are close to reaching the desired pressure. 
              ki and kd are changed since the pressure is already near the desired pressure at the starting phase.
            */
            initKp = 0.54;
            initKi = 0.53;
            initKd = 0.54;
            initGains(initKp, initKi, initKd);
        }
        else
        {
            /*
            kp is changed since we need to reach the desired pressure quickly. 
            ki and kd are slightly changed too to reduce oscillations when we are nearing the desired pressure.
            */
            initKp = 0.50;
            initKi = 0.54;
            initKd = 0.56;

            initGains(initKp, initKi, initKd);
        }
        return;
    }
    void PIDControl::InitialControlGainsValve(ValveStatus vs, PIOCMode mod, SensorStatus st)
    {
        /*
            Initializes the P, I and D gains of the controller with respect to 
            the state of the valve and the condition (OK, RUNNING) of the system. 
        */
        initKp = 0.119012;
        initKi = 0.1;
        initKd = 0.1279;
        initGains(initKp, initKi, initKd);
        if (vs == ValveStatus::OK && mod == PIOCMode::STARTING)
        {
            //Checks for error at the start.
            // I need to check the different test cases.

            if ((onT[0] < 0.4 * offT[0]))
            {
                /*Checks if the onTime of the first valve is 
                  less than 40 percent of the offTime of the first valve. 
                  */
                initKp = 0.5;
                initKi = 0.5;
                initKd = 0.5;
                initGains(initKp, initKi, initKd);
            }
            else
            {
                initKp = 0.10;
                initKi = 0.15;
                initKd = 0.16;
                initGains(initKp, initKi, initKd);
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
        if (x >= 1.0)
            c.kp = c.kp + (0.0001 * x);
        if (x < 1.0)
            c.kp = c.kp - (0.0001 * x);
        if (y >= 1.0)
            c.ki = c.ki + (0.0001 * y);
        if (y < 1.0)
            c.ki = c.ki - (0.0001 * y);
        if (z >= 1.0)
            c.kd = c.kd + (0.0001 * z);
        if (z < 1.0)
            c.kd = c.kd - (0.0001 * z);

        return;
    }
    double PIDControl::computeSum(int i, double *err)
    {
        /*
            Computes the PID controller by including the integral , derivative
            and proportional part of the error. Sums up the value at each 
            state.
        */
        int t = (1.0); // Checks for every 1 millisecond.
        if (i > 0)
        {
            if (*(err + i) > 0)
            {
                prop = -(*(err + i));                         //proportional error
                deriv = -(*(err + i) - *(err + (i - 1))) / t; //derivative error
                integral = -(*(err + i) * t);                 //integral error
                sum = (c.kd * deriv + c.kp * prop + c.ki * integral);
            }

            else
            {
                prop = (*(err + i));                         //proportional error
                deriv = (*(err + i) - *(err + (i - 1))) / t; //derivative error
                integral = (*(err + i) * t);                 //integral error
                sum = (c.kd * deriv + c.kp * prop + c.ki * integral);
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
    double *PIDControl::ControllerComp(double *err)
    {
        /*
         Computes the controller by taking into account, the error in pressure during different time states. 
         Adjusts the gains of the controller accordingly during each time state. 
        */
        int on[] = {3, 4}; //Variables initialized by Default
        int end[] = {1, 3};//Variables initialized by Default
        float mockpressure[] = {};
        int j = 0;
        int arr = 0;
        while (j < 1)
        {
            int k = int(valveArray[j].start);
            while (k <= int(valveArray[j].stop))
            {
                error[arr] = *(err + arr) + sum; // Controller takes into account the error
                                                 // at the immediate previous time step
                //Checks for error at the start of OnTime in all the valves.

                if (error[arr] >= error[arr - 1])
                {
                    //Adjusts the gains if the error at the next time step is higher than the error at the prev time step.
                    //Kp, Ki and Kd gains are changed to compensate for the error.
                    Kpchange = 0.91907;
                    Kichange = -0.001994;
                    Kdchange = 0.3;
                    multiplyGains(Kpchange, Kichange, Kdchange); // Changes the P, I and D at different time steps
                                                                 // depending on how big the error is
                }
                if (error[arr] < error[arr - 1])
                {
                    //If the error at the next time step is smaller than or equal to error at the previous time step,
                    //Kp, Ki and Kd gains are changed to compensate for the error.
                    Kpchange = 0.9752;
                    Kichange = -0.01;
                    Kdchange = 0.05;
                    multiplyGains(Kpchange, Kichange, Kdchange);
                }
                //if (error[arr]>= 3.0 || error[arr]<= -3.0)
                    //error[arr] = 0.5 * error[arr];
                //Computing Controller

                /*if (((error[arr] - error[arr - 1]) > 0.06) && (valveArray[j].stop - arr) <= 500)

                //If there is a significant error during the last few time states, we have to check if the pressure
                //at the pressure sensor is above/below the desired pressure.
                //We have to close/open the subsequent valves accordingly.

                { //We are checking the last 500 ms and we are checking if there is an error.

                    /*If the error in pressure is higher, this implies there is a problem with the valve
                    We are switching to the next valve to make the pressure reading close to the 
                    desired pressure. 

                    if (j == 0)
                    {
                        immediateChange(j, valveArray);
                        valveArray[j + 1].state = true;
                        valveArray[j + 1].start = valveArray[j + 1].start - 500;
                        valveArray[j].state = false;
                    }
                    if (j == 1)
                    {
                        immediateChange(j, valveArray);
                        valveArray[j - 1].state = true;
                        valveArray[j - 1].stop = valveArray[j - 1].stop + 500;
                    }
                }*/
                cout<<"Erroref: "<<sum<<endl;
                sum = computeSum(arr, err);
                arr = arr + 1;
                k = k + 1;
                
            }
            j = j + 1;
        }
        double *a = error;
        return a;
    }

    double *PIDControl::ImplementController(double d, SensorStatus st, ValveStatus vs, PIOCMode mod, double *err)
    {
        /*
            Implements the controller based on the Sensor state, Valve State, the max/min pressure
            the desired pressure reading and the current pressure reading.
        */
        InitialControlGainsSensor(d, st, mod); // Sets Initial Gains
        InitialControlGainsValve(vs, mod, st); // Sets Initial Gains
        double *con = ControllerComp(err);     // Runs the PID Controller
        /*for (int i = 100; i++; i <= 4000)
        {
            for (int j = 0; j++; j < 1)
            {
                if (valveArray[j].start <= i <= valveArray[j].stop)
                    if (valveArray[j].state == 1 && (i == 0 || i == 1))
                    {
                        //Need to check about implementing a function
                        // which will optimize the timing of the valves
                        //after considering the error in the pressure reading.
                        changeTiming(i, *(con + i - 100));
                    }
            }
        }*/
        return con;
    };
}
