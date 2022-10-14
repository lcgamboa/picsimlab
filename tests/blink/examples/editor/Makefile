CC = g++
FLAGS = -Wall -O2 `lxrad_NOGUI-config --cxxflags`

OBJS = peditor.o editor1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -oeditor `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./editor

install:

clean:
	rm -f editor *.o core
