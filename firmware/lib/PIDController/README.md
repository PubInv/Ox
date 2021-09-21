# **PIDController Writeup**:

#### *(Under Development...)* 

(kp: Proportional Gain, ki: Integral Gain, kd: Derivative Gain) At the start, we initialize the PID Control Gains to a default value.

### Initial Control Gains Sensor (Function):

Depending on the desired pressure we need and the pressure during the starting phase, 
We are changing the kp, ki, kd gains based on the pressure at the starting phase.
	
If the desired pressure is too high and the current pressure is well below the desired pressure.
			
    kp is increased to reach the desired pressure quickly.
    ki and kd are still low.

If the desired pressure is high and the current pressure is close to the desired pressure.
			
    kp is reduced since we are close to reaching the desired pressure.
		
    ki and kd are increased since the pressure is already near the desired pressure
    at the starting phase.

If the desired pressure is too low and the current pressure limits is well above the desired pressure. 
			
    kp is slightly increased since we need to desired pressure quickly.
    ki and kd are slightly increased too to reduce oscillations when we are nearing the desired pressure.
    
### Initial Control Gains Valve (Function):

Checks if the onTime is less than 40 percent of the OffTime.
	
    If thats the case, kp, ki and kd are all increased at the start 
    to make sure the current pressure is near the desired pressure at the start. 
    This makes sure the desired pressure is achieved within the given time duration. 
		
Checks if the onTime is greater than 40 percent of the OffTime. 

    If thats the case, integral and derivative gains are made higher than the proportional gain 
    to make sure that there are no oscillations while reaching the desired pressure. 
			
   *Since the onTime is higher than the offTime, we can clearly say that the pressure rate will be higher.* 
   *However, there might be certain cases where the current pressure is well above the desired pressure.* 
   *In such cases, we are making sure that the error is not too large by keeping the proportional gain above 0.5*

### MultiplyGains (Function):

    Takes in the values and alters the gains accordingly. 

### ComputeSum (Function):

    Computes the controller value : Sum = kp * error + kd * derivative error + ki * integral error.		

### ChangeTiming (Function):

    Takes in the controller sum and changes the start time and stop time of valves. 

### Immediate Change (Function):

    Making sure the stop time of the current valve is increased and the start time of the valve at the outlet is decreased to increase oxygen flow.
		
    If the error at the next time step is greater than the error at the previous time step, 
    we are closing the current valve and opening the next valve to make sure that the O2 goes through the sink and not through the outlet.

### ControllerComp (Function):

#### ON TIME

   For every valve, during the start of OnTime, adjusts the gains if the error at the next time step is higher
   than the error at the previous time step. 

    kd is increased to avoid overshoot. kp remains the same more or less. 
    ki is slightly decreased to avoid overshoot and reduce cumulative error.
			
   If the error at the next time step is smaller than or equal to error at the previous time step, 
    
    kd is decreased to avoid unnecessary error accumulations. ki remains the same.
		
   If we are reaching the end of OnTime:
			
   Increase the gains and make the error as small as possible as 
   we are reaching the end of OnTime.
   If the error at the next time step is greater than the error at the previous time step, 
    
    kp is increased if the error at the next time step is higher 
    than the error at the prev time step.
					
     kd is increased to avoid overshoot. ki is slightly increased to avoid overshoot and reduce cumulative error.
			
   If the error at the next time step is lesser than the error at the previous time step, 
     
     kd is reduced to make sure there is no overshoot and the error stays close to 0,
     We are increasing ki again when compared to the previous case to reduce oscillations.
      
#### OFF TIME
   During the offTime of one particular valve, we are checking if the error is significantly larger during the last few time states.
   If there is a significant error during the last few time states, we have to check if the pressure 
   at the pressure sensor is above/below the desired pressure. 
   We have to close/open the subsequent valves accordingly.
				
    In addition to changing the PD gains we are making sure that the 
    onTime and offTime of valve set (1,3) and (2,4) "This can be changed if the valve set is (1,2) and (3,4)." 
    is altered to decrease oxygen flow. 
			
    If the error accumulation decreases, we are making sure that the pressure rate increases 
    by increasing the onTime duration of valves (1,3) and decreasing the 
    onTime duration of valves (2,4) or vice versa.
### Implement Controller (Function):

    Sums up the aggregate of gains from all the test cases mentioned before by 
    taking into account all the functions. 

    Changes the timing of alternate set of valves by a simple feedback loop. 


# Mock Pressure Simulation:

   **Mock Pressure Simulation for a.b ^(x)**&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**PID Performance**
   
   ![MockPressure Sim](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/image1.jpg) ![PIDPerformance](
https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/XPLUSEXPONENTX.png)
   
   
   <br/>
   <br/>
   
   **Mock Pressure Simulation for a.x + log(b.x)**&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**PID Performance**
   ![MockPressure Sim2](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/image2.jpg) ![PIDPerformance](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/XPLUSLOGARITHMX.png)

<br/>
<br/>

   **Mock Pressure Simulation for Sigmoid Function: 1/(1 +ax.power(1.7x))**
   <br/>
   <br/>
   A mix of XPLUSLOGARITHMX and XPLUSEXPONENTX have been utilized to simulate the mock pressure.
   &nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br/>
   <br/> **PID Performance**
   
   ![MockPressure Sim3](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/image4.jpg) ![PIDPerformance](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/SIGMOID1.png)
   
   </br>
   </br>
   
   **Mock Pressure Simulation (2) for Sigmoid Function: 1/(1 - a*x.power(1.7x))** 
   
   A mix of XPLUSLOGARITHMX and y = -ax.^2 + bx have been utilized to simulate the mock pressure. 
   
   ![MockPressure Sim4](https://github.com/PubInv/Ox/blob/Pranav/firmware/lib/PIDController/image3.jpg) 
 ### Details of Mock Pressure Simulation:

| Simulation             | Mock Pressure Description                                                    |
| ---------------------- |:----------------------------------------------------------------------------:| 
| XPLUSLOGARITHMX        | Simulated a couple of functions in  ax + log(bx) with different limits       |
| XPLUSEXPONENTX         | Simulated a couple of functions in ab.^(x) with different limits             |
| SIGMOID Simulation (1) | Simulated functions ab.^(x) and ax + log(bx) with different limits           |  

   
