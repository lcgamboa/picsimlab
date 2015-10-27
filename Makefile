CC = g++
FLAGS = -D_VERSION_=\"0.6.0\" -Wall -O3 -D_SHARE_=\"`pwd`/share/\"   -fomit-frame-pointer `lxrad-config --cxxflags`

OBJS = ppicsimlab.o picsimlab1.o picsimlab2.o picsimlab3.o board.o lcd.o mi2c.o rtc.o rtc2.o -lpicsim prog_psp.o board_1.o board_2.o board_3.o board_4.o mplabxd.o


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -opicsimlab `lxrad-config --libs` 

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./picsimlab

install:

clean:
	rm -f picsimlab *.o core
