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

ifeq ($(shell uname -s),Darwin)
    LDFLAGS := -Wl,-rpath,@executable_path/../lib
else
    LDFLAGS :=
endif

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
	@$(CC) $(FLAGS) $(LDFLAGS) $(OBJS) -osrtank $(LIBS)

%.o: %.cc check-lxrad-config
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./srtank

install_app: all
	install -d ${execdir}
	install -m 755 srtank ${execdir}

install: install_app
ifneq ($(shell uname),Darwin)
	install -d ${appdir}
	install -m 644 ../../share/srtank.desktop ${appdir}
endif

uninstall:
	$(RM) -dvf ${execdir}srtank
ifneq ($(shell uname),Darwin)
	$(RM) -dvf ${appdir}srtank.desktop
endif

clean:
	rm -f srtank *.o core check-lxrad-config
