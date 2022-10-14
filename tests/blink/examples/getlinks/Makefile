CC = g++
FLAGS = -Wall -O2 `lxrad_NOGUI-config --cxxflags`

OBJS = pgetlinks.o getlinks1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -ogetlinks `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./getlinks

install:

clean:
	rm -f getlinks *.o core
