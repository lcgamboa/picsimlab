EESchema Schematic File Version 4
EELAYER 30 0
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
Wire Wire Line
	4500 2750 4700 2750
Wire Wire Line
	4500 2850 4700 2850
Wire Wire Line
	4700 2950 4500 2950
$Comp
L Connector:Conn_01x03_Male J1
U 1 1 5EA77924
P 4300 2850
F 0 "J1" H 4408 3039 50  0000 C CNN
F 1 "Conn_01x03_Male" H 4408 3040 50  0001 C CNN
F 2 "" H 4300 2850 50  0001 C CNN
F 3 "~" H 4300 2850 50  0001 C CNN
	1    4300 2850
	1    0    0    -1  
$EndComp
$Comp
L Motor:Motor_Servo M1
U 1 1 5EA77FE4
P 5000 2850
F 0 "M1" H 5332 2869 50  0000 L CNN
F 1 "Motor_Servo" H 5332 2824 50  0001 L CNN
F 2 "" H 5000 2660 50  0001 C CNN
F 3 "http://forums.parallax.com/uploads/attachments/46831/74481.png" H 5000 2660 50  0001 C CNN
	1    5000 2850
	1    0    0    -1  
$EndComp
$EndSCHEMATC
