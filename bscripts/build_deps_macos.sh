#!/bin/bash
# Build PICSimLab dependencies on macOS (Apple Silicon).
# Stages everything into $DEPS (default: ../../macos/deps relative to this script).
# QEMU is built separately by macos/scripts/build_qemu_macos.sh.
# Safe to re-run: finished deps are skipped when their archive already exists.
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DEPS="${DEPS:-$(cd "$SCRIPT_DIR/../.." && pwd)/macos/deps}"
BREW_PREFIX="$(brew --prefix)"
NPROC="$(sysctl -n hw.ncpu)"

export HOMEBREW_PREFIX="$BREW_PREFIX"
export PATH="$BREW_PREFIX/opt/bison/bin:$BREW_PREFIX/opt/flex/bin:$PATH"
export PKG_CONFIG_PATH="$BREW_PREFIX/opt/openal-soft/lib/pkgconfig:$PKG_CONFIG_PATH"

BREW_CPPFLAGS="-I$BREW_PREFIX/include -I$BREW_PREFIX/opt/openal-soft/include"
BREW_LDFLAGS="-L$BREW_PREFIX/lib -L$BREW_PREFIX/opt/openal-soft/lib"

mkdir -p "$DEPS"
cd "$DEPS"

banner()(echo -e "\033[1;32m ---------------------- $1 ---------------------- \033[0m")

banner "download deps"
[ -d picsim ]          || cl git clone --depth=1 https://github.com/lcgamboa/picsim.git
[ -d lxrad_SDL2 ]      || cl git clone --depth=1 https://github.com/lcgamboa/lxrad_SDL2.git
[ -d simavr ]          || cl git clone --depth=1 https://github.com/lcgamboa/simavr.git
[ -d uCsim_picsimlab ] || cl git clone --depth=1 https://github.com/lcgamboa/uCsim_picsimlab.git
# lunasvg pinned to v2.4.1: lxrad_SDL2 uses the 2.x API (renderToBitmap with bg color)
[ -d lunasvg ]         || cl git clone --depth=1 --branch v2.4.1 https://github.com/sammycage/lunasvg.git

banner "patch deps for macOS"
# lxrad_SDL2: get_current_dir_name()/copy_file_range() are Linux-only
if ! git -C lxrad_SDL2 apply --reverse --check "$SCRIPT_DIR/patches/lxrad_SDL2_macos.patch" 2>/dev/null; then
    cl git -C lxrad_SDL2 apply "$SCRIPT_DIR/patches/lxrad_SDL2_macos.patch"
fi
# simavr: avr-gcc is only needed to build firmware examples, not the
# build-simavr library target used by PICSimLab
sed -i '' 's/\$(error Please install avr-gcc[^)]*)/# avr-gcc not required for build-simavr/g' simavr/Makefile.common

if [ ! -f picsim/src/libpicsim.a ]; then
    banner "build picsim"
    cd picsim
    cl make -j"$NPROC"
    ln -sfn ../include include/picsim
    cd ..
fi

if [ ! -f lunasvg/build/liblunasvg.a ]; then
    banner "build lunasvg"
    cd lunasvg
    rm -rf build && mkdir build && cd build
    cl cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC ..
    cl make -j"$NPROC"
    cd ../..
fi

if [ ! -f lxrad_SDL2/lib/liblxrad_SDL2.a ]; then
    banner "build lxrad_SDL2 (static lib only)"
    cd lxrad_SDL2
    # prefix = the repo itself so the baked-in _SHARE font/icon path resolves
    # without a system install; share/lxrad_SDL2 -> . makes ${datadir}/lxrad_SDL2/
    # point at the repo's share/ (fonts/, icons/)
    cl ./configure --prefix="$DEPS/lxrad_SDL2" CPPFLAGS="$BREW_CPPFLAGS" LDFLAGS="$BREW_LDFLAGS"
    ln -sfn . share/lxrad_SDL2
    # configure.ac overwrites CFLAGS, so brew include paths must be appended
    # to the generated Makefile.Conf to reach the compile lines
    echo "CFLAGS += $BREW_CPPFLAGS -std=gnu++17" >> Makefile.Conf
    # only the static archive: the shared-lib rules use -Wl,-soname (Linux-only)
    cl make -C lib clean
    cl make -C lib static -j"$NPROC"
    cd ..
fi

if [ ! -f simavr/simavr/obj-$(cc -dumpmachine)/libsimavr.a ]; then
    banner "build simavr"
    cd simavr
    cl make build-simavr -j"$NPROC"
    ln -sfn ../sim simavr/sim/simavr
    cd ..
fi

if [ ! -f uCsim_picsimlab/picsimlab/libucsim.a ]; then
    banner "build uCsim"
    cd uCsim_picsimlab
    cl ./configure CFLAGS=-fPIC CXXFLAGS=-fPIC
    cl make -j"$NPROC"
    cd picsimlab
    cl make clean
    cl make
    ln -sfn . ucsim
    cd ../..
fi

if [ ! -f gpsim-install/lib/libgpsim.a ]; then
    banner "build gpsim"
    if [ ! -d gpsim-0.32.1 ]; then
        cl curl -L -o gpsim-0.32.1.tar.gz "https://sourceforge.net/projects/gpsim/files/gpsim/0.32.0/gpsim-0.32.1.tar.gz/download"
        cl tar xf gpsim-0.32.1.tar.gz
    fi
    cd gpsim-0.32.1
    cl ./configure --disable-gui --prefix="$DEPS/gpsim-install" \
        CFLAGS="-fPIC $BREW_CPPFLAGS" \
        CXXFLAGS="-fPIC -std=c++14 -Wno-register $BREW_CPPFLAGS" \
        LDFLAGS="$BREW_LDFLAGS"
    cl make -j"$NPROC"
    cl make install
    cd ..
fi

banner "verify"
FAIL=0
for a in picsim/src/libpicsim.a lunasvg/build/liblunasvg.a lxrad_SDL2/lib/liblxrad_SDL2.a \
         "simavr/simavr/obj-$(cc -dumpmachine)/libsimavr.a" \
         uCsim_picsimlab/picsimlab/libucsim.a gpsim-install/lib/libgpsim.a; do
    if [ -f "$a" ]; then
        echo "OK  $a ($(lipo -archs "$a" 2>/dev/null || echo '?'))"
    else
        echo "MISSING  $a"; FAIL=1
    fi
done
exit $FAIL
