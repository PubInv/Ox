/*===========================================================================================
 *   Filename:  mocksimulation.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/25/2021
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/

#ifndef MOCKSIMULATION_H
#define MOCKSIMULATION_H

#include <stdio.h>
#include <math.h>
using namespace std;
namespace PIOC_MockSimulation
{

    enum SELECTFUNCTION
    {
        XPLUSLOGARITHMX,
        XPLUSEXPONENTX
    };

    class MockSim
    {
        SELECTFUNCTION f;

    public:
        double* mockpressuresim(SELECTFUNCTION f, int on[], int end[]);
        float computemockpressure(SELECTFUNCTION f, float x);
        float pressurechangerandom(SELECTFUNCTION f, float x);
    };
}
#endif