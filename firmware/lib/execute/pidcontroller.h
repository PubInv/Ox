/*=====================================================================================
 *       Filename:  PIDController.h
 *
 *    Description:  
 *
 *        Version:  4.0
 *        Created:  07/26/2021
 *       Revision:  Revised after Version 3.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H
#include "controller.h"
#include "mpr_pressure.h"
#include "timer.h"
#include "config.h"
#include "valve.h"

using namespace std;
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;
using namespace PIOC_Timer;

namespace PIDController
{

    struct ControlGains
    {
        /*
            Initializes the control gains structure
        */
        float kp;
        float kd;
        float ki;
    };

    class PIDControl
    {
        //Sets control gains as a parameter to the PIDControl class.
        ControlGains c;

    public:
        void checkifSystemisOn(SensorStatus st, PIOCMode pstate);
        void initGains(float a, float b, float y);
        void startingGains(ValveStatus vs);
        void InitialControlGainsSensor(double d,SensorStatus st, PIOCMode mod);
        void InitialControlGainsValve(ValveStatus vs, PIOCMode mod, SensorStatus st);
        void multiplyGains(float x, float y, float z);
        double computeSum(int i, double *err);
        void changeTiming(int i, float a);
        void immediateChange(int j, PIOC_Controller::Valve *valve);
        double* ControllerComp(double *error);
        void ImplementController(double d, SensorStatus st, ValveStatus vs, PIOCMode mod, double *err);
    };
}
#endif