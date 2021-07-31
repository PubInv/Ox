
#include <iostream>
#include "mocksimulation.h"
#include "pidcontroller.h"
#include <math.h>

using namespace PIDController;
using namespace PIOC_MockSimulation;
using namespace std;

int main(){
    cerr<<"Hello\n";
    MockSim m;

    SELECTFUNCTION f = XPLUSLOGARITHMX; 
    int j = 1;
    PIOC_Controller::Valve *valve;
    PIDControl p;
    float on[] = {3.3,4.5};
    float end[] = {1.2,3.2};
    float *pdup;
    //p.PIDController::PIDControl::immediateChange( j, valve);
    pdup = m.PIOC_MockSimulation::MockSim::mockpressuresim(f, on, end);
    cout<<"Hello"<<pdup<<"";
    //for(int i = 0; i<12000; i++)
        //cout<< *(pdup + i) <<endl;
    return 0;
}