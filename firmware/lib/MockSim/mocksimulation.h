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

#include <stdio.h>
#include <PIDCnewVersion4.h>
#include <math.h>
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
        SELECTFUNCTION f;
        PIDController::PIDControl p;

    public:
        float mockpressuresim(SELECTFUNCTION f, PIDController::PIDControl p, float on[], float end[]);
        float computemockpressure(SELECTFUNCTION f, float x);
        float pressurechangerandom(SELECTFUNCTION f, float x);
    };
};
