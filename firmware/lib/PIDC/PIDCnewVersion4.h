/*=====================================================================================
 *       Filename:  PIDController.h
 *
 *    Description:  
 *
 *        Version:  3.0
 *        Created:  07/09/2021
 *       Revision:  Revised after Version 2.0
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/

#include <controller.h>
#include <mpr_pressure.h>
#include <timer.h>
#include <config.h>
#include <valve.h>
#include <stdio.h>
#define NUM_VALVES 4 //Referenced from config.h
using namespace std;
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;
using namespace PIOC_Timer;
namespace PIDController
{

    //Referencing update valve timing from Valve
    struct ControlGains
    {
        float kp;
        float kd;
        float ki;
    };

    //Referencing MPRPressure.cpp for getting pressure sensor readings

    class PIDControl
    {
        ControlGains c;
        void checkifSystemisOn(SensorState *st, PIOCState *pstate);
        void startingGains(float kp, float ki, float kd, ValveState vt);
        void InitialControlGainsSensor(SensorState *st, PIOCState *pstate);
        void InitialControlGainsValve(ValveStatus *vs, ValveState *vt, PIOCState *pstate, SensorState *st);
        void multiplyGains(float x, float y, float z);
        float computeSum(float i, float error[]);
        void changeTiming(int i, float a);
        void immediateChange(int j,PIOC_Controller::Valve *valve);
        float ControllerComp(SensorState *st, ValveState *vt);
        void ImplementController(SensorState *st, ValveState *vt, ValveStatus *vs, PIOCState *pstate);
    };
}
