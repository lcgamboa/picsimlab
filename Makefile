CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

FLAGS = -D_VERSION_=\"0.6.0\" -Wall -O3 -D_SHARE_=\"${sharedir}\"   \
 -fomit-frame-pointer `lxrad-config --cxxflags`

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o board.o \
 lcd.o mi2c.o rtc.o rtc2.o prog_psp.o board_1.o board_2.o board_3.o\
 board_4.o mplabxd.o board_x.o


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -opicsimlab -lpicsim `lxrad-config --libs` 

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

install: all
	${MKDIR} ${sharedir}
	$(CP) -dvf share/picsimlab.desktop ${appdir} 
	$(CP) -drvf share/* ${sharedir} 
	$(CP) -dvf picsimlab ${execdir}

uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop



clean:
	$(RM) picsimlab *.o core
