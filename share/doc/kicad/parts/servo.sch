EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:parts-cache
EELAYER 25 0
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
L Motor_Servo M1
U 1 1 5A7076A0
P 5000 2850
F 0 "M1" H 4800 3025 50  0000 L CNN
F 1 "Motor_Servo" H 4800 2690 50  0000 L TNN
F 2 "" H 5000 2660 50  0001 C CNN
F 3 "" H 5000 2660 50  0001 C CNN
	1    5000 2850
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x03 J1
U 1 1 5A707719
P 4300 2850
F 0 "J1" H 4300 3050 50  0000 C CNN
F 1 "Conn_01x03" H 4300 2650 50  0000 C CNN
F 2 "" H 4300 2850 50  0001 C CNN
F 3 "" H 4300 2850 50  0001 C CNN
	1    4300 2850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4500 2750 4700 2750
Wire Wire Line
	4500 2850 4700 2850
Wire Wire Line
	4700 2950 4500 2950
$EndSCHEMATC
