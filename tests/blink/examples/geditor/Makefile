CC = g++
FLAGS = -Wall -O2 `lxrad_NOGUI-config --cxxflags`

OBJS = pgeditor.o geditor1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -ogeditor `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./geditor

install:

clean:
	rm -f geditor *.o core
