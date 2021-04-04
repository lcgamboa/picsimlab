include ../../VERSION

CC = g++

prefix=$(DESTDIR)/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
execdir= ${prefix}/bin/
sharedir= ${prefix}/share/picsimlab/

#lxrad automatic generated block start, don't edit below!

FLAGS =  -D_VERSION_=\"$(VERSION)\" -D_SHARE_=\"${sharedir}\"  -Wall -O2 `lxrad-config --cxxflags`
FLAGS+= -DEXT_BROWSER

LIBS = `lxrad-config --libs`  

OBJS = psrtank.o srtank1.o srtank2.o  serial.o

#lxrad automatic generated block end, don't edit above!

exp: all

all: $(OBJS)
	@echo "Linking srtank"
	@$(CC) $(FLAGS) $(OBJS) -osrtank $(LIBS)

%.o: %.cc
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./srtank

install: all
	$(CP) -dvf  ../../share/srtank.desktop ${appdir} 
	$(CP) -dvf srtank ${execdir}

install_app: all
	$(CP) -dvf srtank ${execdir}

uninstall:
	$(RM) -dvf ${execdir}srtank
	$(RM) -dvf ${appdir}srtank.desktop


clean:
	rm -f srtank *.o core
