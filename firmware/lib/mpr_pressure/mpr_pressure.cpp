#include <mpr_pressure.h>

namespace PIOC_Sensor {

    bool MPR_Pressure::update(uint32_t msNow){
        // TODO
        
        return true;
    }

    uint32_t MPR_Pressure::getPressure(){
        return state.pressure;
    }
    
    SensorStatus MPR_Pressure::getStatus(){
        return state.status;
    }
    
    SensorState MPR_Pressure::getState(){
        return state;
    }
    
    bool MPR_Pressure::setUnits(PressureUnits pu){
        units = pu;
        return true;
    }
    
    PressureUnits MPR_Pressure::getUnits(){
        return units;
    }


}