# Electrical Information on ODECOS Controler(s) and Plant(s)
PCB Connector Pin signal names, Due Signal Names, Chassis wiring and related information
Inverse cronological order

## Unit 3-???
A place holder for future work
See: TBD


## Unit 2
### Notes for creation of a new PCB  
This table pulls from information in the references 1 and 2.   
References:
1. Due Pin-Out Map for OEDCS1 : https://docs.google.com/spreadsheets/d/18wP1Cyg5-j3FP92x7no_JsCCFNvo9y2avyn5nvmAj6I/edit#gid=0
2. Fan Controller and Fan Wiring for OEDCS1: https://docs.google.com/document/d/1n3v-AxqNKEzcKqmHicGKnFmG8sKbNEL_pGaUlU9CZ8U/edit

#### Due Pin Wiring
| **Due Pin Number** 	| **Breakout** 	| **Signal Name** 	| **Set as I/O** 	| **Connection**                      	| **Notes**                             	|
|--------------------	|--------------	|-----------------	|----------------	|-------------------------------------	|---------------------------------------	|
| D0                 	| J6-2         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D1                 	| J6-1         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D2                 	| J6-3         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D3                 	| J6-4         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D4                 	| J6-5         	| TEMP1           	| ?BiDirect?     	| Dallas 1 Wire / MAX31850K           	| Three daisy chained                   	|
| D5                 	| J6-6         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D6                 	| J6-7         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D7                 	| J6-8         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D8                 	| J7-1         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D9                 	| J7-2         	| nFAN1_PWM       	| Output         	| Controlles transistor Q1            	| Was BLOWER_PWM                        	|
| D10                	| J7-3         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D11                	| J7-4         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D12                	| J7-5         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D13                	| J7-6         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D13                	| J7-6         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D14                	| J2-1         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D15                	| J2-2         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D16                	| J2-3         	| NA              	| Input pullup.  	| None                                	| Can be TX2                            	|
| D17                	| J2-4         	| NA              	| Input pullup.  	| None                                	| Can be RX2                            	|
| D18                	| J2-5         	| D_PSU_TX        	| UART1 TX       	| D_PSU_TX, R15 (then to Power Supply 	| TX1                                   	|
| D19                	| J2-6         	| D_PSU_RX        	| UART1 RX       	| D_PSU_RX, R14 (then to Power Supply 	| RX1                                   	|
| D20                	| J2-7         	| SDA             	| SDA            	| test point SDA1, R4, R13            	| SDA                                   	|
| D21                	| J2-8         	| SCL             	| SCL            	| test point SCL1, R2, R16            	| SCL                                   	|
| D22                	| J8-4         	| BLOWER_ENABLE   	| OUTPUT         	| R10                                 	| Enable Switched_Fan_Power at J15 9-12 	|
| D23                	| J8-3         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D24                	| J8-6         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D25                	| J8-5         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D26                	| J8-8         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D27                	| J8-7         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D28                	| J8-10        	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D29                	| J8-9         	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D30                	| J8-12        	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D31                	| J8-11        	| NA              	| Input pullup.  	| None                                	| Not used                              	|
| D32                	| J8-14        	| nCS_GPAD        	| Output for nCS 	| R55, R61, Q5                        	| on 3.3 Volt                           	|
| D33                	| J8-13        	| TC_nCS1         	| Output for nCS 	| Thermocouple 1, R7                  	| on 3.3 Volt                           	|
| D34                	| J8-16        	| TC_nCS2         	| Output for nCS 	| Thermocouple 2, R3                  	| on 3.3 Volt                           	|
| D35                	| J8-15        	| TC_nCS3         	| Output for nCS 	| Thermocouple 3, R1                  	| on 3.3 Volt                           	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|
|                    	|              	|                 	|                	|                                     	|                                       	|


### Chassis Wiring of ODECOS Unit 2.



## Unit 1
Wiring of ODECOS Unit 1 Notes
1. Due Pin-Out Mape for OEDCS1 : https://docs.google.com/spreadsheets/d/18wP1Cyg5-j3FP92x7no_JsCCFNvo9y2avyn5nvmAj6I/edit#gid=0
2. Fan Controller and Fan Wiring for OEDCS1: https://docs.google.com/document/d/1n3v-AxqNKEzcKqmHicGKnFmG8sKbNEL_pGaUlU9CZ8U/edit

Wiring of thermocouple amplifires
![image](https://github.com/PubInv/NASA-COG/assets/5836181/81112c45-1a9b-4129-8997-f63fd8dc1c07)

Revisions:
* 20230821, Update DB25 Male 2-5 from +12V to "+24V Switched (fans)"

| Fan Breakout 	| Signal-Function   	| DB25 MALE Pin # 	| DB25 FEMALE Pin # 	| Other 	| Due Pin 	| Due Signal 	|
|--------------	|-------------------	|-----------------	|-------------------	|-------	|---------	|------------	|
| 1            	| +24V Switched (fans) 	| 2-5             	| 9-12              	|       	| ???      	| FAN_POWER         	|
| 2            	| GND for all fans  	| 22-25           	| 14-17             	|       	| GND     	| NA         	|
| 3            	| #1 PWM            	| 13              	| 1                 	| J10-1 	| D9      	| nFAN1_PWM  	|
| 4            	| #1 Tach           	| 9               	| 5                 	| J9-1  	| A0      	| FAN1_FG    	|
| 5            	| #2 PWM            	| 12              	| 2                 	| J10-2 	|         	| nFAN2_PWM  	|
| 6            	| #2 Tach           	| 8               	| 6                 	| J9-2  	|         	| FAN2_FG    	|
| 7            	| #3 Tach           	| 11              	| 3                 	| J10-3 	|         	| nFAN3_PWM  	|
| 8            	| #3 PWM            	| 7               	| 7                 	| J9-3  	|         	| FAN3_FG   	|
| 9            	| #4 Tach           	| 10              	| 4                 	| J10-4 	|         	| nFAN4_PWM  	|
| 10           	| #4 PWM            	| 6               	| 8                 	| J9-4  	|         	| FAN4_FG   	|
|              	|                   	|                 	| 25                	| J11-3 	|         	|            	|
|              	|                   	|                 	| 13                	| J11-4 	|         	|            	|
|              	|                   	|                 	| 18                	| J12-1 	|         	|            	|
|              	|                   	|                 	| 19                	| J12-2 	|         	|            	|
|              	|                   	|                 	| 20                	| J12-3 	|         	|            	|
|              	|                   	|                 	| 21                	| J12-4 	|         	|            	|
|              	|                   	|                 	| 22                	| J12-5 	|         	|            	|
|              	|                   	|                 	| 23                	| J12-6 	|         	|            	|
|              	|                   	|                 	| 24                	| J12-7 	|         	|            	|

