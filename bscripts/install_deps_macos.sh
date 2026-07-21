#!/bin/bash
# Install PICSimLab build dependencies on macOS via Homebrew.
# No sudo required. Safe to re-run (brew skips installed packages).
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

if ! xcode-select -p >/dev/null 2>&1; then
    echo "Xcode Command Line Tools not found. Run: xcode-select --install" >&2
    exit 1
fi

if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew not found. Install it from https://brew.sh" >&2
    exit 1
fi

BREW_PREFIX=$(brew --prefix)

echo -e "\033[1;32m ---------------------- install packages (Homebrew) ---------------------- \033[0m"

#common
cl brew install git make wget unzip ccache pkg-config

#lxrad_SDL2
cl brew install sdl2 sdl2_gfx sdl2_image sdl2_ttf openal-soft cmake

#picsimlab
# (libcurl comes from the macOS SDK)
cl brew install minizip

#simavr
cl brew install libelf

#ucsim
cl brew install bison flex

#gpsim
cl brew install glib popt

#virtual serial (tty0tty replacement)
cl brew install socat

#qemu
cl brew install python3 pixman dtc ninja meson libgcrypt libslirp gnu-sed

echo -e "\033[1;32m ---------------------- environment notes ---------------------- \033[0m"
cat << EOF
Keg-only packages need explicit paths when building:

  openal-soft:
    PKG_CONFIG_PATH=${BREW_PREFIX}/opt/openal-soft/lib/pkgconfig
    CPPFLAGS=-I${BREW_PREFIX}/opt/openal-soft/include
    LDFLAGS=-L${BREW_PREFIX}/opt/openal-soft/lib

  bison/flex (macOS ships old versions; brewed ones are keg-only):
    PATH=${BREW_PREFIX}/opt/bison/bin:${BREW_PREFIX}/opt/flex/bin:\$PATH

  libelf headers install under ${BREW_PREFIX}/include/libelf.

bscripts/build_deps_macos.sh exports these automatically.
EOF
