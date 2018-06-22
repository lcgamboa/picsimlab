include ../Makefile.conf

CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

override CFLAGS += -D_VERSION_=\"$(VERSION)\" -Wall -O3 -D_SHARE_=\"${sharedir}\"   -fomit-frame-pointer  `lxrad-config --cxxflags`
#CFLAGS = -D_VERSION_=\"0.7.0\" -Wall -ggdb -D_SHARE_=\"${sharedir}\"   -fomit-frame-pointer  `lxrad-config --cxxflags`

#CFLAGS+= -D_EXPERIMENTAL_

LIBS = `lxrad-config --libs` -lpicsim -lsimavr -lelf
#LIBS= `wx-config --libs` ../../picsim/src/libpicsim.a ../../LXRAD_WX/libteste/liblxrad.a  ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a 

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o picsimlab4.o picsimlab5.o futils.o boards/board.o boards/boards_defs.o boards/board_pic.o boards/board_avr.o boards/board_avr_serial.o devices/lcd_hd44780.o devices/mi2c_24CXXX.o devices/rtc_ds1307.o devices/rtc_pfc8563.o devices/lcd_pcf8833.o devices/lcd_pcd8544.o boards/board_1.o boards/board_2.o boards/board_3.o boards/board_4.o boards/board_5.o boards/board_6.o boards/board_7.o devices/mplabxd.o boards/board_x.o parts/parts_defs.o parts/part.o parts/part_servo.o parts/part_step.o parts/part_d_transfer_function.o parts/part_LEDs.o parts/part_push_buttons.o parts/part_switchs.o parts/part_pot.o parts/part_RGB_LED.o parts/part_LCD_hd44780.o parts/part_LCD_pcf8833.o parts/part_LCD_pcd8544.o

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -opicsimlab $(LIBS) 

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@ 

install: all
	${MKDIR} ${sharedir}
	$(CP) -dvf  ../share/picsimlab.desktop ${appdir} 
	$(CP) -drvf ../share/* ${sharedir} 
	$(CP) -drvf ../docs ${sharedir} 
	$(CP) -dvf picsimlab ${execdir}

uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop

run: all
	./picsimlab

clean:
	$(RM) picsimlab *.o core */*.o 
