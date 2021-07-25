#include <math.h>
#include <stdio.h>
#include <PIDCnewVersion3.h>
#include <mocksimulation.h>

using namespace std;
using namespace PIDController;
using namespace PIOC_MockSimulation;

namespace PIOC_MockSimulation
{
    float on[] = {1, 2};
    float end[] = {2.2, 3.4};
    PIDController::PIDControl p;
    uint8_t maxpressure = 25;
    uint8_t minpressure = 0;

    SELECTFUNCTION f;
    float x;
    float pres;
    float a;
    float computemockpressure(SELECTFUNCTION f, float x)
    {
        if (f == XPLUSLOGARITHMX)
            pres = (log(1.1022 * x) + 0.0027 * x);

        if (f == XPLUSEXPONENTX)
            pres = 3.7043 * (pow(1.0003, x)); //We can keep it probably towards the end.
        return pres;
    };

    float pressurechangerandom(SELECTFUNCTION f, float x)
    {
        if (valve[1].start < valve[0].stop || valve[1].stop > valve[0].start)
        {
            if (valve[1].start < valve[0].stop)
                x = valve[0].stop - valve[1].start;
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

    PIOC_Controller::Valve *valve = valveArray;

    float mockpressuresim(SELECTFUNCTION f, PIDController::PIDControl p, float on[], float end[])
    {
        int i = 0;
        while (i <= 1)
        {
            x = 0;
            while (x <= valve[i].stop)
            {
                if (i == 0 && valve[i].stop > valve[i + 1].start)
                    end[i] = valve[i + 1].start;
                else if (i == 1 && valve[i].stop > valve[i - 1].start)
                    end[i] = valve[i - 1].start;
                else
                    end[i] = valve[i].stop;
                x = end[i] - on[i];
                f = XPLUSLOGARITHMX;
                int val = x / 100;

                if (((val % 2) != 0) && x > 1000)
                {
                    pres = pressurechangerandom(f, val);
                    x += 1;
                }
                else
                {
                    pres = computemockpressure(f, val);
                    x += 1;
                }
            }
            i += 1;
        }
        return pres;
    };

}