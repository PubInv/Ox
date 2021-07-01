/*=====================================================================================
 *       Filename:  PIDController.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2021
 *       Revision:  none
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 * =====================================================================================*/
#include<stdio.h>
#include <controller.h>
#include <mpr_pressure.h>
#include <timer.h>
#include <cmath>
#include <config.h>
#include <valve.h>
#include <PIDCnew.h>
#define NUM_VALVES 4 //Referenced from config.h
using namespace PIDController;
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;

namespace PIDController {
    
    int valves[NUM_VALVES][6] = {
  { 0, 0, 0, 1, 100, 4000 },
  { 1, 1, 0, 2, 4000, 8000 },
  { 2, 2, 0, 4, 3700, 4000 },
  { 3, 3, 0, 8, 7700, 8000 }};

    bool checkifSystemisOn (SensorState st, PIOCState pstate) {
            if (st.status == 0 && pstate.mode == RUNNING)
                printf("PID Controller can be initialized");
    }
    
    int kp;
    int kd;
    int ki;
    bool startingGains(int kp,int  kd,  int ki,  ValveState vt){
        if (vt.status == 0){
            kp = 0.5;
            kd = 0.5;
            ki = 0.5;
        }
        return true;
    }
    
    void ImplementController(SensorState st, ValveState vt, ValveStatus vs, PIOCState pstate){
        uint16_t ms = PIOC_Timer::timeSinceEpochMs();
        ControlGains c;
        c.InitialControlGainsSensor(SensorStatus ss, SensorState st, PIOCState pstate);
        c.InitialControlGainsValve(ValveState vt, PIOCState state, SensorState st);
        int con = c.ControllerComp(ValveState vt);
        kp = c.kp;
        kd = c.kd;
        ki = c.ki;
        for (int i = 0; i++; i<= ms){
            if (valves[i][4] <= ms <= valves[i][5])
                if (vt.isOn == true){
                    int valves[i][4] = con*valves[i][4];
                } 
                
                 
            else if(((valves[i][4] - ms) <= 500) ||   ((ms - valves[i][5]) >= 500 ))       
                if (vt.isOn == false){
                    int valves[i][5] = con*valves[i][5];
            }
            if (((valves[i][4] - ms) <))
        }
    }}
