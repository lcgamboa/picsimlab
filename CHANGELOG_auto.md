## (unreleased)

### New

* New spare part VCD Dump Memory (RAM). [lcgamboa]

* Version 0.9.2 realease! [lcgamboa]

### Changes

* Support to 16 bit memory in VCD Dump Memory part added. [lcgamboa]

* PICGenios board demostration updated. [lcgamboa]

* Github actions updated. [lcgamboa]

* Github actions updated. [lcgamboa]

### Fix

* Fix gpboard write to input pins bug. [lcgamboa]

* Fix: Fix I2C memory 24CXXX part Save button (https://github.com/lcgamboa/picsimlab/issues/139). Thanks to @kjellc. [lcgamboa]

* Fix ili9341 with "spi+touch" crashes. Thanks to  m-meltner  https://github.com/lcgamboa/picsimlab/issues/131. [lcgamboa]

* Fix BluePill board PWM outputs. [lcgamboa]

### Other

* Github build scripts updated. [lcgamboa]


## v0.9.2 (2024-10-05)

### New

* Version 0.9.2 realease! [lcgamboa]

* Support to wait for GDB in qemu based boards added. [lcgamboa]

* Add a new picsimlab_tool command line tool to upload firmware direct to PICSimLab (from platformio). [lcgamboa]

* New spare part MCP23017 I2C IO expander. [lcgamboa]

* New part \"Logic Block\" with logic functions: Not, Buffer, And, Nand, Or, Nor, Xor and Xnor. [lcgamboa]

* Increase oscilloscope time scale up to 1s/div. [lcgamboa]

* Support to PIC18F67J60 added to Breadboard board. [lcgamboa]

* Add initial support to ESP32 RMT TX. [lcgamboa]

* Add support to use analog values in gpboard (gpsim backend) [lcgamboa]

* Add I2C support to ESP32C3. [lcgamboa]

* Add support to PWM outputs for ESP32C3 and STM32 boards. [lcgamboa]

### Changes

* Add support to set PWM frequency in STM32 Qemu based boards. [lcgamboa]

* Adds support for showing in the status bar when waiting for GDB connection. [lcgamboa]

* On fails make backup and load demonstrantion workspace to recover. [lcgamboa]

* Add support to set baud rate in bitbang_uart (STM32 boards) [lcgamboa]

* Add support to set baud rate in bitbang_uart (ESP32 and ESP32C3) [lcgamboa]

* Add suport to set PWM frequency in bitbang_pwm (LEDC for ESP32, ESP32C3) [lcgamboa]

* Add support to Read operations and interrupts in MCP23S17 spare part. [lcgamboa]

* Gpboard updated to use gpsim-0.32.1. [lcgamboa]

* Gpsim interface optimization to improve simulation performance. [lcgamboa]

* Changlog updated. [lcgamboa]

### Fix

* Fix BluePill Board onboard LED polarity. [lcgamboa]

* Fix errors when setting time/date in RTC DS1307 and PFC8563. [lcgamboa]

* Fix spare parts Buzzer crash on board reset. [lcgamboa]

* Fix spare parts properties window position. [lcgamboa]

* Fix state of input pins after reset in AVR based boards. [lcgamboa]

* Fix RTC DS1307 support to 12h mode. [lcgamboa]

* Fix Oscilloscope to use all SpareParts pins. [lcgamboa]

* Fix gpboard configuration bits dump for PIC18F. [lcgamboa]

* Fix gpboard simulation clock speed. [lcgamboa]

* Fix spare parts window position on start. [lcgamboa]

* Fix support to multi byte char in command line file name. [lcgamboa]

* Fix path for load rcontrol and debug port from options. [lcgamboa]


## v0.9.1 (2023-12-10)

### New

* Add WIFI support to ESP32C3 and ESP-NOW protocol. [lcgamboa]

* Add support to enable/disable watchdog and ethernet support of ESP32-C3. [lcgamboa]

* Add suport to ESP32 LEDC (PWM output) [lcgamboa]

* SD card support to CRC16 on write block and CRC7 on commands. [lcgamboa]

* SD card initial support to CRC16 on (ESP32 compatibility). [lcgamboa]

### Changes

* Add support to save Vterm window size. [lcgamboa]

* Add time tolerance to reset pulse of DHTxx sensors. [lcgamboa]

* Change vterm font to monospaced. [lcgamboa]

* Rename speed label from Clk to IO in qemu boards. [lcgamboa]

* Draw the spare part before clicking in the window when a new part is added. [lcgamboa]

* Scripts updated and max flash file size fixed for qemu. [lcgamboa]

* Add support to change ambient temperature and LM35 voltage offset in TempSys spare part. [lcgamboa]

### Fix

* Fix invalid serial read on qemu based boards. [lcgamboa]

* Fix I2C address of adxl345 and bmp280. [lcgamboa]

* Remove simulation reset when minimize/maximize on Windows. [lcgamboa]

* Fix SDCard spare parts CIPO signal. [lcgamboa]

* Fix invalid serial handle in Windows. [lcgamboa]

* Fixes the issue of opening multiple workspaces in multiple instances simultaneously. [lcgamboa]

* Fix Arduino Uno board missing spare parts when open workspace files. [lcgamboa]

### Other

* Version 0.9.1. [lcgamboa]

* Version 0.9.0. [lcgamboa]


## v0.9.0 (2023-07-29)

### New

* Move Curiosity, Curiosity HPC and Xpress boards from experimental to stable version. [lcgamboa]

* Add support to PIC16F819, PIC16F1827 and PIC16F1847 in Breadboard. [lcgamboa]

* Add support to Text Box part open external URLs on click. [lcgamboa]

* Add suport to ESP32-C3 to use efuse file. [lcgamboa]

* Move RemoteTCP board from experimental to stable version. [lcgamboa]

* Add support to use efuse file with ESP32 qemu based boards. [lcgamboa]

* Add support to IO virtual term part autodetect uart pins. [lcgamboa]

* New board ESP32-C3-DevKitC-02 (riscv32) [lcgamboa]

* New part NTC temperature sensor. [lcgamboa]

* Support to PIC16F688 added to board Breadboard. [lcgamboa]

* Support to UART, SPI, I2C and ADC added to RemoteTCP board. [lcgamboa]

* Add support to syncronize Remote TCP simulation. [lcgamboa]

* New part HX711 (Load Cell Amp) [lcgamboa]

* Option to set the number of elements in parts: Potentiometers, Push Buttons, Switches and LEDs. [lcgamboa]

* Option to set the number of elements in parts: Potentiometers, Push Buttons, Switches and LEDs. [lcgamboa]

* Support to PIC16F1947 added to Breadboard board. [lcgamboa]

* Support to PIC16F1516 and PIC1824Q10 added to Breadboard board. [lcgamboa]

* Support to save debug status per board. [lcgamboa]

* Support to PIC16F1829 added to Breadboard board. [lcgamboa]

* Support to save Board and Oscilloscope windows postions and options per board. [lcgamboa]

* Support to save Spare Parts window postion per board. [lcgamboa]

* Support to enable/disable reset by DTR/RTS serial signals. [lcgamboa]

### Changes

* Add support to disable default serial port on Qemu based boards. [lcgamboa]

* Add stack trace to log when PICSimLab crash. [lcgamboa]

* Add support to use icount=auto in qemu based boards. [lcgamboa]

* Add option to enable ethernet support in ESP32 DevKitC board. [lcgamboa]

* Qemu-ESP32 updated to support https over Wifi. [lcgamboa]

* Icons updated. [lcgamboa]

* Support to load only app.bin in qemu-esp32 based boards. [lcgamboa]

* Add the "Load Board Demo" menu option and automatic demo load on board first use. [lcgamboa]

* Add support to remove VT100 codes in Virtual Term part. [lcgamboa]

### Fix

* Fix button read after pin mode set to input in McLab2 board. [lcgamboa]

* Add line break in Virtual Term to avoid UI freezing. [lcgamboa]

* Add 2k text limit to virtual_Text part. [lcgamboa]

* Fix 7s Display part draw when active mode is set to low. [lcgamboa]

* Fix PQDB board seven segments display point and analog channel order. [lcgamboa]

* Fix crash in rcontrol when use PinViewer pin toggle on qemu based boards. [lcgamboa]

* Fix breadboard freezing when using File Load Hex menu. [lcgamboa]

* Fix serial port baudrate calculation. [lcgamboa]

* Fix the path of last open file for qemu based boards used by reload last menu option. [lcgamboa]

* Fix pin initial value used by LEDs and 7 seg. displays. [lcgamboa]

* Fix qemu based boards to not freeze when clock is changed. [lcgamboa]

* Fix TextBox init sporadic error messages. [lcgamboa]

* Fix Arduino Nano board power and uart pins. [lcgamboa]

* Add support to use atmega2560 four UARTs. [lcgamboa]

### Other

* Update simulated PQDB board to work like the real one, added support for PIC18F4550 and PIC18F4620. [lcgamboa]

* Merge pull request #76 from necrashter/update-gitignore. [lcgamboa]

* Add *.o and other files generated after compiling to gitignore. [necrashter]

* Merge pull request #70 from fathonix/master. [lcgamboa]

* Add .gitignore to prevent accidentally committing the build folder. [Aldo Adirajasa Fathoni]


## v0.8.12 (2023-01-20)

### New

* Version 0.8.12. [lcgamboa]

* Support to PIC16F886 and PIC18F28K80 added to Breadboard board. [lcgamboa]

* Support to use UART pins with qemu ESP32 and STM32 added. [lcgamboa]

* Support to use Virtual Terminal part with rcontrol interface. [lcgamboa]

* New part ADXL345 Accelerometer. [lcgamboa]

* Support to SPI in bmp280 sensor added. [lcgamboa]

* Support to SPI in qemu-stm32 added. [lcgamboa]

* Support to I2C in qemu-stm32 added. [lcgamboa]

* Add support to PIC18F4580 on boards Breadboard, McLAb2 and  PICGenios. [lcgamboa]

* New board Arduino Mega added. [lcgamboa]

* New board Arduino Nano added. [lcgamboa]

* Support to run multiple instances of PICSimLab. [lcgamboa]

* Support for logging and recovering from errors. [lcgamboa]

* Support to ESP32 Iomux on board DevKitC. [lcgamboa]

* Added support to I2C and SPI to ESP32 DevKitC board. [lcgamboa]

* Esp32 DevKitC board support to ADC added. [lcgamboa]

### Changes

* Change VCD Dump part to only show used channels in pulseview. [lcgamboa]

* Add instance numbers to windows title bars. [lcgamboa]

* Step motor part now show steps and turns. [lcgamboa]

* Scripts updated. [lcgamboa]

* README updated. [lcgamboa]

### Fix

* Fix Virtual terminal and increase internal timers precision. [lcgamboa]

* Fix window redraw when used in wayland (Linux) [lcgamboa]

* Add support to windows serial ports greater than COM9 in qemu backend simulator. [lcgamboa]

* PICGenious board interference of keys bounce on keyboard activation removed. [lcgamboa]


## v0.8.11 (2022-08-02)

### New

* New part BMP280 I2C pressure and temperature sensor. [lcgamboa]

* New part BMP180 I2C pressure and temperature sensor. [lcgamboa]

* New part DS18B20 1-wire temperature sensor. [lcgamboa]

* New part DHT11 humidity and temperature sensor. [lcgamboa]

* New part DHT22 humidity and temperature sensor. [lcgamboa]

* New board ESP32-DevKitC. [lcgamboa]

* Support to disable bounce effect in switches and push buttons parts. [lcgamboa]

* Part IO MM74C922 keyboard controller added. [lcgamboa]

* Version 0.8.10. [lcgamboa]

### Changes

* Move boards Blue Pill, DevKitC and STM32 H103 from experimental to stable version. [lcgamboa]

* Move gpboard from experimental to stable version. [lcgamboa]

* Blue Pill and STM32 H103 boards picture updated. [lcgamboa]

* PQDB and ESP32-DevKitC boards pictures updated. [lcgamboa]

* Scripts updated. [lcgamboa]

* Readme updated. [lcgamboa]

* Readme updated. [lcgamboa]

### Fix

* Fix qemu configuration window of board ESP32 DEvKitC. [lcgamboa]

* Fix ESP32 DevKitC to work on windows. [lcgamboa]

* Fix board PICGenios dip switches functions. [lcgamboa]

### Other

* Merge pull request #61 from lcgamboa/libqemu. [lcgamboa]


## v0.8.10 (2022-04-09)

### New

* Version 0.8.10. [lcgamboa]

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

* New part DS1621 I2C temperature sensor. [lcgamboa]

* New part DS1621 I2C temperature sensor. [lcgamboa]

* Support to add combined part LCD_hd44780 with PCF8574 I2C expander. [lcgamboa]

* Support to ATtiny85 USI TWI master mode. [lcgamboa]

* Push buttons bounce effect added. [lcgamboa]

* Oscilloscope support to show measures in screen added. [lcgamboa]

### Changes

* Use system default application to open the serial terminal. [lcgamboa]

* Disable autosave workspace for online version. [lcgamboa]

* IO PCF8574 I2C expander support to read pins added. [lcgamboa]

* Debian changelog updated. [lcgamboa]

* Use default application to open VCD files in VCDDump part. [lcgamboa]

* Appimage scripts updated. [lcgamboa]

* Build scripts updated. [lcgamboa]

* Move uCboard from experimental to stable version. [lcgamboa]

* Move uCboard from experimental to stable version. [lcgamboa]

* Makefiles updated! [lcgamboa]

* Speed optimization to only process parts when io is updated (only PICSim and simavr backend) [lcgamboa]

* Speed optimization to only process parts when io is updated (only simavr) [lcgamboa]

### Fix

* Remove simavr external interrupt aways enabled with slow down simulation. [lcgamboa]

* Save external EEPROM from boards to workspace files. [lcgamboa]

* Bitbang I2C ACK fixed for all I2C parts. [lcgamboa]

* Issue with device 74xx595 serial ouptut fixed. [lcgamboa]

* Fixes month writing issue in RTCs. [lcgamboa]

* Windows build. [lcgamboa]

* Fix power off statusbar messages. [lcgamboa]

* Board McLab2 7 segments display bug removed. [lcgamboa]

* IO Uart part slow simulation bug removed. [lcgamboa]

* Signal Generator part frequency shift error removed. [lcgamboa]

* Removed the bug when all pins of step motor part are zero. [lcgamboa]

### Other

* VERSION date updated. [lcgamboa]


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


