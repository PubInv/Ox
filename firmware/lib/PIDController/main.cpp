#include <iostream>
#include <mocksimulation.h>
#include <pidcontroller.h>
#include <math.h>
#include <controller.h>
#include <PID_v2.h>

using namespace PIDController;
using namespace PIOC_MockSimulation;
using namespace std;
using namespace PIOC_Controller;
int main(){
    MockSim m;
    SELECTFUNCTION f = XPLUSLOGARITHMX; 
    int j = 1;
    SensorState *st;
    ValveState *vt;
    PIOC_Controller::Valve *valve = valveArray;
    double y;
    double output;
    double a;
    double aggKp = 2.34, aggKi = 1.2, aggKd = 2.33;
    double Kp = 0.0161;
    double Ki = 0.0031228;
    double Kd = 0.013052;
    //double Kp = 0.00261;
    //double Ki = 0.000128;
    //double Kd = 0.00252;
    int on[] = {3,4};
    int end[] = {1,3};
    double *pdup;
    double k[] = {};
    pdup = m.PIOC_MockSimulation::MockSim::mockpressuresim(f, on, end);
    //Intended pressure(setpoint) for Mock (XPLUSLOGARITHMX): y = 0.0035008*x + 1.996499
    //Intended pressure(setpoint) for Mock (XPLUSEXPONENTX): y = 0.0017504*x + 4.498249
   
    double q;
    int g =0;
    PID_v2 myPID( Kp, Ki, Kd, PID::Direct );
    j = valveArray[0].start;
    double out;
    while (j<= valveArray[0].stop){
        
        //if ( f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
        y = 0.0035008*j + 1.996499;
        //if (f ==PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
        //y = 0.0017504*j + 4.498249; 
        //if (j == 100)
        output = 0.0017504*(j+ 120) + 4.498249;
        //if (j < 2000)
            //output = q;
        //cout<<"output"<<output<<endl;
        myPID.Start(*(pdup+g),output, y );
        a = *(pdup+g);
        double error = abs( a-y);
        if (a-y > 0){
            myPID.SetControllerDirection(DIRECT);
        }
        if (a - y<0){
            myPID.SetControllerDirection(REVERSE);
        }
        if (error < 0.5) {
        // we're close to setpoint, use conservative tuning parameters
            myPID.SetTunings(Kp, Ki, Kd);
        } else {
        // we're far from setpoint, use aggressive tuning parameters
            myPID.SetTunings(aggKp, aggKi, aggKd);
        }
        q = myPID.Run(*(pdup+g));
        cout<<"Error: "<<abs(q-y)<<endl;    
        j = j + 1;
        g = g+1;
    }   
    return 0;
}