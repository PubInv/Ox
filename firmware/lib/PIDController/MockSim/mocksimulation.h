/*=====================================================================================
 *       Filename:  mocksimulation.h
 *
 *    Description:  
 *
 *        Version:  2.0
 *        Created:  07/29/2021
 *       Revision:  Revised after Version 1.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#ifndef MOCKSIMULATION_H
#define MOCKSIMULATION_H

#include <stdio.h>
#include <controller.h>
#include <config.h>
#include <math.h>
#include <PIDCnewVersion4.h>

using namespace std;
using namespace PIDController;
namespace PIOC_MockSimulation
{
    enum SELECTFUNCTION
    {
        XPLUSLOGARITHMX,
        XPLUSEXPONENTX
    };

    class MockSim
    {
        SELECTFUNCTION function;
        
        public:
        SELECTFUNCTION setfunction(SELECTFUNCTION value){
            function = value;
            return function;
        }
        float* mockpressuresim(SELECTFUNCTION function, float on[], float end[]);
        float computemockpressure(SELECTFUNCTION function, float x);
        float pressurechangerandom(SELECTFUNCTION function, float x);
    };
};

#endif
