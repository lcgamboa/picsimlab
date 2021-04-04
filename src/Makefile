include Makefile.Common

CXX= g++

PKG_CONFIG= `pkg-config --cflags glib-2.0` 

prefix=$(DESTDIR)/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

override CXXFLAGS+= -D_VERSION_=\"$(VERSION)\" -Wall -g -O2 -D_SHARE_=\"../share/picsimlab/\"  `lxrad-config --cxxflags`
override CXXFLAGS+= -DEXT_BROWSER
#override CXXFLAGS+=-fsanitize=address
override CXXFLAGS+= -D_ARCH_=\"Linux64_WX\" -D_DATE_=\"${DATE}\"

LIBS =  `lxrad-config --libs` -lpicsim -lsimavr -lelf $(ELIBS)
#LIBS=  ../../picsim/src/libpicsim_dbg.a ../../LXRAD_WX/libteste/liblxrad.a  ../../lunasvg/build/liblunasvg.a \
     ../../simavr/simavr/obj-x86_64-linux-gnu/libsimavr.a  -lopenal `wx-config --libs` `wx-config --libs stc` $(ELIBS)


#lxrad automatic generated block end, don't edit above!



all: picsimlab
	
picsimlab: $(OBJS)
	@echo "Linking picsimlab"
	@$(CXX) $(CXXFLAGS) $(OBJS) -opicsimlab $(LIBS) 

%.o: %.cc
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

install: all
	${MKDIR} ${sharedir}
	${MKDIR} ${sharedir}docs/
	$(CP) -dvf  ../share/picsimlab.desktop ${appdir} 
	$(CP) -drvf ../share/* ${sharedir}
	$(CP) -dvf picsimlab ${execdir}
	strip ${execdir}picsimlab	
	xdg-icon-resource install --size 64  --novendor ../share/picsimlab.png 
	xdg-icon-resource install --context mimetypes --size 64 ../share/application-x-picsimlab-workspace.png application-x-picsimlab-workspace
	xdg-mime install --mode system ../share/application-x-picsimlab-workspace.xml
	xdg-mime default picsimlab.desktop application/x-picsimlab-workspace
	update-mime-database /usr/share/mime &
	update-desktop-database ${appdir} &

install_app: all
	install -d $(execdir) $(sharedir) $(appdir) ${sharedir}/../icons/hicolor/64x64/apps/ \
	${sharedir}/../icons/hicolor/64x64/mimetypes/ ${sharedir}/../mime/application/
	$(CP) -dvf  ../share/picsimlab.desktop ${appdir}/
	$(CP) -drvf ../share/* ${sharedir} 
	$(CP) -dvf picsimlab ${execdir}
	strip ${execdir}picsimlab
	$(CP) -drvf ../share/picsimlab.png ${sharedir}/../icons/hicolor/64x64/apps/
	$(CP) -drvf ../share/application-x-picsimlab-workspace.png  ${sharedir}/../icons/hicolor/64x64/mimetypes/
	$(CP) -drvf ../share/application-x-picsimlab-workspace.xml ${sharedir}/../mime/application/x-picsimlab-workspace.xml

install_docs:
	$(CP) -drvf ../docs ${sharedir} 
		

uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop
	xdg-icon-resource uninstall  --size 64 --novendor picsimlab
	xdg-icon-resource uninstall  --context mimetypes --size 64 application-x-picsimlab-workspace
	xdg-mime uninstall  --mode system ../share/application-x-picsimlab-workspace.xml

run: all
	./picsimlab

clean:
	$(RM) picsimlab *.o core */*.o 
