
# PICSimLab - PIC Simulator Laboratory

PICSimLab is a realtime emulator of development boards with integrated MPLABX/avr-gdb debugger. 
PICSimLab supports picsim microcontrollers PIC16F628/16F777/16F877A/18F452/18F4550/18F4620 and simavr microcontroller ATMEGA328.
PICSimLab have integration with  MPLABX/Arduino IDE for programming the boards microcontrollers.

![PICsimLab](share/doc/screenshot.png?raw=true "PICsimLab")

![PICsimLab](share/doc/picsimlab4.png?raw=true "PICsimLab")

## Requirements for compiling:

wxwidget 	- For graphic suport		   http://www.wxwidgets.org

picsim          - PIC simulator                    https://github.com/lcgamboa/picsim 

lxrad           - Graphic library                  https://github.com/lcgamboa/lxrad 

simavr          - AVR simulator                    https://github.com/buserror/simavr

## Utils:

picsimlab_md    - for debug with MPLABX      https://github.com/lcgamboa/picsimlab_md

com0com		- For serial emulation in windows  http://com0com.sourceforge.net/ 

tty0tty 	- For serial emulation in linux    https://github.com/lcgamboa/tty0tty 

## Install from source

In Debian Linux and derivatives:

```
git clone https://github.com/lcgamboa/picsimlab.git
cd picsimlab
./picsimlab_build_all_and_deps.sh
```
