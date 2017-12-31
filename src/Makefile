CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

FLAGS = -D_VERSION_=\"0.6.5\" -Wall -O2 -D_SHARE_=\"${sharedir}\"   -fomit-frame-pointer `lxrad-config --cxxflags` -I"../../simavr/simavr/sim/"


LIBS = `lxrad-config --libs` -lpicsim ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a 
#LIBS= `wx-config --libs` ../../picsim/src/libpicsim.a ../../LXRAD_WX/libteste/liblxrad.a  ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a 

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o picsimlab4.o picsimlab5.o board.o board_pic.o board_avr.o lcd_hd44780.o mi2c_24CXXX.o rtc_ds1307.o rtc_pfc8563.o board_1.o board_2.o board_3.o board_4.o board_5.o board_6.o board_7.o mplabxd.o board_x.o parts_defs.o part.o part_servo.o part_step.o part_d_transfer_function.o part_LEDs.o part_push_buttons.o part_switchs.o part_pot.o

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -opicsimlab $(LIBS) 

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

install: all
	${MKDIR} ${sharedir}
	$(CP) -dvf  ../share/picsimlab.desktop ${appdir} 
	$(CP) -drvf ../share/* ${sharedir} 
	$(CP) -dvf picsimlab ${execdir}

uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop

run: all
	./picsimlab

clean:
	$(RM) picsimlab *.o core 
