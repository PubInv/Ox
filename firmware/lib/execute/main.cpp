
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
    MockSim m;
    SELECTFUNCTION f = XPLUSLOGARITHMX; 
    int j = 1;
    SensorStatus st;
    ValveState vt;
    PIOC_Controller::Valve *valve = valveArray;
    double y;
    double output;
    double a = 0;
    double aggKp = 8.34, aggKi = 8.2, aggKd = 8.33;
    double Kp = 1.0161;
    double Ki = 1.31228;
    double Kd = 1.613052;
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
   
    double q = 0;
    int g =0;
    a = *(pdup+ g);
    y = 0.0035008*j + 1.996499;
    PID_v2 myPID( Kp, Ki, Kd, PID::Direct );
    j = int(valveArray[0].start - 100);
    double out;    
    while (j <= int(valveArray[0].stop -100)){
        //cout <<"Mock Pressure"<<g<<" "<<a<<endl;
       
        //if ( f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
    
        //cout <<"Mock Pressure"<<y<<endl;
        //if (f ==PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
        //y = 0.0017504*j + 4.498249; 
        //if (j == 100)
        
        //if (j < 2000)
            //output = q;
        //cout<<"output"<<output<<endl;
        a = *(pdup+ g) + (q-y);
        y = 0.0035008*j + 1.996499;
        output = 0.0017504*(j+ 120) + 4.498249 ;
        myPID.Start(a,output, y );
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
        } if (error >= 0.5) {
        // we're far from setpoint, use aggressive tuning parameters
            myPID.SetTunings(aggKp, aggKi, aggKd);
        }
        q = myPID.Run(a);
        cout<<"Error: "<<(q - y)<<endl;    
        j = j + 1;
        g = g+1;
    }   
    return 0;
}