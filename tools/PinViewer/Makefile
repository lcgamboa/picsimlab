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
FLAGS+= -D_ARCH_=\"Linux64_WX\" -D_DATE_=\"${DATE}\" 

LIBS = `lxrad-config --libs`  

OBJS = pPinViewer.o PinViewer1.o 

#lxrad automatic generated block end, don't edit above!

exp: all

check-lxrad-config:
	@type lxrad-config >/dev/null 2>&1 || { echo "Cannot find lxrad-config"; exit 1; }
	@touch $@

all: $(OBJS) check-lxrad-config
	@echo "Linking PinViewer"
	@$(CC) $(FLAGS) $(LDFLAGS) $(OBJS) -oPinViewer $(LIBS)

%.o: %.cc check-lxrad-config
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./PinViewer

install_app: all
	install -d ${execdir}
	install -m 755 PinViewer ${execdir}

install: install_app
ifneq ($(shell uname),Darwin)
	install -d ${appdir}
	install -m 644 ../../share/PinViewer.desktop ${appdir}
endif

uninstall:
	$(RM) -dvf ${execdir}PinViewer
ifneq ($(shell uname),Darwin)
	$(RM) -dvf ${appdir}PinViewer.desktop
endif

clean:
	rm -f PinViewer *.o core check-lxrad-config
