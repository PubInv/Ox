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
L 74xx:74HC595 U?
U 1 1 5F55E21B
P 8450 4300
F 0 "U?" H 8450 5081 50  0000 C CNN
F 1 "74HC595" H 8450 4990 50  0000 C CNN
F 2 "" H 8450 4300 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 8450 4300 50  0001 C CNN
	1    8450 4300
	1    0    0    -1  
$EndComp
$Comp
L Display_Graphic:EA_eDIPTFT32-ATP U?
U 1 1 5F55E9EE
P 10600 3850
F 0 "U?" H 10550 5331 50  0000 C CNN
F 1 "EA_eDIPTFT32-ATP" H 10550 5240 50  0000 C CNN
F 2 "Display:EA_eDIPTFT32-XXX" H 10600 2350 50  0001 C CNN
F 3 "http://www.lcd-module.com/fileadmin/eng/pdf/grafik/ediptft43-ae.pdf" H 10900 3650 50  0001 C CNN
	1    10600 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR?
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
L Connector:Conn_01x16_Female J?
U 1 1 5F596BFB
P 5900 6300
F 0 "J?" H 5792 7185 50  0000 C CNN
F 1 "Conn_01x16_Female" H 5792 7094 50  0000 C CNN
F 2 "" H 5900 6300 50  0001 C CNN
F 3 "~" H 5900 6300 50  0001 C CNN
	1    5900 6300
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x12_Female J?
U 1 1 5F58DD30
P 5900 4550
F 0 "J?" H 5792 5235 50  0000 C CNN
F 1 "Conn_01x12_Female" H 5792 5144 50  0000 C CNN
F 2 "" H 5900 4550 50  0001 C CNN
F 3 "~" H 5900 4550 50  0001 C CNN
	1    5900 4550
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
