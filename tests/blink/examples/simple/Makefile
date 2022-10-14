CC = g++
FLAGS = -Wall -O0 -g `lxrad_NOGUI-config --cxxflags`

OBJS = psimple.o simple1.o  


all: $(OBJS)
	#$(CC) $(FLAGS) $(OBJS) -osimple `lxrad_NOGUI-config --libs`
	$(CC) $(FLAGS) $(OBJS) -osimple ../../lib/liblxrad_NOGUI.a -lminizip -lpthread

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./simple

install:

clean:
	rm -f simple *.o core
