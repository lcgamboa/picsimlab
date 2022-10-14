CC = g++
FLAGS = -Wall -O2 `lxrad_NOGUI-config --cxxflags`

OBJS = pLCDfont.o LCDfont1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -oLCDfont `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./LCDfont

install:

clean:
	rm -f LCDfont *.o core
