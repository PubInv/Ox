/*=====================================================================================
 *       Filename:  PIDController.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2021
 *       Revision:  none
 *       
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/

#include <controller.h>
#include <mpr_pressure.h>
#include <timer.h>
#include <cmath>
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
        uint16_t kp;
        uint16_t kd;
        uint16_t ki;
    };

    //Referencing MPRPressure.cpp for getting pressure sensor readings

    class PIDControl
    {
    public:
        ControlGains c;
        void checkifSystemisOn(SensorState *st, PIOCState *pstate);
        void startingGains(int kp, int kd, int ki, ValveState vt);
        void InitialControlGainsSensor(SensorState *st, PIOCState *pstate);
        void InitialControlGainsValve(ValveStatus *vs, ValveState *vt, PIOCState *pstate, SensorState *st);
        void multiplyGains(int x, int y, int z);
        int computeSum(int i, int error[]);
        void changeTiming(int i, float a);
        int16_t ControllerComp(SensorState *st, ValveState *vt);
        void ImplementController(SensorState *st, ValveState *vt, ValveStatus *vs, PIOCState *pstate);
    };
}
