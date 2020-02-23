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

override CFLAGS += -D_VERSION_=\"$(VERSION)\" -Wall -O2 -D_SHARE_=\"${sharedir}\"   -fomit-frame-pointer  `lxrad-config --cxxflags`

#override CFLAGS+=-fsanitize=address
#CFLAGS+= -D_EXPERIMENTAL_

LIBS = `lxrad-config --libs` -lpicsim -lsimavr -lelf
#LIBS= `wx-config --libs` ../../picsim/src/libpicsim.a ../../LXRAD_WX/libteste/liblxrad.a  ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a 

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o picsimlab4.o picsimlab5.o\
 boards/board.o boards/boards_defs.o boards/board_pic.o boards/board_avr.o boards/board_avr_serial.o\
 boards/board_1.o boards/board_2.o boards/board_3.o boards/board_4.o boards/board_5.o boards/board_6.o\
 boards/board_7.o boards/board_x.o boards/board_0.o\
 devices/lcd_hd44780.o devices/mi2c_24CXXX.o devices/rtc_ds1307.o devices/rtc_pfc8563.o devices/lcd_pcf8833.o\
 devices/lcd_pcd8544.o devices/mplabxd.o devices/ldd_max72xx.o devices/io_74xx595.o\
 parts/parts_defs.o parts/part.o parts/part_servo.o parts/part_step.o parts/part_d_transfer_function.o\
 parts/part_LEDs.o parts/part_push_buttons.o parts/part_switchs.o parts/part_pot.o parts/part_RGB_LED.o\
 parts/part_LCD_hd44780.o parts/part_LCD_pcf8833.o parts/part_LCD_pcd8544.o parts/part_gamepad.o\
 parts/part_LED_matrix.o parts/part_7s_Display.o parts/part_TempSys.o parts/part_keypad.o \
 parts/part_MI2C_24CXXX.o parts/part_RTC_ds1307.o parts/part_RTC_pfc8563.o parts/part_IO_74xx595.o \
 parts/part_VCD_Dump.o

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
	xdg-icon-resource install --context mimetypes --size 48 ../share/application-x-picsimlab-workspace.png application-x-picsimlab-workspace
	xdg-mime install --mode system ../share/application-x-picsimlab-workspace.xml
	xdg-mime default picsimlab.desktop application/x-picsimlab-workspace
	update-mime-database /usr/share/mime &
	update-desktop-database ${appdir} &

uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop
	xdg-icon-resource uninstall  --context mimetypes --size 48 application-x-picsimlab-workspace
	xdg-mime uninstall  --mode system ../share/application-x-picsimlab-workspace.xml

run: all
	./picsimlab

clean:
	$(RM) picsimlab *.o core */*.o 
