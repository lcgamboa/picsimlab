CC = g++
FLAGS = -Wall -O5 `lxrad_NOGUI-config --cxxflags`

OBJS = pmotor.o motor1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -omotor `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./motor

install:

clean:
	rm -f motor *.o core
