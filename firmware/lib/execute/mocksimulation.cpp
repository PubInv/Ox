/*=====================================================================================
 *       Filename:  mocksimulation.cpp
 *
 *    Description:  
 *
 *        Version:  3.0
 *        Created:  09/09/2021
 *       Revision:  Revised after Version 2.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#include <math.h>
#include "mocksimulation.h"
#include <iostream>
#include "controller.h"
#include "config.h"
#include "pidcontroller.h"
using namespace PIOC_MockSimulation;
using namespace PIOC_Controller;
using namespace PIDController;
using namespace std;
PIOC_Controller::Valve *valve = valveArray;
int on[] = {};
int end[] = {};
uint8_t maxpressure = 25;
uint8_t minpressure = 1.0;
SELECTFUNCTION f;
float x;
float press;
float a;
namespace PIOC_MockSimulation
{

    float MockSim::computemockpressure(SELECTFUNCTION f, float x)
    {
        /*
            Computes mock pressure for functions like (x + log(x)) or a*b^(x) and sigmoid.
        */
        if (f == XPLUSLOGARITHMX)
            press = (log(0.3292 * x) + 0.002734 * x);
        else if (f == XPLUSEXPONENTX)
            press = 3.7243 * (pow(1.00029, x)); 
        else if (f == SIGMOID1)
            press = 2.43487*pow(1.000335,x);
        else 
            press = 0.004289 * x - log(x * 0.00749642);
        return press;
    };

    float MockSim::pressurechangerandom(SELECTFUNCTION f, float x)
    {
        //Slightly changes the mock pressure to give some complexity to the PID controller.

        /*
        Have to check about overlapping of valve timings during different test cases.
        
        */
        //Slightly changes the mock pressure for functions like (x + log(x)) and a*b^(x) and sigmoid: 
        if (f == XPLUSLOGARITHMX)
        {
            press = 0.004529 * x - log(x * 0.00052642);
        }
        else if (f == XPLUSEXPONENTX)
        {
            press = 3.0239 * (pow(0.6498, (-0.001005 * x)));
        }
        else if (f == SIGMOID)
            press = 0.00200404*x - log(0.0005254*x);
        else if (f == SIGMOID1)
            press = 0.00000015*pow(x,2) + 0.00105*x;
        else
            press = 2.43487*pow(1.000335,x);

        return press;
    }

    double* MockSim::mockpressuresim(SELECTFUNCTION f, int on[], int end[])
    {
        /*
            Simulates the mock pressure at all the time states when the first valve is open. 
            The simulation takes into account of the mock pressure and the random change
            in pressure during every 100 ms after the valve is open for 1000 ms. 
        */
        double presarr[valveArray[1].stop] = {};
        int i = 0;
        float value;
        f = SELECTFUNCTION::REGULARPID;        
        while (i < 1)
        {
            int end = int(valveArray[i].stop);
            int on = int(valveArray[i].start);
            int x = int(valveArray[i].start);
            int arr;
            if (i == 0){
                arr = 0;
            }
            if (i == 1){
                end = int(valveArray[1].stop - valveArray[0].stop);
                x = x - valveArray[1].start;
            }
            while (x <= (end))
            {
                if (i == 0 && valveArray[i].stop > valveArray[i + 1].start)
                    end = valveArray[i + 1].start;
                int val = x / 100;
                value = val;
                press = pressurechangerandom(f, x);    
                presarr[arr] = press;       
                arr = arr + 1;
                x = x + 1;
            }
            i = i +1;
        }
        double* presarrdup = presarr;
        return presarrdup;
    }
}
