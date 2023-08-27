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
| **Due Pin Number** 	| **Breakout** 	| **Signal Name** 	| **Set as I/O**   	| **Connection**                      	| **Notes**                             	|
|--------------------	|--------------	|-----------------	|------------------	|-------------------------------------	|---------------------------------------	|
| D0                 	| J6-2         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D1                 	| J6-1         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D2                 	| J6-3         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D3                 	| J6-4         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D4                 	| J6-5         	| D4: SD card chip select | Output     	| via Shield           	|  https://wiki.seeedstudio.com/Ethernet_Shield_V2.0/#interface	|
| D5                 	| J6-6         	| TEMP1            	| BiDirection     	| Dallas 1 Wire / MAX31850K            	| Three daisy chained                    	|
| D6                 	| J6-7         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D7                 	| J6-8         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D8                 	| J7-1         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D9                 	| J7-2         	| nFAN1_PWM       	| Output           	| Controlles transistor Q1            	| Was BLOWER_PWM                        	|
| D10                	| J7-3         	|  W5200 Chip Select| Input pullup.    	| via Shield           	                | SEEED Studio Eternet Shield            	|
| D11                	| J7-4         	| SPI MOSI          | Output.    	      | via Shield                            | SEEED Studio Eternet Shield            	|
| D12                	| J7-5         	| SPI MISO          | Input.    	      | via Shield                            | SEEED Studio Eternet Shield            	|
| D13                	| J7-6         	| SPI SCK           | Output.    	      | via Shield                            | SEEED Studio Eternet Shield            	|
| D13                	| J7-6         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D14                	| J2-1         	| NA              	|                  	| None                                	| Not used                              	|
| D15                	| J2-2         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D16                	| J2-3         	| NA              	| Input pullup.    	| None                                	| Can be TX2                            	|
| D17                	| J2-4         	| NA              	| Input pullup.    	| None                                	| Can be RX2                            	|
| D18                	| J2-5         	| D_PSU_TX        	| UART1 TX         	| D_PSU_TX, R15 (then to Power Supply 	| TX1                                   	|
| D19                	| J2-6         	| D_PSU_RX        	| UART1 RX         	| D_PSU_RX, R14 (then to Power Supply 	| RX1                                   	|
| D20                	| J2-7         	| SDA             	| SDA              	| test point SDA1, R4, R13            	| SDA                                   	|
| D21                	| J2-8         	| SCL             	| SCL              	| test point SCL1, R2, R16            	| SCL                                   	|
| D22                	| J8-4         	| BLOWER_ENABLE   	| OUTPUT           	| R10                                 	| Enable Switched_Fan_Power at J15 9-12 	|
| D23                	| J8-3         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D24                	| J8-6         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D25                	| J8-5         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D26                	| J8-8         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D27                	| J8-7         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D28                	| J8-10        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D29                	| J8-9         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D30                	| J8-12        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D31                	| J8-11        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D32                	| J8-14        	| nCS_GPAD        	| Output for nCS   	| R55, R61, Q5                        	| on 3.3 Volt                           	|
| D33                	| J8-13        	| TC_nCS1         	| Output for nCS   	| Thermocouple 1, R7                  	| on 3.3 Volt                           	|
| D34                	| J8-16        	| TC_nCS2         	| Output for nCS   	| Thermocouple 2, R3                  	| on 3.3 Volt                           	|
| D35                	| J8-15        	| TC_nCS3         	| Output for nCS   	| Thermocouple 3, R1                  	| on 3.3 Volt                           	|
| D36                	| J8-18        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D37                	| J8-17        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D38                	| J8-20        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D39                	| J8-19        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D39                	| J8-19        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D40                	| J8-22        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D41                	| J8-21        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D42                	| J8-24        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D43                	| J8-23        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D44                	| J8-26        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D45                	| J8-25        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D46                	| J8-28        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D47                	| J8-27        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D48                	| J8-30        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D49                	| J8-29        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D50                	| J8-32        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D51                	| J8-31        	| SSR_PWM         	| OUTPUT           	| R9                                  	| Active High.                          	|
| D52                	| J8-34        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| D53                	| J8-33        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| SDA1               	| J7-9         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| SCL1               	| J7-10        	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| CANR               	| J4-7         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| CANT               	| J4-8         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| DAC0               	| J4-5         	| Disable         	| Only Output      	| None                                	| Not used                              	|
| DAC1               	| J4-6         	| Disable         	| Only Output      	| None                                	| Not used                              	|
| A0                 	| J5-1         	| BLOWER_TACH     	| Input pullup.    	| R11                                 	| Measure RPM                           	|
| A1                 	| J5-2         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A2                 	| J5-3         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A3                 	| J5-4         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A4                 	| J5-5         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A5                 	| J5-6         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A6                 	| J5-7         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A7                 	| J5-8         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A8                 	| J4-1         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A9                 	| J4-2         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A9                 	| J4-2         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A10                	| J4-3         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| A11                	| J4-4         	| NA              	| Input pullup.    	| None                                	| Not used                              	|
| AREF               	| J7-8         	| NA              	| From DUE         	| None                                	| Not used                              	|
| nRST1              	| J3-6         	| NA              	| Pulled up on DUE 	| None                                	| Not used                              	|
| IOREF              	| J3-7         	| NA              	| NA               	| None                                	| Not used                              	|
| VIN                	| J3-1         	| VIN             	| NA               	| +12V into this pin through R76      	|                                       	|
| +5V                	| J3-4         	| VIN+5V          	| NA               	| Power from Due voltage regulator    	| Three pins.                           	|
| 3.3V               	| J3-5         	| 3.3V            	| NA               	| Power from Due voltage regulator    	| One pin.                              	|
| MISO               	| tp CPIO1     	| CIPO            	| Input            	| SPI bus                             	|                                       	|
| MOSI               	| tp COPI      	| COPI            	| Output           	| SPI bus                             	|                                       	|
| SCK                	| tp SPI_SCK   	| SPI_SCK         	| Output           	| SPI bus                             	|                                       	|
| nRST2              	| NA           	| SPI_nRST        	| Input            	| S1 reset switch                     	|                                       	|
| end                	|              	|                 	|                  	|                                     	|                                       	|

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

