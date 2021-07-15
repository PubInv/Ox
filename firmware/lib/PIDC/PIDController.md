PIDController writeup: 

(kp: Proportional Gain, ki: Integral Gain, kd: Derivative Gain)
At the start, we initialize the PID Control Gains to a default value.
	
InitialControlGainsSensor:

			Depending on the desired pressure we need 
			and the pressure during the starting phase, 
			We are changing the kp, ki, kd gains based
		 	on the pressure at the starting phase.
		
			If the desired pressure is too high and the 
			current pressure is well below the desired pressure.
				
				kp is increased to reach the desired pressure quickly.
				ki and kd are still low.
	
			If the desired pressure is high and the current 
			pressure is close to the desired pressure.
				
				kp is reduced since we are close to reaching the desired pressure.
			
				ki and kd are increased since the pressure is already near the desired pressure
				at the starting phase.

			If the desired pressure is too low and the 
			current pressure limits is well above the desired pressure. 
				
				kp is slightly increased since we need to desired pressure quickly.

				ki and kd are slightly increased too to reduce oscillations 
				when we are nearing the desired pressure.

InitialControlGainsValve: 
			
			Checks if the onTime is less than 40 percent of the OffTime.
		
				If thats the case, kp, ki and kd are all increased at the start 
				to make sure the current pressure is near the desired pressure at the start. 
				This makes sure the desired pressure is achieved within the given time duration. 
			
			Checks if the onTime is greater than 40 percent of the OffTime. 

				If thats the case, integral and derivative gains are made higher than the proportional gain
				to make sure that there are no oscillations while reaching the desired pressure. 
				
				Since the onTime is higher than the offTime, we can clearly say that the pressure rate will be higher. 
				However, there might be certain cases where the current pressure is well above the desired pressure. 
				In such cases, we are making sure that the error is not too large by keeping the proportional gain above 0.5;
		
multiplyGains: 

			Takes in the values and alters the gains accordingly. 
		
computeSum: 

			Computes the controller value : Sum = kp * error + kd * derivative error + ki * integral error.		
		
changeTiming:	

			Takes in the controller sum and changes the start time and stop time of valves. 
		
immediateChange: 
		
			Making sure the stop time of the current valve is increased and the start time of the valve at the outlet is decreased to increase oxygen flow.
			
			If the error at the next time step is greater than the error at the previous time step, 
			Closing the current valve and opening the next valve to make sure that the O2 goes through the sink and not through the outlet.

ControllerComp:

			*ON TIME*

			For every valve, during the start of OnTime,
				Adjusts the gains if the error at the next time step is higher
			 	than the error at the prev time step. 

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
						
						kd is increased to avoid overshoot. 
						ki is slightly increased to avoid overshoot and reduce cumulative error.
				
					If the error at the next time step is lesser than the error at the previous time step, 
						kd is reduced to make sure there is no overshoot and the error stays close to 0.
 						We are increasing ki again when compared to the previous case to reduce oscillations.
			*OFF TIME*
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
			
ImplementController: 		

				Sums up the aggregate of gains from all the test cases mentioned before by 
				taking into account all the functions. 

				Changes the timing of alternate set of valves by a simple feedback loop. 
