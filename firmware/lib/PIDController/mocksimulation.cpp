/*=====================================================================================
 *       Filename:  mocksimulation.cpp
 *
 *    Description:  
 *
 *        Version:  2.0
 *        Created:  07/29/2021
 *       Revision:  Revised after Version 1.0
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
uint8_t minpressure = 0;
double *presarrdup;
SELECTFUNCTION f;
float x;
float press;
float a;
namespace PIOC_MockSimulation
{

    float MockSim::computemockpressure(SELECTFUNCTION f, float x)
    {
        if (f == XPLUSLOGARITHMX)
            press = (log(1.1022 * x) + 0.0027 * x);

        if (f == XPLUSEXPONENTX)
            press = 3.7043 * (pow(1.0003, x)); //We can keep it probably towards the end.
        return press;
    };

    float MockSim::pressurechangerandom(SELECTFUNCTION f, float x)
    {
        if (valveArray[1].start < valveArray[0].stop || valveArray[1].stop > valveArray[0].start)
        {
            //if (valveArray[1].start < valveArray[0].stop)
            //x = valveArray[0].stop - valveArray[1].start;
            //if (valve[1].stop > valve[0].start)
            //x = valve[1].stop - valve[0].start;
        }
        if (f == XPLUSLOGARITHMX)
        {
            press = 0.0044 * x - log(x * 0.00052642);
        }
        else
        {
            press = 3.6239 * (pow(0.7248, (-0.001 * x)));
        }
        //pres = 0.0052*x*pow(0.9642,(0.001*x))
        return press;
    }

    double *MockSim::mockpressuresim(SELECTFUNCTION f, int on[], int end[])
    {
        double presarr[valveArray[1].stop] = {};
        int i = 0;
        float value;
        int arr = 0;
        int a;
        while (i <= 1)
        {
            int end = int(valveArray[i].stop);
            int on = int(valveArray[i].start);
            int x = 0;
            while (x <= (end - on))
            {
                if (i == 0 && valveArray[i].stop > valveArray[i + 1].start)
                    end = valveArray[i + 1].start;
                f == XPLUSLOGARITHMX;
                int val = x / 100;
                value = val;

                if (((val % 2) != 0) && x > 1000)
                {
                    press = pressurechangerandom(f, x);
                }
                else
                {
                    press = computemockpressure(f, x);
                }
                presarr[arr] = press;
                //cout<<presarr[arr]<<endl;
                arr = arr + 1;

                x = x + 1;
            }
            i = i +1;
        }
        presarrdup = presarr;
        return presarrdup;
    }
}