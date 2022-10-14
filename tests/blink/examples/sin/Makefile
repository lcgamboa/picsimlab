CC = g++
FLAGS = -Wall -O0 -ggdb  `lxrad_NOGUI-config --cxxflags`

OBJS = psin.o sin1.o  


all: $(OBJS)
	#$(CC) $(FLAGS) $(OBJS) -osin `lxrad_NOGUI-config --libs`
	$(CC) $(FLAGS) $(OBJS) -osin ../../lib/liblxrad_NOGUI.a -lminizip -lpthread

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./sin

install:

clean:
	rm -f sin *.o core
