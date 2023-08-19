# Due CFC Pinouts

This document describes the pinouts for the OEDCS designed for the CFC of approximately Aug. 18th.


| Due Pin Number  | 	Signal Name  | Set as I/O  | Connection  |  Notes |
|---|---|---|---|---|
| A0 |	FAN1_FG | Input | Blower 1 Tachometer
| D4 |	TEMP1	| BIDirect |	Dallas One-Wire connection to Thermocouple Breakouts |	Daisy chain connection to temprature probes. Address of ???
| D9 | nFAN1_PWM | Output | Blower PWM | This output will be inverted
| D18 |	TX1 | Output |	Digital Power Supply TF800 Pin 23 | This is Serial1 TX for (power supply)[https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
| D19 | RX1 |Input | Digital Power Supply TF800 Pin 24 | This is Serial1 RX for (power supply) [https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
| D51 |HEAT1 |Output |	Positive SSR signal
| 3.3V | Pull up rail of blower board| Source | This is the small perfboard I soldered to gether with pullup/pulldown reistors and then glued onto the DB25 PCB.
| GND |	Pull down rail and GND of blower board |Sink|	Please see the Fritzing Diagram of this breakout board: https://github.com/PubInv/NASA-COG/blob/develop/fancontroller/SanyoAceBreakout.fzz
| GND | | Sink | |			Digital Power Supply TF800 Pin 22
| GND |

Notes: 			
Most components require a GND rail. There are three of them on the DUE. I have tied the all of these to GND.			
The GND rail is tied to the GND of the 12V (fixed) power supply, which is also tied to the GND bus bar.			
Pin Signal Names are constant, all upper case. 			
Leading "n" indicated active low naming convention			


## Digitaly controled Power Supply				
Note: The DPS connections are particularly delicate in the OEDCS SN#1.				
Two signal wires and one GND wire are used. Jumper wires are plugged into a female connector which is plugged into the power supply. 				
The numbering of this power supply is a little tricky. Please see the datasheet: [https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf](https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf)			
In additon to the GND wire, the DUE's Serial1 interface is used. This is pins 19(RX) and 18(TX). 				
These are connected to Pins on the TF800:				
				
| TF800 Signal Name	| TF800 PIN |	Due PIN	| Due Signal Name	|
|---|---|---|---|---|---|
| TX|	24|	19|	RX|	
|RX |	23|	18|	TX|	
|GND|	22|	GND|	GND|	
