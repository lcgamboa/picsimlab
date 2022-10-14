CC = g++
FLAGS = -Wall -O2 `lxrad_NOGUI-config --cxxflags`

OBJS = pimageview.o imageview1.o  


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -oimageview `lxrad_NOGUI-config --libs`

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./imageview

install:

clean:
	rm -f imageview *.o core
