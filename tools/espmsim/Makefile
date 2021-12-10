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

FLAGS = -D_VERSION_=\"$(VERSION)\" -D_SHARE_=\"${sharedir}\" -Wall -g -O2 -DLINUX `lxrad-config --cxxflags`
FLAGS+= -DEXT_BROWSER -flto

LIBS = `lxrad-config --libs`  

OBJS = pespmsim.o espmsim1.o espmsim2.o serial.o tcp.o

#lxrad automatic generated block end, don't edit above!

exp: all
all: $(OBJS)
	@echo "Linking espmsim"
	@$(CC) $(FLAGS) $(OBJS) -oespmsim $(LIBS)

%.o: %.cc
	@echo "Compiling $<"
	@$(CC) -c $(FLAGS) $< 

run: all
	./espmsim

install: all
	$(CP) -dvf  ../../share/espmsim.desktop ${appdir} 
	$(CP) -dvf espmsim ${execdir}

install_app: all
	$(CP) -dvf espmsim ${execdir}

uninstall:
	$(RM) -dvf ${execdir}espmsim
	$(RM) -dvf ${appdir}espmsim.desktop

clean:
	rm -f espmsim *.o core
