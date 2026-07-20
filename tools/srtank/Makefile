include ../../VERSION

CC = g++

DESTDIR ?= /usr
prefix = $(DESTDIR)

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
execdir= ${prefix}/bin/
sharedir= ${prefix}/share/picsimlab/

# Suppose lxrad-config may be installed locally in DESTDIR
export PATH := $(abspath ${execdir}):$(PATH)

#lxrad automatic generated block start, don't edit below!

FLAGS =  -D_VERSION_=\"$(VERSION)\" -D_SHARE_=\"${sharedir}\"  -Wall -g -O2 `lxrad-config --cxxflags`
FLAGS+= -DEXT_BROWSER 

LIBS = `lxrad-config --libs`  

OBJS = psrtank.o srtank1.o srtank2.o  serial.o

#lxrad automatic generated block end, don't edit above!

exp: all

check-lxrad-config:
	@type lxrad-config >/dev/null 2>&1 || { echo "Cannot find lxrad-config"; exit 1; }
	@touch $@

all: $(OBJS) check-lxrad-config
	@echo "Linking srtank"
	@$(CC) $(FLAGS) $(OBJS) -osrtank $(LIBS)

%.o: %.cc check-lxrad-config
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
	rm -f srtank *.o core check-lxrad-config
