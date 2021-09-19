## (unreleased)

### New

* Support to add combined part LCD_hd44780 with PCF8574 I2C expander. [lcgamboa]

* Support to ATtiny85 USI TWI master mode. [lcgamboa]

* Push buttons bounce effect added. [lcgamboa]

* Oscilloscope support to show measures in screen added. [lcgamboa]

### Changes

* Speed optimization to only process parts when io is updated (only simavr) [lcgamboa]

### Fix

* IO Uart part slow simulation bug removed. [lcgamboa]

* Signal Generator part frequency shift error removed. [lcgamboa]

* Removed the bug when all pins of step motor part are zero. [lcgamboa]


## v0.8.9 (2021-07-25)

### New

* Support to TinyDebug message write added to Franzininho DIY board. [lcgamboa]

* Support to open files using drag and drop. [lcgamboa]

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

* Support to only draw parts when need update to reduce cpu usage. [lcgamboa]

* Support to only draw board when need update to reduce cpu usage. [lcgamboa]

### Fix

* Buzzer passive mode audio clicks removed. [lcgamboa]

* Removed the problem that caused PICSimLab don't start when qemu-stm32 was not found. [lcgamboa]

### Other

* Rcontrol new command loadhex. [lcgamboa]

* Merge pull request #38 from dantop03/master. [lcgamboa]

* Merge branch 'master' of https://github.com/dantop03/picsimlab Correções. [dantop03]

* Correções imagem e arquivos Franzininho. [dantop03]

* Merge pull request #37 from dantop03/master. [lcgamboa]

* Merge branch 'lcgamboa:master' into master. [dantop03]

* Atualização arquivos Franzininho. [dantop03]

* New board pic for Franzinino DIY. [dantop03]


## v0.8.8 (2021-05-08)

### New

* New part 7 segment display with BCD-7S decoder. [lcgamboa]

### Changes

* Remote control running in one separated thread and support to show real simulation speed. [lcgamboa]

### Fix

* Error on arduino uno bootloader addr fixed. [lcgamboa]


## v0.8.7 (2021-04-22)

### New

* New menu "virtual" in spare parts window. [lcgamboa]

* Initial support to Franzininho DIY board with attiny85. [lcgamboa]

* Resize board to display size when board size is bigger than display. [lcgamboa]

* Support to scroll ans scale spare parts window with mouse. [lcgamboa]

* Added a status bar int the Spare parts window to show alias, scale and offset. [lcgamboa]

* Support to use SVG instead PNG for part pictures. [lcgamboa]

* Support to use SVG instead PNG for boards pictures. [lcgamboa]

* Oscilloscope channel info on screen added. [lcgamboa]

* Spare part window menu "add" split in "Inputs", "Outputs" and "Others" [lcgamboa]

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

### Other

* Merge pull request #30 from rmaalmeida/master. [lcgamboa]

* Adding PQDB board to the project. [Rodrigo Almeida]

* Adding PQDB board to the project. [Rodrigo Almeida]

* Merge pull request #2 from lcgamboa/master. [Rodrigo Almeida]

* Merge pull request #1 from lcgamboa/master. [Rodrigo Almeida]


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

* Microcontroller with more than 40 pins support ! minor. [lcgamboa]

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

* Initial support to STM32 Blue Pill board. [lcgamboa]

* Support to ucsim 8051. [lcgamboa]

* New preference tags and support to legacy workspace (before 0.8.2) [lcgamboa]

* Support to save clock speed for each board. [lcgamboa]

* Dynamic parts registration for better modularization. [lcgamboa]

* Dynamic board registration for better modularization. [lcgamboa]

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

* Board registartion improved. [lcgamboa]

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

### Other

* Merge branch 'new_features' of https://github.com/lcgamboa/picsimlab into new_features. [lcgamboa]

* Lxaudio support updated. [lcgamboa]

* Read pin updated. [lcgamboa]

* Refresh var removed. [lcgamboa]

* Default Open Value for pin support added. [lcgamboa]

* 16e intel hex read and write. [lcgamboa]

* Tools remove chtml when use external browser. [lcgamboa]

* Remove chtml when not used. [lcgamboa]

* Emscripten JS support. [lcgamboa]

* C++ compat. [lcgamboa]

* Makefile updated. [lcgamboa]

* Docs updated. [lcgamboa]

* Kicad files updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Makefile cross win32 updated. [lcgamboa]

* Readme updated. [lcgamboa]

* Makefiles and scripts updated. [lcgamboa]

* Makefiles and scripts updated. [lcgamboa]

* Makefiles and scripts updated. [lcgamboa]

* Makefiles and scripts updated. [lcgamboa]

* Makefiles updated. [lcgamboa]

* Package script updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Debian info updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Debian info updated. [lcgamboa]

* Script updated. [lcgamboa]

* Debian control updated. [lcgamboa]

* Debian package info. [lcgamboa]

* Debian package info. [lcgamboa]

* Package script. [lcgamboa]

* Makefile updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Install script updated. [lcgamboa]

* Install script updated. [lcgamboa]

* Makefile updated. [lcgamboa]

* Update to use only online help. [lcgamboa]

* Mplabxd end free bug fixed. [lcgamboa]

* Board 0 label centralized. [lcgamboa]

* Board 0 label centralized. [lcgamboa]

* Merge pull request #12 from TomasHubelbauer/patch-1. [lcgamboa]

* Fix a typo in xquartz name. [Tomáš Hübelbauer]

* Merge pull request #7 from TomasHubelbauer/patch-1. [lcgamboa]

* Add instructions for running the prebuilt Windows executable in Wine. [Tomáš Hübelbauer]


