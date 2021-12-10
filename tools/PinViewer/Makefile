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

#lxrad automatic generated block start, don't edit below!

FLAGS =  -D_VERSION_=\"$(VERSION)\" -D_SHARE_=\"${sharedir}\"  -Wall -g -O2 `lxrad-config --cxxflags`
FLAGS+= -D_ARCH_=\"Linux64_WX\" -D_DATE_=\"${DATE}\" -flto

LIBS = `lxrad-config --libs`  

OBJS = pPinViewer.o PinViewer1.o 

#lxrad automatic generated block end, don't edit above!

exp: all

all: $(OBJS)
	@echo "Linking PinViewer"
	@$(CC) $(FLAGS) $(OBJS) -oPinViewer $(LIBS)

%.o: %.cc
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./PinViewer

install: all
	$(CP) -dvf  ../../share/PinViewer.desktop ${appdir} 
	$(CP) -dvf PinViewer ${execdir}

install_app: all
	$(CP) -dvf PinViewer ${execdir}

uninstall:
	$(RM) -dvf ${execdir}PinViewer
	$(RM) -dvf ${appdir}PinViewer.desktop


clean:
	rm -f PinViewer *.o core
