EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L v1-rescue:74HC595-74xx U?
U 1 1 5F55E21B
P 7350 2650
F 0 "U?" H 7350 3431 50  0000 C CNN
F 1 "74HC595" H 7350 3340 50  0000 C CNN
F 2 "" H 7350 2650 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 7350 2650 50  0001 C CNN
	1    7350 2650
	1    0    0    -1  
$EndComp
$Comp
L v1-rescue:+24V-power #PWR?
U 1 1 5F56C437
P 3350 1550
F 0 "#PWR?" H 3350 1400 50  0001 C CNN
F 1 "+24V" H 3365 1723 50  0000 C CNN
F 2 "" H 3350 1550 50  0001 C CNN
F 3 "" H 3350 1550 50  0001 C CNN
	1    3350 1550
	1    0    0    -1  
$EndComp
$Comp
L v1-rescue:Conn_01x16_Female-Connector Adafruit_Feather_ESP32
U 1 1 5F596BFB
P 5900 6300
F 0 "Adafruit_Feather_ESP32" H 5792 7185 50  0000 C CNN
F 1 "Conn_01x16_Female" H 5792 7094 50  0001 C CNN
F 2 "" H 5900 6300 50  0001 C CNN
F 3 "~" H 5900 6300 50  0001 C CNN
	1    5900 6300
	-1   0    0    -1  
$EndComp
$Comp
L v1-rescue:Conn_01x12_Female-Connector Adafruit_Feather_ESP32
U 1 1 5F58DD30
P 5900 4550
F 0 "Adafruit_Feather_ESP32" H 5792 5235 50  0000 C CNN
F 1 "Conn_01x12_Female" H 5792 5144 50  0001 C CNN
F 2 "" H 5900 4550 50  0001 C CNN
F 3 "~" H 5900 4550 50  0001 C CNN
	1    5900 4550
	-1   0    0    -1  
$EndComp
$Comp
L v1-rescue:GND-power #PWR?
U 1 1 5F57FB9A
P 10350 5600
F 0 "#PWR?" H 10350 5350 50  0001 C CNN
F 1 "GND" H 10355 5427 50  0000 C CNN
F 2 "" H 10350 5600 50  0001 C CNN
F 3 "" H 10350 5600 50  0001 C CNN
	1    10350 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 5400 10350 5500
$Comp
L v1-rescue:EA_eDIPTFT32-ATP-Display_Graphic U?
U 1 1 5F55E9EE
P 10350 4100
F 0 "U?" H 10300 5581 50  0001 C CNN
F 1 "Adafruit_ILI9431" H 10300 5490 50  0000 C CNN
F 2 "Display:EA_eDIPTFT32-XXX" H 10350 2600 50  0001 C CNN
F 3 "http://www.lcd-module.com/fileadmin/eng/pdf/grafik/ediptft43-ae.pdf" H 10650 3900 50  0001 C CNN
	1    10350 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 5400 10250 5500
Wire Wire Line
	10250 5500 10350 5500
Connection ~ 10350 5500
Wire Wire Line
	10350 5500 10350 5600
Wire Wire Line
	10250 2800 10250 2650
Wire Wire Line
	10350 2800 10350 2650
Wire Wire Line
	10350 2650 10250 2650
$Comp
L v1-rescue:VCC-power #PWR?
U 1 1 5F583948
P 10250 2350
F 0 "#PWR?" H 10250 2200 50  0001 C CNN
F 1 "VCC" H 10267 2523 50  0000 C CNN
F 2 "" H 10250 2350 50  0001 C CNN
F 3 "" H 10250 2350 50  0001 C CNN
	1    10250 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 2350 10250 2650
Connection ~ 10250 2650
$EndSCHEMATC
