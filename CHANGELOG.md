## v0.8.10 (2022-04-09)

### New

* Bluepill board updated to use qemu-stm32 version 6.2.50. [fariouche]

* Option to use single 7 Segment Display part. [lcgamboa]

* New part IO 74xx573 octal latch. [lcgamboa]

* New Text Box spare part. [lcgamboa]

* Support to change On position in switches parts and color status indication. [lcgamboa]

* Support to change color of LEDs part (Red, Green, Blue, Yellow and White) [lcgamboa]

* Support to use +3V3 instead only +5V in some boards. [lcgamboa]

* New RGB LED WS2812B part added. [lcgamboa]

* Bounce effect added to Switches part and boards McLab1, McLab2, X and PICGenios. [lcgamboa]

* LCD HD44780 support to read in 4bits mode. [lcgamboa]

* Support to RAM data write/read breakpoints for PIC16F/18F added. [lcgamboa]

* Timer peripheral added to RIPES board. [lcgamboa]

* Support to automatic save the last open workspace file (.pzw) [lcgamboa]

* New spare part LDR. [lcgamboa]

* Support to PIC16F887 added to boards Breadboard, McLab2 and PICGenios. [lcgamboa]

* New board RemoteTCP with Ripes support added. [lcgamboa]

* New part DS1621 I2C temperature sensor. [lcgamboa]

* Support to add combined part LCD_hd44780 with PCF8574 I2C expander. [lcgamboa]

* Support to ATTiny85 USI TWI master mode. [lcgamboa]

* Push buttons bounce effect added. [lcgamboa]

* Added oscilloscope support to show measures in screen. [lcgamboa]

### Changes

* Use system default application to open the serial terminal. [lcgamboa]

* IO PCF8574 I2C expander support to read pins added. [lcgamboa]

* Use default application to open VCD files in VCDDump part. [lcgamboa]

* Move uCboard from experimental to stable version. [lcgamboa]

* Speed optimization to only process parts when io is updated (only PICSim and simavr backend) [lcgamboa]

### Fix

* Remove simavr external interrupt aways enabled with slow down simulation. [lcgamboa]

* Save external EEPROM from boards to workspace files. [lcgamboa]

* Bitbang I2C ACK fixed for all I2C parts. [lcgamboa]

* Issue with device 74xx595 serial ouptut fixed. [lcgamboa]

* Fixes month writing issue in RTCs. [lcgamboa]

* Fix power off statusbar messages. [lcgamboa]

* Board McLab2 7 segments display bug removed. [lcgamboa]

* IO Uart part slow simulation bug removed. [lcgamboa]

* Signal Generator part frequency shift error removed. [lcgamboa]

* Removed the bug when all pins of step motor part are zero. [lcgamboa]


## v0.8.9 (2021-07-25)

### New

* Support to TinyDebug message write added to Franzininho DIY board. [lcgamboa]

* Support to open files using drag and drop. [lcgamboa]

* Buzzer new passive mode Tone added. [lcgamboa]

* New part Fixed Voltage. [lcgamboa]

* New part SHT3X temperature and humidity sensor. [lcgamboa]

* New part FM50 temperature sensor. [lcgamboa]

* New part LM35 temperature sensor. [lcgamboa]

* Support to dump Flash memory in rcontrol interface. [lcgamboa]

* Support to dump RAM and internal EEPROM memory in rcontrol interface. [lcgamboa]

* Support to save EEPROM of AVR MCU between PICSimLab sessions. [lcgamboa]

* Pin Viewer tool added. [lcgamboa]

* VCD Dump support to change external VCD viewer (eg. use pulseview instead gtkwave) [lcgamboa]

* New part MPU6050 accel and gyro added (only raw values, no FIFO or DMP) [lcgamboa]

* Support to only draw when need update to reduce cpu usage. [lcgamboa]

* Rcontrol new command loadhex. [lcgamboa]

* New board picture for Franzinino DIY. [dantop03]

### Fix

* Buzzer passive mode audio clicks removed. [lcgamboa]

* Removed the problem that caused PICSimLab don't start when qemu-stm32 was not found. [lcgamboa]


## v0.8.8 (2021-05-08)

### New

* New part 7 segment display with BCD-7S decoder. [lcgamboa]

### Changes

