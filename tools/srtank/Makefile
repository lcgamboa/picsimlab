CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

FLAGS = -Wall -O2 `lxrad-config --cxxflags` -D_VERSION_=\"0.7.0\"

LIBS = `lxrad-config --libs`  

OBJS = psrtank.o srtank1.o  serial.o

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -osrtank $(LIBS)

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./srtank

install:
	$(CP) -dvf srtank ${execdir}

uninstall:
	$(RM) -dvf ${execdir}srtank


clean:
	rm -f srtank *.o core
