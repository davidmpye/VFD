EESchema Schematic File Version 4
LIBS:IV-11-breakout-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
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
L vfd:IV-11 VFD1
U 1 1 5B131352
P 3600 3150
F 0 "VFD1" H 4093 4317 60  0000 C CNN
F 1 "IV-11" H 4093 4211 60  0000 C CNN
F 2 "VFD:IV-11" H 3550 3450 60  0001 C CNN
F 3 "" H 3550 3450 60  0001 C CNN
	1    3600 3150
	1    0    0    -1  
$EndComp
$Comp
L LED:WS2812B D1
U 1 1 5B131414
P 3650 4950
F 0 "D1" H 3991 4996 50  0000 L CNN
F 1 "WS2812B" H 3991 4905 50  0000 L CNN
F 2 "LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm" H 3700 4650 50  0001 L TNN
F 3 "https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf" H 3750 4575 50  0001 L TNN
	1    3650 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5B13176B
P 3650 5250
F 0 "#PWR0101" H 3650 5000 50  0001 C CNN
F 1 "GND" H 3655 5077 50  0000 C CNN
F 2 "" H 3650 5250 50  0001 C CNN
F 3 "" H 3650 5250 50  0001 C CNN
	1    3650 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5B131796
P 4100 3300
F 0 "#PWR0102" H 4100 3050 50  0001 C CNN
F 1 "GND" H 4105 3127 50  0000 C CNN
F 2 "" H 4100 3300 50  0001 C CNN
F 3 "" H 4100 3300 50  0001 C CNN
	1    4100 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 3100 4200 3100
Wire Wire Line
	4100 2950 4000 2950
Wire Wire Line
	4100 2950 4100 3300
$Comp
L power:GND #PWR0106
U 1 1 5B1318AF
P 7650 3250
F 0 "#PWR0106" H 7650 3000 50  0001 C CNN
F 1 "GND" H 7655 3077 50  0000 C CNN
F 2 "" H 7650 3250 50  0001 C CNN
F 3 "" H 7650 3250 50  0001 C CNN
	1    7650 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 2800 7650 3250
Wire Wire Line
	3950 4950 4350 4950
Wire Wire Line
	3350 4950 2900 4950
Text Label 4200 4950 0    50   ~ 0
LED_DOUT
Wire Wire Line
	3600 4050 3600 4200
Wire Wire Line
	3150 3550 2700 3550
Wire Wire Line
	3150 3400 2700 3400
Wire Wire Line
	3150 3250 2700 3250
Wire Wire Line
	3150 3100 2700 3100
Wire Wire Line
	3150 2950 2700 2950
Wire Wire Line
	3150 2800 2700 2800
Wire Wire Line
	3150 2650 2700 2650
Wire Wire Line
	3150 2500 2700 2500
Text Label 2700 2500 0    50   ~ 0
A
Text Label 2700 2650 0    50   ~ 0
B
Text Label 2700 2800 0    50   ~ 0
C
Text Label 2700 2950 0    50   ~ 0
D
Text Label 2700 3100 0    50   ~ 0
E
Text Label 2700 3250 0    50   ~ 0
F
Text Label 2700 3400 0    50   ~ 0
G
Text Label 2700 3550 0    50   ~ 0
DP
Text Label 7500 1900 0    50   ~ 0
A
Text Label 7500 2000 0    50   ~ 0
B
Text Label 7500 2100 0    50   ~ 0
C
Text Label 7500 2200 0    50   ~ 0
D
Text Label 3600 4200 0    50   ~ 0
GRID
Text Label 7500 2300 0    50   ~ 0
E
Text Label 7500 2400 0    50   ~ 0
F
Text Label 7500 2500 0    50   ~ 0
G
Text Label 7500 2600 0    50   ~ 0
DP
Text Label 7500 2700 0    50   ~ 0
GRID
Text Label 2900 4950 0    50   ~ 0
LED_DIN
Wire Wire Line
	7500 1900 7100 1900
Wire Wire Line
	7500 2000 7100 2000
Wire Wire Line
	7500 2100 7100 2100
Wire Wire Line
	7500 2200 7100 2200
Wire Wire Line
	7500 2300 7100 2300
Wire Wire Line
	7500 2400 7100 2400
Wire Wire Line
	7500 2500 7100 2500
Wire Wire Line
	7500 2600 7100 2600
Wire Wire Line
	7500 2700 7100 2700
Wire Wire Line
	7650 2800 7100 2800
Wire Wire Line
	7100 2900 7850 2900
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 5B139411
P 7150 4750
F 0 "J2" H 7070 4525 50  0000 C CNN
F 1 "Conn_01x01" H 7070 4616 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7150 4750 50  0001 C CNN
F 3 "~" H 7150 4750 50  0001 C CNN
	1    7150 4750
	-1   0    0    1   
$EndComp
Wire Wire Line
	7350 4750 7800 4750
Text Label 7800 4750 0    50   ~ 0
LED_DOUT
Wire Wire Line
	4200 3100 4200 2800
Text Label 4200 2800 0    50   ~ 0
HEATER+V
Wire Wire Line
	3650 4650 3650 4400
Text Label 3650 4400 0    50   ~ 0
LED+V
Text Label 7850 2900 0    50   ~ 0
HEATER+V
$Comp
L Connector:Conn_01x13_Male J1
U 1 1 5B190F7C
P 6900 2500
F 0 "J1" H 7006 3278 50  0000 C CNN
F 1 "Conn_01x13_Male" H 7006 3187 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x13_P2.54mm_Vertical" H 6900 2500 50  0001 C CNN
F 3 "~" H 6900 2500 50  0001 C CNN
	1    6900 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 3000 7750 3000
Wire Wire Line
	7100 3100 7750 3100
Text Label 7750 3100 0    50   ~ 0
LED_DIN
Text Label 7750 3000 0    50   ~ 0
LED+V
$EndSCHEMATC
