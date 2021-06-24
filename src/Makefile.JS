include Makefile.Common

CXX = em++

prefix=/usr

RM= rm -f
CP= cp
MKDIR = mkdir -p

appdir= ${prefix}/share/applications/
sharedir= ${prefix}/share/picsimlab/
execdir= ${prefix}/bin/

#lxrad automatic generated block start, don't edit below!

override CXXFLAGS+= -D_ARCH_=\"Javascript\" -D_DATE_=\"${DATE}\"
override CXXFLAGS+= -Os
#override CXXFLAGS+= -g4 --source-map-base -Werror
override CXXFLAGS+=  -DEXT_BROWSER -DNO_DEBUG -DNO_TOOLS \
        -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_GFX=2\
	-s SDL2_IMAGE_FORMATS='["png","bmp"]' -s USE_SDL_TTF=2 --preload-file assets --preload-file home \
	-D_VERSION_=\"${VERSION}\" -Wall -D_SHARE_=\"/assets/share/\" -I../../LXRAD_SDL2/include/ \
	-I../../picsim/include/  -I../../ucsim-0.6/picsimlab/\
	-I../../simavr/simavr/ -DEXT_BROWSER_EXAMPLES  \
	-I../teste/glib_js/target/include  -I../teste/glib_js/target/include/glib-2.0 \
	-I../teste/glib_js/target/lib/glib-2.0/include 

override CXXFLAGS+= --pre-js load.js
#	-s "BINARYEN_TRAP_MODE='clamp'" 

LINK= -s DEMANGLE_SUPPORT=0  -s SAFE_HEAP=0 -s ASSERTIONS=0 -s TOTAL_MEMORY=128MB \
      -s EXPORTED_FUNCTIONS='["_main","_file_ready","_lxrad_scale_up","_lxrad_scale_down","_dirt_load"]' \
      -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' 

#CXXFLAGS+=-s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=2 
#CXXFLAGS+=--emrun
CXXFLAGS+= -D_NOTHREAD 

LIBS= ../../LXRAD_SDL2/lib/liblxrad_SDL2_js.a ../../picsim/src/libpicsim_js.a ../../simavr/simavr/obj-wasm32-unknown-emscripten/libsimavr.a
LIBS+= ../../LXRAD_SDL2/teste/zlib-master/contrib/minizip/minizip.a
LIBS+= ../../ucsim-0.6/picsimlab/libucsim.ajs
LIBS+= ../teste/glib_js/target/lib/libgpsim.a
LIBS+= ../teste/glib_js/target/lib/libglib-2.0.a
LIBS+= ../../lunasvg/buildjs/liblunasvg.a

#LIBS = `lxrad_SDL2-config --libs` -lpicsim -lsimavr -lelf

#lxrad automatic generated block end, don't edit above!

all: $(OBJS)
	@echo "Linking picsimlab"
	@$(CXX) $(CXXFLAGS) $(LINK) -s WASM=1 $(OBJS) -o picsimlab.html $(LIBS)  --shell-file template.html 
	@$(CXX) $(CXXFLAGS) $(LINK) -s WASM=0 $(OBJS) -o picsimlab_asmjs.html $(LIBS)  --shell-file template_asmjs.html 

%.o: %.cc
	@echo "Compiling $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 


JS_FILES=  emsc.js picsimlab_asmjs.js picsimlab.html picsimlab_mt.js  picsimlab.wasm \
	picsimlab_asmjs.html  picsimlab.css picsimlab.js picsimlab_mt.wasm \
	picsimlab_asmjs.html.mem picsimlab.data picsimlab_mt.html picsimlab_mt.worker.js


JS_DIR= ../../lcgamboa.github.io/js/
EXP_DIR= ../../lcgamboa.github.io/js_exp/

install: 
	cp -fv $(JS_FILES) $(JS_DIR)
	sed -i 's/picsimlab_asmjs\.data/picsimlab\.data/g' $(JS_DIR)picsimlab_asmjs.js
	sed -i 's/picsimlab_mt\.data/picsimlab\.data/g' $(JS_DIR)picsimlab_mt.js

install_exp: 
	cp -fv $(JS_FILES) $(EXP_DIR)
	sed -i 's/picsimlab_asmjs\.data/picsimlab\.data/g' $(EXP_DIR)picsimlab_asmjs.js
	sed -i 's/picsimlab_mt\.data/picsimlab\.data/g' $(EXP_DIR)picsimlab_mt.js


uninstall:
	$(RM) -drvf ${sharedir} 
	$(RM) -dvf ${execdir}picsimlab
	$(RM) -dvf ${appdir}picsimlab.desktop
	xdg-icon-resource uninstall  --context mimetypes --size 48 application-x-picsimlab-workspace
	xdg-mime uninstall  --mode system ../share/application-x-picsimlab-workspace.xml

run: all
	./picsimlab

clean:
	$(RM) *.o core */*.o picsimlab.html picsimlab_asmjs.html picsimlab.js *.wasm *.map *.pre *.data *.wast *.mem\
	       	picsimlab_asmjs.js
