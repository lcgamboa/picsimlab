
[![Linux release](https://github.com/lcgamboa/picsimlab/actions/workflows/linux-release.yml/badge.svg)](https://github.com/lcgamboa/picsimlab/actions/workflows/linux-release.yml)
[![Windows release](https://github.com/lcgamboa/picsimlab/actions/workflows/windows-release.yml/badge.svg)](https://github.com/lcgamboa/picsimlab/actions/workflows/windows-release.yml)

# PICSimLab - Programmable IC Simulator Laboratory

PICSimLab is a realtime emulator of [development boards](https://lcgamboa.github.io/picsimlab_docs/stable/Boards.html) with MPLABX/avr-gdb debugger integration. 
PICSimLab supports microcontrollers from [picsim](https://github.com/lcgamboa/picsim), [simavr](https://github.com/buserror/simavr), [uCsim](http://mazsola.iit.uni-miskolc.hu/~drdani/embedded/ucsim/), [qemu-stm32](http://beckus.github.io/qemu_stm32/), [qemu-esp32](https://github.com/a159x36/qemu), and [gpsim](http://gpsim.sourceforge.net/).
PICSimLab has integration with MPLABX/Arduino IDE for programming the microcontroller's boards.
As the purpose of PICSimLab is to emulate real hardware it does not have any source code editing support.
For code editing and debugging the same tools used for a real board should be used with PICSimLab, such as MPLABX, Arduino IDE, or VSCode with PlatformIO.

PICSimLab supports several devices (spare parts) that can be connected to the boards for simulation. 
Such as example LEDs and push buttons for simple outputs and inputs and some more complex ones like the ethernet 
shield w5500 for internet connection or the color graphic display ili9340 with touchscreen. 
The complete list of parts can be accessed in the [documentation](https://lcgamboa.github.io/picsimlab_docs/stable/SpareParts.html). 
 


![PICsimLab](https://lcgamboa.github.io/picsimlab_docs/gifs/analog.gif "PICsimLab")

![PICsimLab](https://lcgamboa.github.io/picsimlab_docs/gifs/main.gif "PICsimLab")

## More info

### For users

[Online Documentation](https://lcgamboa.github.io/picsimlab_docs/stable/)

[Examples](https://lcgamboa.github.io/picsimlab_examples/examples_index.html)

[Online version of PICSimLab](https://lcgamboa.github.io/picsimlab_docs/stable/OnlineSimulator.html)

[Changelog](./CHANGELOG.md)

[PICSimLab on Twitter](https://twitter.com/PICSimLab)

[PICSimLab on Discord](https://discord.com/invite/fMT8szFYq7)

### For developers

[Development Documentation](https://lcgamboa.github.io/picsimlab_docs/devel/html/)

[TODO list](./TODO.md)

## Stable version executables download 

If you are on Linux or Windows you can download the last version at:

https://github.com/lcgamboa/picsimlab/releases

If you are on macOS you can run PICSimLab using Wine:

- Download and install [`xquartz`](https://www.xquartz.org)
- Download and install [Wine](https://dl.winehq.org/wine-builds/macosx/download.html)
- Download the executable and double-click it to run the installer

## Unstable last code version executables download 

The binaries of last code available on github can be downloaded at: [Latest code build (unstable) release](https://github.com/lcgamboa/picsimlab/releases/tag/latestbuild)
 
The unstable test version have the unreleased features of [Changelog_auto.md](./CHANGELOG_auto.md)

If you need a specific binary that is not available please contact me. 

## Install from source

### In Debian Linux and derivatives Linux native:

**Using a user with permission to run the sudo command:**

In first time build:
```
git clone --depth=1 https://github.com/lcgamboa/picsimlab.git
cd picsimlab
bscripts/build_all_and_install.sh
```

To recompile use:
```
make -j$(nproc)
```

### Cross-compiling for Windows (from Linux or [WSL](https://docs.microsoft.com/windows/wsl/install-win10) on win10)

In first time build in Debian Linux and derivatives target Windows 64 bits:

```
git clone https://github.com/lcgamboa/picsimlab.git
cd picsimlab
bscripts/build_w64.sh
```
To recompile use:
```
make FILE=Makefile.cross -j$(nproc) 
```

For target Windows 32 bits:

```
git clone https://github.com/lcgamboa/picsimlab.git
cd picsimlab
bscripts/build_w32.sh
```
To recompile use:
```
make FILE=Makefile.cross_32 -j$(nproc) 
```

### Experimental version

Experimental version can be built using the parameter "exp" on scripts:
```
bscripts/build_all_and_install.sh exp
bscripts/build_w64.sh exp
bscripts/build_w32.sh exp
```
And recompiled using the parameter "exp" on Makefiles:
```
make -j$(nproc) exp
make FILE=Makefile.cross -j$(nproc)  exp
make FILE=Makefile.cross_32 -j$(nproc) exp
```

### macOS from source

Theoretically it is possible to compile PICSimLab natively on macOS. But I do not have access to any computer
 with macOS to try to compile and until today nobody has communicated that they managed to do it. (help wanted) 


## Troubleshooting:
The simulation in PICSimLab consists of 3 parts:

- The microcontroller program
- Microcontroller simulation (made by [picsim](https://github.com/lcgamboa/picsim) and [simavr](https://github.com/buserror/simavr))
- Simulation of boards and parts

When a problem occurs it is important to detect where it is occurring.

One of the most common problems is the error in the microcontroller program. Before creating an issue, test your code on a real circuit (even partially) to make sure the problem is not there.

Errors in the microcontroller simulation can be detected using code debugging. Any instruction execution or peripheral behavior outside the expected should be reported in the project of simulator used ([picsim](https://github.com/lcgamboa/picsim) or [simavr](https://github.com/buserror/simavr)).

If the problem is not in either of the previous two options, the problem is probably in PICSimLab. A good practice is to send a source code together with a PICSimLab workspace (.pzw file) to open the issue about the problem.


## Libraries and projects used in PICSimLab (all free software):

### Graphic interface

- **wxwidgets** 	- For graphic suport		   
  - http://www.wxwidgets.org

- **lxrad**           - Graphic library                  
  - https://github.com/lcgamboa/lxrad 

- **lunasvg**         - Support to use SVG               
  - https://github.com/sammycage/lunasvg

### Microcontroller simulation

- **picsim**          - PIC simulator                    
  - https://github.com/lcgamboa/picsim 

- **simavr**          - AVR simulator                    
  - original: https://github.com/buserror/simavr 						   
  - modified version used by PICSimLab: https://github.com/lcgamboa/simavr 

- **ucsim**           - 8051, STM8 and Z80 simulator     
  - original: http://mazsola.iit.uni-miskolc.hu/~drdani/embedded/ucsim/
  - modified version used by PICSimLab: https://github.com/lcgamboa/uCsim_picsimlab

- **qemu-stm32**      - STM32 simulator                  
  - original: https://github.com/beckus/qemu_stm32/
  - modified version used by PICSimLab: https://github.com/lcgamboa/qemu/tree/picsimlab-stm32

- **qemu-esp32**      - ESP32 simulator                  
  - original: https://github.com/a159x36/qemu
  - modified version used by PICSimLab: https://github.com/lcgamboa/qemu/tree/picsimlab-esp32

- **gpsim**           - PIC simulator                    
  - http://gpsim.sourceforge.net/

### Data visualization

- **gtkwave**         - Wave viewer                      
  - http://gtkwave.sourceforge.net/

- **Pulseview**       - Logic analyzer (with protocol decoder support)and MSO GUI for sigrok
  - https://sigrok.org/wiki/PulseView

### Graphic editing

- **gimp**		- pictures and img maps edition    
  - https://www.gimp.org/

- **inkscape**	- SVG pictures edition		   
  - https://inkscape.org/

- **pcbdraw**		- To convert kicad pcb to SVG      
  - https://github.com/yaqwsx/PcbDraw

### Circuit design

- **kicad**		- To design circuits and pcbs      
  - https://kicad.org/

## Utils:

- **picsimlab_md**      - for debug with MPLABX      
  - https://github.com/lcgamboa/picsimlab_md

- **com0com**		- For serial emulation in windows  
  - http://com0com.sourceforge.net/ 

- **tty0tty**   	- For serial emulation in linux    
  - https://github.com/lcgamboa/tty0tty 

## Download analytics

- GitHub [`lcgamboa/picsimlab`](https://github.com/lcgamboa/picsimlab/) repo
  - latest PICSimLab release
[![Github Latest Release](https://img.shields.io/github/downloads/lcgamboa/picsimlab/latest/total.svg)](https://github.com/lcgamboa/picsimlab/releases/)
  - all PICSimLab releases [![Github All Releases](https://img.shields.io/github/downloads/lcgamboa/picsimlab/total.svg)](https://github.com/lcgamboa/picsimlab/releases/)
  - [individual file counters](https://somsubhra.github.io/github-release-stats/?username=lcgamboa&repository=picsimlab) (grouped per release)

- SourceForge [`picsimlab`](https://sourceforge.net/projects/picsim/) repo
  - latest PICSimLab release [![Sourceforge Latest Relesases](https://img.shields.io/sourceforge/dt/picsim/v0.9.2)](https://sourceforge.net/projects/picsim/files/v0.9.2/)
  - all PICSimLab releases [![Sourceforge All Relesases](https://img.shields.io/sourceforge/dt/picsim)](https://sourceforge.net/projects/picsim/files/)

Credit to [Shields IO](https://shields.io) for the badges and to
[Somsubhra/github-release-stats](https://github.com/Somsubhra/github-release-stats)
for the individual file counters.


---
In papers and reports, please refer to PICSimLab as follows: 'L. C. G. Lopes. PICsimLab - Programmable IC Simulator Laboratory . https://github.com/lcgamboa/picsimlab', e.g. using the following BibTeX code:
```
@MISC{PICSimLab,
	author = {L. C. G. Lopes},
	title = {PICsimLab - Programmable IC Simulator Laboratory },
	howpublished = "\url{https://github.com/lcgamboa/picsimLab}"
}

```

