CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

FLAGS = -Wall -O2 -DLINUX `lxrad-config --cxxflags`

LIBS = `lxrad-config --libs`  
LIBS = `lxrad-config --libs`  

OBJS = pespmsim.o espmsim1.o serial.o tcp.o
OBJS = pespmsim.o espmsim1.o serial.o tcp.o

#lxrad automatic generated block end, don't edit above!


all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -oespmsim $(LIBS)

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./espmsim

install:
	$(CP) -dvf espmsim ${execdir}

uninstall:
	$(RM) -dvf ${execdir}espmsim

clean:
	rm -f espmsim *.o core
