#include <iostream>
#include "mocksimulation.h"
#include "pidcontroller.h"
#include <math.h>
#include "controller.h"
#include "PID_v2.h"

using namespace PIDController;
using namespace PIOC_MockSimulation;
using namespace std;
using namespace PIOC_Controller;
int main(){
    /*
        UNDER DEVELOPMENT
        Takes into account the PID controller developed by myself and the PID controller library
        referenced from Arduino to simulate the performance of the controller on the mock pressure.
    */

    SensorStatus st;
    ValveStatus vs ;
    PIOCMode mod = PIOCMode::RUNNING;
    double y;
    double output;
    double error[valveArray[1].stop - 100] = {};
    double aggKp = 2.34, aggKi = 1.2, aggKd = 2.33;
    double Kp = 0.000161;
    double Ki = 0.00031228;
    double Kd = 0.0013052;
    PIDControl p;    
    int on[] = {3,4};
    int end[] = {1,3};
    MockSim m;
    SELECTFUNCTION f = SELECTFUNCTION::XPLUSEXPONENTX; 
    double* pdup;
    pdup = m.mockpressuresim(f, on, end);     //Input Pressure
    int j = 0;
    int arr = 0;
    double d = 0;
    while (j<= 1){
        int x = int(valveArray[0].start);
        if (j == 1)
            x = valveArray[0].start - 100;
        while (x<= (valveArray[0].stop))
        {
        /*Computes the error between the desired pressure and the mock pressure simulation.
        */
        if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
            y = 0.0040501*x + 1.9995949;
        if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
            y = 0.00325008*x + 1.9996749;                        // Set Point Pressure
        if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID1)
            y = 1/(1 -0.0909839*pow(x,(1.7*0.1609986)));       // Set Point Pressure
        if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID)
            y = 1/(1 -0.08599*pow(x,(1.7*0.169002)));
        error[arr] = y - *(pdup +x);                              // Difference between Input and SetPoint
        
        if (x == valveArray[j].start )
            d = y;
        x = x+1;
        arr = arr + 1;
        }
        j = j + 1;
    }
    double* err = error;  
    p.ImplementController(d, st, vs, mod,  err);                      //Depending on the setpoint and input, we
                                                               // initialize the gains and run the PID
                                                                   //Controller
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