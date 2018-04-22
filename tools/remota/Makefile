CC = g++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

FLAGS = -Wall -O3 `lxrad-config --cxxflags`
#FLAGS = -I/usr/include -Wall -ggdb -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -I/usr/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread

LIBS = `lxrad-config --libs`  
#LIBS = `wx-config --libs`  

OBJS = premota.o remota1.o  serial.o 
#OBJS = premota.o remota1.o  serial.o ../LXRAD_WX/libteste/liblxrad.a

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -oremota $(LIBS)

%.o: %.cc
	$(CC) -c $(FLAGS) $< 

run: all
	./remota

install:
	$(CP) -dvf remota ${execdir}

uninstall:
	$(RM) -dvf ${execdir}remota


clean:
	rm -f remota *.o core
