CC = g++
FLAGS = -Wall -O0 -ggdb `lxrad_NOGUI-config --cxxflags`

OBJS = pclock.o clock1.o  


all: $(OBJS)
	#$(CC) $(FLAGS) $(OBJS) -oclock `lxrad_NOGUI-config --libs`
	$(CC) $(FLAGS) $(OBJS) -oclock ../../lib/liblxrad_NOGUI.a -lminizip -lpthread

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./clock

install:

clean:
	rm -f clock *.o core