* Remote control running in one separated thread and support to show real simulation speed. [lcgamboa]

### Fix

* Error on arduino uno bootloader addr fixed. [lcgamboa]
* Memory leek of lxrad library removed. [lcgamboa]


## v0.8.7 (2021-04-22)

### New

* All pictures converted to SVG. [lcgamboa]

* Initial support to Franzininho DIY board with attiny85. [lcgamboa]

* Resize board to display size when board size is bigger than display. [lcgamboa]

* Support to scroll ans scale spare parts window with mouse. [lcgamboa]

* Added a status bar int the Spare parts window to show alias, scale and offset. [lcgamboa]

* Adding PQDB board to the project. [Rodrigo Almeida]

* Support to use SVG instead PNG for boards and part pictures. [lcgamboa]

* Oscilloscope channel info on screen added. [lcgamboa]

* Spare part window menu "add" split in "Inputs", "Outputs","Others" and "Virtual" [lcgamboa]

* Part Ultrasonic HC-SR04 added. [lcgamboa]

* Menu entry to download MPLABX debugger plugin. [lcgamboa]

* Command "pins" to show pins status and dir in control interface. [lcgamboa]

### Changes

* Docs moved to picsimlab_docs repo. [lcgamboa]

### Fix

* Missing +5V on atemga2560 and attiny85 added. [lcgamboa]

* Support to use windows serial ports above COM9. [lcgamboa]

* Arduino IDE bootloaders reset to programm bug fixed. [lcgamboa]

* No value on pins on debug stepping bug removed. [lcgamboa]


## v0.8.6 (2021-01-25)

### New

* Part DC Motor added. [lcgamboa]

* Part encoder (rotary) added. [lcgamboa]

* Support to configure remote control port number in configuration window. [lcgamboa]

* Push buttons and potentiometer animation implemented. [lcgamboa]

* New part rotary potentiometer. [lcgamboa]

* Support to active parts with low input signal for LEDs, RGB LED, Buzzer and 7 segments display. [lcgamboa]

* Support to save board potentiometers position. [lcgamboa]

* Initial support to TCP remote control interface. [lcgamboa]

* Initial support to touch controller xpt2046. [lcgamboa]

* Support to read in ili9340 8 bit interface added. [lcgamboa]

* Support to set offset in Signal Generator added. [lcgamboa]

* Support to qemu_stm32 analog channels added. [lcgamboa]

* Support to ili9341 8 bit parallel mode added. [lcgamboa]

### Fix

* Correct state of uart txpin on reset fixed. [lcgamboa]

* Serial bitbang tx bug fixed. [lcgamboa]

## v0.8.5 (2020-11-28)

### New

* Support to read version of workspaces. [lcgamboa]

* Board_avr initial support to bitbang uart added. [lcgamboa]

* Part virtual terminal added. [Luis Claudio Gambôa Lopes]

* Gamepad Analogic part added. [Luis Claudio Gambôa Lopes]

* Support to spare parts orientation (rotation 0, 90, 180 and 270) [Luis Claudio Gambôa Lopes]

* Initial support to part LCD ili9341. [lcgamboa]

* Part lcd ssd1306 SPI and I2C added. [lcgamboa]

* Part Jumper Wires added. [lcgamboa]

* New part IO UART. [lcgamboa]

* SD card part added. [lcgamboa]

* VCD Play part added. [lcgamboa]

* Board STM32_H103 added. [lcgamboa]

* Part ethernet shield w5500 added. [lcgamboa]

* Support to pin name alias in spare part window. [lcgamboa]

* Added support to PIC18F46J50. [lcgamboa]

### Changes

* More microcontrollers of PICSim added in PICGenios, Breadboard and McLab2 boards. [lcgamboa]

### Fix

* Picsim boards debug disable on load hex bug fixed. [lcgamboa]

* Support to mean value in all pins of boards McLAb2 and PICGenios. [lcgamboa]

* Signal generator fixed to generate digital signal. [lcgamboa]


## v0.8.2 (2020-08-25)

### New

* Initial support to PIC microcontrollers of gpsim as backend simulator. [lcgamboa]

* Support for selecting board, processor and files on the command line. [lcgamboa]

* Added support to boards and parts help link and about information. [lcgamboa]

