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
    /*
        UNDER DEVELOPMENT
        Takes into account the PID controller developed by myself and the PID controller library
        referenced from Arduino to simulate the effect of the controller on the mock pressure
    */

    /*MockSim m;
    SELECTFUNCTION f = SELECTFUNCTION::XPLUSLOGARITHMX; 
    */int j;
    SensorStatus st;
    ValveStatus vs ;
    PIOCMode mod = PIOCMode::RUNNING;
    PIOC_Controller::Valve *valve = valveArray;
    double y;
    double output;
    double error[valveArray[1].stop] = {};
    double aggKp = 2.34, aggKi = 1.2, aggKd = 2.33;
    //double Kp = 0.0161;
    //double Ki = 0.0031228;
    //double Kd = 0.013052;
    //int on[] = {3,4};
    //int end[] = {1,3};
    //double* pdup;
    double k[] = {};
    PIDControl p;    
    int on[] = {3,4};
    int end[] = {1,3};
    MockSim m;
    SELECTFUNCTION f = SELECTFUNCTION::XPLUSLOGARITHMX; 
    double* pdup;
    pdup = m.mockpressuresim(f, on, end);
    //cout<<*(pdup+23);
    //Intended pressure(setpoint) for Mock (XPLUSLOGARITHMX): y = 0.0035008*x + 1.996499
    //Intended pressure(setpoint) for Mock (XPLUSEXPONENTX): y = 0.0017504*x + 4.498249
    int x = 0;
    while (x<= (valveArray[0].stop + 900))
    {
        /*Computes the error between the desired pressure and the mock pressure simulation.
        */
        if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
            y = 0.0040501*x + 1.9995949;
        if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
            y = 0.00325008*x + 1.9996749;
        error[x] = y - *(pdup +x);
        x = x+1;
    }
    double* err = error;
    x = 0;
    /*while (x <= (valveArray[0].stop - 500)){
        //cout<<""<<*(err+x)<<endl;
        x = x+1;
    }*/
        
    p.ImplementController(st, vs, mod,  err);
    //cout<<""<<sum<<endl;
    /*
    double q;
    int g =0;
    //PID_v2 myPID( Kp, Ki, Kd, PID::Direct );
    j = 0;
    double out;
    while (j<= 8000){
        //cout<<*(pdup+g)<<endl;
        //if ( f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
        y = 0.0035008*j + 1.996499;
        //if (f ==PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
        //y = 0.0017504*j + 4.498249; 
        output = 0.0017504*(j+ 150) + 4.498249;
        //myPID.Start(*(pdup+g),output, y );
        //double error = abs( a-y);
        
        // we're close to setpoint, use conservative tuning parameters
         //else {
        // we're far from setpoint, use aggressive tuning parameters
            //myPID.SetTunings(aggKp, aggKi, aggKd);
        //}
        //q = myPID.Run(*(pdup+g));
        //cout<<"Pressure"<<*(pdup +j)<<endl;
        j = j + 1;
        g = g+1;
    }   */
    return 0;
}