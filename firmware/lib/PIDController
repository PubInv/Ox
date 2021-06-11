/*=====================================================================================
 *
 *       Filename:  PIDController.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/11/2021
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pranav Shankar Srinivasan (spranav1911@gmail.com)
 *
 * =====================================================================================*/
#include <Arduino.h>
#include <Wire.h>
#include <inttypes.h>
#include <iostream>
#include <valve>
#include <refactored>
namespace PIDController {
    bool checkifSystemisOn () {
        PIOC_Sensor :: MPR_Pressure {
            if (state.status == OK)
                print("PID Controller can be initialized")
        }
    }

    class PIDController {
        int kp;
        int kd;
        int ki;
        bool initialize(kp, kd,ki){
           int valve_status = ValveStatus Valve::getValveStatus(){
           if (valve_status == 1){
               kp = 0.5;
               kd = 0.5;
               ki = 0.5;
           }
           }
           return true;
        }
        void constantsmodulation (uint32_t mprpressure){
          uint32_t mprpressure  = MPR_Pressure::getPressure()
            if (mprpressure >= (0.7*state.maxPSI))
                kd = 1.32;
            if (mprpressure <= (1.54*state.minPSI))
                ki = 0.78;
        }

        int setupflowsensor(uint32_t flowsensor){
            
        }
        void getDatafromFlowSensor() {
            




        }
    }


    

}
