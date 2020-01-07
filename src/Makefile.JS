include ../Makefile.conf

CC = em++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

CFLAGS= --pre-js load.js -D_ONEWIN -DEXT_BROWSER -DNO_DEBUG -DNO_TOOLS -Werror -s WASM=1 -Os -s DEMANGLE_SUPPORT=0 -s "BINARYEN_TRAP_MODE='clamp'" -s SAFE_HEAP=0 -s ASSERTIONS=0 -s TOTAL_MEMORY=128MB -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png","bmp"]' -s USE_SDL_TTF=2 --preload-file assets --preload-file home -D_VERSION_=\"0.7.0\" -Wall -D_SHARE_=\"/assets/share/\" -I../../LXRAD_SDL2/include/ -I../../picsim/include/ -I../../simavr/simavr/   

#CFLAGS+=--emrun

#CFLAGS+= -D_EXPERIMENTAL_

LIBS= ../../LXRAD_SDL2/lib/liblxrad_SDL2.a ../../picsim/src/libpicsim.a ../../simavr/simavr/obj-asmjs-unknown-emscripten/libsimavr.a
LIBS+= ../../LXRAD_SDL2/teste/zlib-master/contrib/minizip/minizip.a

#LIBS = `lxrad_SDL2-config --libs` -lpicsim -lsimavr -lelf

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o picsimlab4.o picsimlab5.o\
 boards/board.o boards/boards_defs.o boards/board_pic.o boards/board_avr.o boards/board_avr_serial.o\
 boards/board_1.o boards/board_2.o boards/board_3.o boards/board_4.o boards/board_5.o boards/board_6.o\
 boards/board_7.o boards/board_x.o boards/board_0.o\
 devices/lcd_hd44780.o devices/mi2c_24CXXX.o devices/rtc_ds1307.o devices/rtc_pfc8563.o devices/lcd_pcf8833.o\
 devices/lcd_pcd8544.o devices/mplabxd.o devices/ldd_max72xx.o\
 parts/parts_defs.o parts/part.o parts/part_servo.o parts/part_step.o parts/part_d_transfer_function.o\
 parts/part_LEDs.o parts/part_push_buttons.o parts/part_switchs.o parts/part_pot.o parts/part_RGB_LED.o\
 parts/part_LCD_hd44780.o parts/part_LCD_pcf8833.o parts/part_LCD_pcd8544.o parts/part_gamepad.o\
 parts/part_LED_matrix.o parts/part_7s_Display.o parts/part_TempSys.o parts/part_keypad.o \
 parts/part_MI2C_24CXXX.o parts/part_RTC_ds1307.o parts/part_RTC_pfc8563.o

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o picsimlab.html $(LIBS) 

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
	$(RM) picsimlab_SDL2 *.o core */*.o *.html picsimlab.js *.wasm *.map *.pre *.data *.wast
