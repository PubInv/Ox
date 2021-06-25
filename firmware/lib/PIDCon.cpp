#include <Arduino.h>
#include <controller.h>
#include <mpr_pressure.h>
#include <timer.h>
#include <cmath>
#include <config.h>
#include <valve.h>
#define NUM_VALVES 4 //Referenced from config.h
using namespace PIOC_Valve;
using namespace PIOC_Sensor;
using namespace PIOC_Controller;
namespace PIDController {
    int integral = 0;
    int deriv;
    //Referenced from config.h     // { id, num, state, pin, start, stop }
    int valves[NUM_VALVES][6] = {
     { 0, 0, 0, 1, 100, 4000 },
     { 1, 1, 0, 2, 4000, 8000 },
     { 2, 2, 0, 4, 3700, 4000 },
     { 3, 3, 0, 8, 7700, 8000 }};
    //Referencing update valve timing from Valve

    //Referencing MPRPressure.cpp for getting pressure sensor readings
    class ControlGains{
        
        uint16_t kp;
        uint16_t kd;
        uint16_t ki;
        void InitialControlGainsSensor(SensorStatus ss, SensorState st, PIOCMode mode){    
            if (ss == 'OK' && (st.pressure> (1.27*st.minPSI)) && mode == 'RUNNING')
            {
                kp = 0.65;
                ki = 0.56;
                kd = 0.34;}
            else if (ss == 'OK' &&(st.pressure <= 0.85 *st.maxPSI ) && mode == 'RUNNING')
            {
                kp = 0.54;
                kd = 0.67;
                ki = 0.43;}
            else{
                kp = 0.59;
                kd = 0.623;
                ki = 0.367;}
           }
        void InitialControlGainsValve(ValveStatus vs, ValveState vt, PIOCMode mode, SensorState st){
            if (vs == 'OK' && mode == 'STARTING'){ //Checks for error at the start.
                if ((vt.onTime > vt.offTime) ){
                    kp = 0.56;
                    kd = 0.34; // Derivative Gain is halved.
                    ki = 0.45;} // Integral gain is reduced to compensate for the state error.
                if ((vt.onTime <= vt.offTime) ){
                    kp = 0.56;
                    kd = 0.63; // Derivative Gain is double the instance when the onTime is increased. 
                    ki = 0.65; }//Integral gain is slightly raised to compensate for the state error.
            }}
                    //Need to check about tackling MISSED and ERROR
        //Need to shiftOutValves to alter the timing of the oxygen concentrator. 
           
        void ControllerComp(SensorState st, ValveState vt ){
            uint16_t sum = 0;
            uint16_t dsum = 0;
            int error[] = {3,4,5};  //Initialized as dupe array
            int pressure[] ={1.4,5.6}; //Initialized as dupe array
            if (vt.isOn == true){
                for (int i = 0; i++; i<= vt.OnTime){
                    if (i > 0){
                        uint16_t t = msNow-vt.msLast;
                        pressure[i] = st.pressure;
                        error[i] = pressure[i] - pressure[i-1];
                        if (error [i] > 0.65* error [i-1])
                            kd = kd * 0.87;      // Weighing the derivative factor to make sure we do not have overshoot
                        if ((vt.OnTime - i) <= 0.5)
                        {
                            if (error[i] > 0.057){
                                ki = ki *1.01;    //Reduce the steady state error as much as possible since we are reaching the end of OnTime. 
                            }
                        }
                        deriv = (error[i] - error[i-1])/t; //derivative controller
                        integral = integral + (error[i]*t);
                        sum = sum + kp*error + kd*deriv + ki*integral; //Computing Controller
                    }
                    else {
                        uint16_t t = msNow-vt.msLast;
                        pressure[i] = st.pressure;
                        error[i] = 0;//Desired set rate-st.pressure;
                        integral = error[i]*t;
                        sum = sum + kd*deriv + ki*integral + kp*error[i]; //Computing Controller
                    }
                    for (int j = 0; j++; j<=3){
                        if (valves[j][2] == 0){ //checking state of valve
                            if (valves[j][4] <=  msNow < valves[j][5]){
                                int modulation = st.pressure;
                                if ( (vt.pin == valves[j][3]) && ((msNow - valves[j][4])<= (valves[j][5] - msNow) )){
                                    for (int k = 0; k++; k<= vt.onTime){ //Checks for error at the start of OnTime in all the valves.
                                        if (error[k] > error[k-1]){
                                            kp = kp * 0.93;             //Adjusts the gains if the error at the next time step is higher than the error at the prev time step. 
                                            kd = kd * 1.04;             // kd is increased to avoid overshoot. kp is slightly decreased to avoid cumulative error. 
                                            ki = ki * 0.68;             // ki is slightly decreased to avoid overshoot and reduce cumulative error.
                                        }
                                        else {
                                            kd = kd * 0.54;             //If the error at the next time step is smaller than or equal to error at the previous time step, kd is decreased to avoid unnecessary error accumulations. 
                                            ki = ki *0.53;              // ki is slightly decreased to avoid overshoot and reduce cumulative error.
                                        } 
                                    }
                                }
                                if ( (vt.pin == valves[j][3]) && ((msNow - valves[j][4]) > (valves[j][5] - msNow) )){
                                    for (int k = 0; k++; k<= vt.onTime){
                                        if (error[k] > error[k-1]){     //Increase the gains and make the error as small as possible as we are reaching the end of OnTime.
                                            kp =  kp* 1.1;              //kp is increased if the error at the next time step is higher than the error at the prev time step.
                                            kd = kd *1.05;              // kd is increased to avoid overshoot. 
                                            ki = ki * 1.08;             // ki is slightly increased to avoid overshoot and reduce cumulative error.
                                        }
                                        else {
                                            kd = kd * 0.54;              // kd is reduced to make sure there is no overshoot and the error stays close to 0.
                                            ki = ki * 1.13;              // if error is lesser than that of previous state. We are increasing ki again when compared to the previous case to reduce oscillations.
                                        } 
                                    }} } }}
                } }
            else {
                    for (int i = 0; i++; i<= vt.OffTime){
                        if (i > 0){
                            int t = msNow -vt.msLast;
                            pressure[i] = st.pressure;
                            error[i] = pressure[i] - pressure[i-1];
                            sum = sum + error[i] ;
                             if (error[i] >= 1.3* error[i-1])
                                kd = kd * 1.2;
                                kp = kp * 1.15; //If the error accumulation increases largely, we change the PD gains.
                        }
                        else {
                            pressure[i] = st.pressure;
                            error[i] = 0;
                        }} }}}    
}
