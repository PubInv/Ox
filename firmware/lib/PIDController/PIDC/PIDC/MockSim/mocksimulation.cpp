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
#include <windows.h>
#include "controller.h"
#include "config.h"

using namespace PIOC_Controller;
using namespace std;
using namespace PIOC_MockSimulation;

namespace PIOC_MockSimulation{
    float on[] = {1, 2};
    float end[] = {2.2, 3.4};
    uint8_t maxpressure = 25;
    uint8_t minpressure = 0;
    
    SELECTFUNCTION f;
    float x;
    float pres;
    float a;
    float MockSim::computemockpressure(SELECTFUNCTION f, float x)
    {
        if (f == XPLUSLOGARITHMX)
            pres = (log(1.1022 * x) + 0.0027 * x);

        if (f == XPLUSEXPONENTX)
            pres = 3.7043 * (pow(1.0003, x)); //We can keep it probably towards the end.
        return pres;
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
            pres = 0.0044 * x - log(x * 0.00052642);
        else if (f == XPLUSEXPONENTX)
            pres = 3.6239 * (pow(0.7248, (-0.001 * x)));
        //pres = 0.0052*x*pow(0.9642,(0.001*x))
        return pres;
    }

    
    float* MockSim::mockpressuresim(SELECTFUNCTION f, float on[], float end[])
    {
        float presarr[valveArray[1].stop] = {};
        int i = 0;
        float value;
        while (i <= 1)
        {
            end[i] = valveArray[i].stop;
            on[i] = valveArray[i].start;
            x = 0;
            int arr = valveArray[i].start;
            
            while (x <= (end[i] - on[i]))
            {
                if (i == 0 && valveArray[i].stop > valveArray[i + 1].start)
                    end[i] = valveArray[i + 1].start;
                f == XPLUSLOGARITHMX;
                int val = x / 100;
                value = val;
                
                if (((val % 2) != 0) && x > 1000)
                {
                    pres = pressurechangerandom(f, x);
                }
                else
                {
                    pres = computemockpressure(f, x);
                }
                //cout<<"\n"<<pres;
                presarr[arr] = pres;
                //cout<<"\n"<<presarr[arr];
                
                arr = arr +1;
                x = x+ 1;
            }
            i += 1;
        }
        float *presarrdup = presarr;
        return presarrdup;
    };
}