* LCD hd44780 read support added. [lcgamboa]

* Build script with support to build experimental features. [lcgamboa]

* Initial support to uCsim Z80 and STM8S103. [lcgamboa]

* New board Curiosity HPC. [lcgamboa]

* Clock frequency of 64 and 48Mhz added. [lcgamboa]

* Initial support to STM32 Blue Pill board. [lcgamboa]

* Support to uCsim 8051. [lcgamboa]

* New preference tags and support to legacy workspace (before 0.8.2) [lcgamboa]

* Support to save clock speed for each board. [lcgamboa]

* Dynamic parts and boards registration for better modularization. [lcgamboa]

* Examples are moved to other repository. [lcgamboa]

* Menu option to kick load Arduino bootloader. [lcgamboa]

* Support to change debug TCP port. [lcgamboa]

* Support to chose AVR debugger type. [lcgamboa]

* Support to atmega2560. [lcgamboa]

### Changes

* Spare parts properties combo and input text sizes increased. [lcgamboa]

* Using boards names instead numbers. [lcgamboa]

* Board debug support methods renamed. [lcgamboa]

* Register macros simplified. [lcgamboa]

* Board registration improved. [lcgamboa]

### Fix

* Added missing two phase excitation on step motor part. [lcgamboa]

* Fixed the lose of parts on spare part window when open new pzw file. [lcgamboa]

* AVR GDB debug speed increased. [lcgamboa]


## v0.8.1 (2020-06-27)

### New

* Support to PIC16F18324. [lcgamboa]

* MCP23017 part added and WProp window pointer redefined. [lcgamboa]

* Support to flash address greater than 16 bits. [lcgamboa]

* VCD dump analogic added. [lcgamboa]

* LED matrix serial output and angle support added. [lcgamboa]

* Default Open Value for pin support added. [lcgamboa]

* 16e intel hex file read and write. [lcgamboa]

* Add instructions for running the prebuilt Windows executable in Wine. [Tomáš Hübelbauer]

* Support to PIC16F18324. [lcgamboa]

* MCP23017 part added and WProp window pointer redefined. [lcgamboa]

* Support to flash address greater than 16 bits. [lcgamboa]

* VCD dump analogic added. [lcgamboa]

* LED matrix serial output and angle support added. [lcgamboa]

### Changes

* Board5 switch from atmega328 to atmega328p. [lcgamboa]

* Remove chtml when not used. [lcgamboa]

* Kicad files updated. [lcgamboa]

* Updated to use only online help. [lcgamboa]

* Board5 switch from atmega328 to atmega328p. [lcgamboa]

### Fix

* Disable debug config bit fixed. [lcgamboa]

* LCD hd44780 default initialization bug fixed. [lcgamboa]

* Mean output value bug fixed. [lcgamboa]

* Buzzer audio bug fixed. [lcgamboa]

* Pin name bug fixed. [lcgamboa]

* Temperature system bug fixed. [lcgamboa]

* Mplabxd end free bug fixed. [lcgamboa]

* Disable debug config bit fixed. [lcgamboa]

* LCD hd44780 default initialization bug fixed. [lcgamboa]

* Mean output value bug fixed. [lcgamboa]

* Buzzer audio bug fixed. [lcgamboa]

* Pin name bug fixed. [lcgamboa]

* Temperature system bug fixed. [lcgamboa]


## v0.0.8 (2019-12-24)

### New

* Support to board0 

* RTC and memory parts added

## v0.0.7 (2017-08-15)

### New

* Support to ATMEGA328 using simavr

* Spare parts window added

* Oscilloscope device added

## v0.0.6 (2015-10-14)

### New

* MPLABX debugger support implemented

* Animated cooler implemented

* Serial baud rate emulation support

* Reload last hex file implemented

* LCD cursor draw and LCD shift support added

* LCD full fontset added

* Sampled I/O for speed otimization 

* Board processor combo added

## v0.0.5 (2011-07-26)

### New

* psp support 18f

* Configure Window and psp memory limits

* RTC DS1307 implemented

* Max clock auto ajust 

* Resize window implemented

* Board 5 and 6 added

* I2C Bus bug fixed

* LCD 4 bit mode implemented

## v0.0.4 (2011-03-22)

### New

* memory save files 

* LCD dot matrix implemented


