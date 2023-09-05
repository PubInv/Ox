# Due CFC Pinouts

This document describes the pinouts for the Stage 2 heater


| Due Pin Number  | 	Signal Name  | Set as I/O  | Connection  |  Notes |
|---|---|---|---|---|
| A0 |	FAN1_FG | Input | Blower 1 Tachometer
| D4 |	TEMP1	| BIDirect |	Dallas One-Wire connection to Thermocouple Breakouts |	Daisy chain connection to temprature probes. Address of ???
| D9 | nFAN1_PWM | Output | Blower PWM | This output will be inverted
| D18 |	TX1 | Output |	Digital Power Supply TF800 Pin 23 | This is Serial1 TX for (power supply)[https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
| D19 | RX1 |Input | Digital Power Supply TF800 Pin 24 | This is Serial1 RX for (power supply) [https://assets.alliedelec.com/v1560852133/Datasheets/1d230174086e96b6e4801d1c963649f3.pdf]
| D22 | BLOWER_ENABLE |Output |	Blower Enable 
| D32 | GPAD_nCS | Output |	 External SPI inverted select (for the GPAD)
| D44 |LPBK0 |Output |	Varying loopback signal
| D45 |LPBK1 |Input |	Read of digital loopback signal
| D51 |HEAT1 |Output |	Positive SSR signal for Internal Heater
| D50 |HEAT2 |Output |	Positive SSR signal for External Heater 1
| D49 |HEAT3 |Output |	Positive SSR signal for External Heater 2
| 3.3V | Pull up rail of blower board| Source | This is the small perfboard I soldered to gether with pullup/pulldown reistors and then glued onto the DB25 PCB.
| GND |	Pull down rail and GND of blower board |Sink|	Please see the Fritzing Diagram of this breakout board: https://github.com/PubInv/NASA-COG/blob/develop/fancontroller/SanyoAceBreakout.fzz
| GND | | Sink | |			Digital Power Supply TF800 Pin 22
| D10 | SD_CD | Pull up |	Pull Up for SD carde detect
| D24 | BTN_2 | Pull Up |	Pull up for EncoderB
| D40 | BTN_1 | Pull Up |	Pull up for EncoderA
| D39 | BTN_ENC | Pull Up |	Pull up for Encoder push button
| D48 | LCD_SS | Output |	Signal Select for LCD SPI
| D47 | MOSI | Output |	SPI MOSI signal
| D2 | MISO | Output |	SPI MISO signal and DO signal
| D8 | CLK | Output |	SPI Clock signal
| D38 | LCD_CS | Output |	SPI LCD Chip Select signal LCD_EN
| D31 | LCD_A0 | Output |	SPI LCD A0 RS signal 
| D53 | LCD_LED_Blue | Output |	LED Enable High On
| D37 | LCD_LED_Red | Output |	LED Enable High On
| D23 | LCD_LED_Green | Output |	LED Enable High On
| D30 | LCD_BEEP | Output |	Buzzer Enable High On
| D3 | TC_1_CS | Output |	SPI TC_1 Chip Select signal
| D29 | TC_2_CS | Output |	SPI TC_2 Chip Select signal
| D28 | TC_3_CS | Output |	SPI TC_3 Chip Select signal
| D46 | LCD_RST | Input |	Reset pin, could be left unconnected also indicates LCD present
| RST | KILL / RST | INPUT |	Pull Down reset pin, with external 10k pull up resistor and filter capacitor 0.1uF
| D3 | TC_1_CS | Output |	SPI TC_1 Chip Select signal


Notes: 			
Most components require a GND rail. There are three of them on the DUE. I have tied the all of these to GND.			
The GND rail is tied to the GND of the 12V (fixed) power supply, which is also tied to the GND bus bar.			
Pin Signal Names are constant, all upper case. 			
Leading "n" indicated active low naming convention			


