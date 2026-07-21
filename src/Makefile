include Makefile.Common

CXX= g++

PKG_CONFIG= `pkg-config --cflags glib-2.0` 

DESTDIR ?= /usr
prefix = $(DESTDIR)

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
libdir= ${prefix}/lib/picsimlab/
execdir= ${prefix}/bin/

# Suppose lxrad-config may be installed locally in DESTDIR
export PATH := $(abspath ${execdir}):$(PATH)

#lxrad automatic generated block start, don't edit below!

override CXXFLAGS+= -D_ARCH_=\"Linux64_WX\" -D_DATE_=\"${DATE}\"
override CXXFLAGS+= -D_VERSION_=\"$(VERSION)\" -Wall -g -O2 -D_SHARE_=\"../share/picsimlab/\" \
-D_LIB_=\"../lib/picsimlab/\" `lxrad-config --cxxflags`
override CXXFLAGS+= -DEXT_BROWSER
override CXXFLAGS+= -I${prefix}/include -L${prefix}/lib
#override CXXFLAGS+=-fsanitize=address
#override CXXFLAGS+=-fno-omit-frame-pointer

LIBS =  `lxrad-config --libs` -lpicsim -lsimavr -lelf $(ELIBS) -lucsim -ldl -lgpsim -lcurl
#LIBS=  ../../picsim/src/libpicsim_dbg.a ../../LXRAD_WX/libteste/liblxrad.a  ../../lunasvg/build/liblunasvg.a \
     ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a  -lopenal `wx-config --libs` `wx-config --libs stc` \
     -ldl -lgpsim

#lxrad automatic generated block end, don't edit above!

check-lxrad-config:
	@type lxrad-config >/dev/null 2>&1 || { echo "Cannot find lxrad-config"; exit 1; }
	@touch $@

all: picsimlab
	
picsimlab: $(OBJS) check-lxrad-config
	@echo "Linking picsimlab"
	@$(CXX) $(CXXFLAGS) $(OBJS) -opicsimlab $(LIBS) 

%.o: %.cc check-lxrad-config
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

install_app: all
	install -d $(execdir) ${libdir}
	install -m 755 picsimlab $(execdir)
	strip ${execdir}picsimlab
	$(CP) -PRvf ../lib/* ${libdir}
ifneq ($(shell uname),Darwin)
	install -d  $(sharedir) $(appdir) ${sharedir}/../icons/hicolor/64x64/apps/ \
	${sharedir}/../icons/hicolor/64x64/mimetypes/ ${sharedir}/../mime/application/
	$(CP) -Pvf  ../share/picsimlab.desktop ${appdir}/
	$(CP) -PRvf ../share/* ${sharedir}
	$(CP) -PRvf ../share/picsimlab.png ${sharedir}/../icons/hicolor/64x64/apps/
	$(CP) -PRvf ../share/application-x-picsimlab-workspace.png  ${sharedir}/../icons/hicolor/64x64/mimetypes/
	$(CP) -PRvf ../share/application-x-picsimlab-workspace.xml ${sharedir}/../mime/application/x-picsimlab-workspace.xml
endif

install: install_app
ifneq ($(shell uname),Darwin)
	xdg-icon-resource install --size 64  --novendor ../share/picsimlab.png 
	xdg-icon-resource install --context mimetypes --size 64 ../share/application-x-picsimlab-workspace.png application-x-picsimlab-workspace
	#no need for --mode system, xdg-mime will install to system if executed under root
	xdg-mime install ../share/application-x-picsimlab-workspace.xml
	xdg-mime default picsimlab.desktop application/x-picsimlab-workspace
	update-mime-database /usr/share/mime &
	update-desktop-database ${appdir} &
endif

install_docs:
	$(CP) -PRvf ../docs ${sharedir}
		

uninstall:
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -drvf ${libdir}
ifneq ($(shell uname),Darwin)
	$(RM) -drvf ${sharedir}
	$(RM) -dvf ${appdir}picsimlab.desktop
	xdg-icon-resource uninstall  --size 64 --novendor picsimlab
	xdg-icon-resource uninstall  --context mimetypes --size 64 application-x-picsimlab-workspace
	xdg-mime uninstall  --mode system ../share/application-x-picsimlab-workspace.xml
endif

run: all
	./picsimlab

clean:
	$(RM) picsimlab *.o core */*.o check-lxrad-config
