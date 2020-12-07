## (unreleased)

### Fix

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


