#include <fstream>
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
int main()
{
    /*
        UNDER DEVELOPMENT
        Takes into account the PID controller developed by myself and the PID controller library
        referenced from Arduino to simulate the performance of the controller on the mock pressure.
    */
    SensorStatus st;
    ValveStatus vs;
    PIOCMode mod = PIOCMode::RUNNING;
    double y;
    double ydup;
    double output;
    double error[valveArray[1].stop - 99] = {};
    double dupy[valveArray[1].stop] = {};
    double time[valveArray[1].stop] = {};
    int on[] = {3, 4};
    int end[] = {1, 3};
    MockSim m;
    PIDControl p;
    SELECTFUNCTION f = SELECTFUNCTION::REGULARPID;
    double *pdup;
    pdup = m.mockpressuresim(f, on, end); //Input Pressure
    int j = 0;
    int arr = 0;
    double d = 0;
    ofstream mo;
    mo.open("data.dat");
    ofstream mc;
    mc.open("data1.dat");
    while (j < 1)
    {
        int x = int(valveArray[0].start);
        int dup = int(valveArray[0].start);
        if (j == 1)
        {
            x = valveArray[0].start - 100;
            dup = valveArray[0].start - 100;
        }
        while (x <= (valveArray[0].stop))
        {
            //Computes the error between the desired pressure and the mock pressure simulation.

            if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
            {
                y = 0.0040501 * x + 1.9995949;
                ydup = 0.0040501 * dup + 1.9995949;
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
            {
                y = 0.00325008 * x + 1.9996749; // Set Point Pressure
                ydup = 0.00325008 * dup + 1.9996749;
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID1)
            {
                y = 1 / (1 - 0.0909839 * pow(x, (1.7 * 0.1609986))); // Set Point Pressure
                ydup = 1 / (1 - 0.0909839 * pow(dup, (1.7 * 0.1609986)));
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID)
            {
                y = 1 / (1 - 0.08599 * pow(x, (1.7 * 0.169002)));
                ydup = 1 / (1 - 0.08599 * pow(dup, (1.7 * 0.169002)));
            }
            else
            {
                y = 8.0;
                ydup = 8.0;
            }

            //error[arr] = y - *(pdup +arr);                              // Difference between Input and SetPoint
            //cout<<"Error: "<<error[arr]<<endl;
            mo << dup << "  " << ydup << endl;
            if (dup >= 4000)
                mo.close();

            x = x + 1;
            dup = dup + 200;
            arr = arr + 1;
        }
        j = j + 1;
    }
    double *err = error;
    double *back = p.ImplementController(d, st, vs, mod, err); //Depending on the setpoint and input, we
    // initialize the gains and run the PID
    //Controller
    double Kp = 0.13219;
    double Ki = 0.99090;
    double Kd = 0.03792;

    double q;
    int g = 0;
    /*PID_v2 myPID(Kp, Ki, Kd, PID::Direction::Direct);
    j = 100;

    int l = 0;
    output = 2.43487 * pow(1.000335, 100);
    y = 8.0;
    myPID.Start(output, output, y);
        
    while (j <= 4000)
    {

        output = 2.43487 * pow(1.000335, j);
        //cout<<*(pdup+g)<<endl;
        //if ( f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
        y = 8.0;
        //double error = abs( a-y);

        // we're close to setpoint, use conservative tuning parameters
        //else {
        // we're far from setpoint, use aggressive tuning parameters
        //myPID.SetTunings(aggKp, aggKi, aggKd);
        //}
        double gap = abs(y - output); // distance away from setpoint
        if (gap < 2)
        {
            // we're close to setpoint, use conservative tuning parameters
            //myPID.SetTunings(Kp, Ki, Kd);
        }
        else
        {
            // we're far from setpoint, use aggressive tuning parameters
            //myPID.SetTunings(aggKp, aggKi, aggKd);
        }
        q = myPID.Run(output);
        mc << (valveArray[0].start + l) << " " << (q) << " " << endl;
        l = l + 1;
        cout << "Pressure: " << q-y << endl;
        j = j + 1;
    }
    mc.close();
    */
    arr = 0;
    j = 0;
    while (j < 1)
    {
        int x = int(valveArray[0].start);
        if (j == 1)
        {
            x = valveArray[0].start - 100;
        }
        while (x <= (valveArray[0].stop))
        {
            //Computes the error between the desired pressure and the mock pressure simulation.

            if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSLOGARITHMX)
            {
                y = 0.0040501 * x + 1.9995949;
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::XPLUSEXPONENTX)
            {
                y = 0.00325008 * x + 1.9996749; // Set Point Pressure
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID1)
            {
                y = 1 / (1 - 0.0909839 * pow(x, (1.7 * 0.1609986))); // Set Point Pressure
            }
            else if (f == PIOC_MockSimulation::SELECTFUNCTION::SIGMOID)
            {
                y = 1 / (1 - 0.08599 * pow(x, (1.7 * 0.169002)));
            }
            else
                y = 8.0;
            dupy[arr] = y; // Difference between Input and SetPoint
            x = x + 1;
            arr = arr + 1;
        }
        j = j + 1;
    }

    int k = 0;
    //cout<<""<<sum<<endl;

    return 0;
}