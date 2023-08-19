# Due CFC Pinouts

This document describes the pinouts for the OEDCS designed for the CFC of approximately Aug. 18th.


| Due Pin Number  | 	Signal Name  | Set as I/O  | Connection  |  Notes |
|---|---|---|---|---|
| A0 |	FAN1_FG | Input | Blower 1 Tachometer
| D4 |	TEMP1	| ?BIDirect? |	Dallas One-Wire connection to Thermocouple Breakouts |	Daisy chain connection to temprature probes. Address of ???
| D9 | nFAN1_PWM | Output | Blower PWM | This output will be inverted
| D18 |	TX1 | Output |	Digital Power Supply TF800 Pin 23 | This is Serial1 TX for power supply 1	https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf
| D19 | RX1 |Input | Digital Power Supply TF800 Pin 24 | This is Serial1 RX for power supply 1	https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf
| D51 |HEAT1 |Output |	Positive SSR signal
| 3.3V |			| Pull up rail of blower board|	This is the small perfboard I soldered to gether with pullup/pulldown reistors and then glued onto the DB25 PCB.
| GND |		| Pull down rail and GND of blower board	Please see the Fritzing Diagram of this breakout board: https://github.com/PubInv/NASA-COG/blob/develop/fancontroller/SanyoAceBreakout.fzz
| GND | | | |			Digital Power Supply TF800 Pin 22
| GND |

