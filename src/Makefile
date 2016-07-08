CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

FLAGS = -D_VERSION_=\"0.6.5\" -Wall -O3 -D_SHARE_=\"${sharedir}\"   \
 -fomit-frame-pointer `lxrad-config --cxxflags`

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o board.o \
 lcd_hd44780.o mi2c_24CXXX.o rtc_ds1307.o rtc_pfc8563.o board_1.o board_2.o board_3.o\
 board_4.o board_5.o board_6.o mplabxd.o board_x.o

LIBS = -lpicsim `lxrad-config --libs`

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



clean:
	$(RM) picsimlab *.o core
