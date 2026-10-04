#!/usr/bin/env bash
#show errors in color red 
set -euo pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

# We do not support installation into system directories on macOS
if [ -z "${DESTDIR-}" ]; then
    echo "Error: DESTDIR environment variable is not set." >&2
    echo "Building and installing the project on macOS without the DESTDIR variable set is not possible." >&2
    exit 1
fi
mkdir -p ${DESTDIR/#\~/$HOME}
DESTDIR="$(realpath "${DESTDIR/#\~/$HOME}")"

# Check if Command Line Tools are installed
if ! xcode-select -p &> /dev/null; then
    echo "Command Line Developer Tools are not installed."
    echo "Please run 'xcode-select --install' to install them."
    exit 1
else
    echo "Command Line Developer Tools found at: $(xcode-select -p)"
fi

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "--------------------------------------------------------"
    echo "Homebrew is required but not installed."
    echo "Please install it from the official website: https://brew.sh"
    echo 'ATTENTION! Follow the "Next Steps" instructions to update your .zprofile?'
    echo "--------------------------------------------------------"
    exit 1
fi
# Check if brew is actually accessible in the path
# (Sometimes 'brew' is installed but the paths are not set up)
if [[ ":$PATH:" != *":$(brew --prefix)/bin:"* ]]; then
    echo "--------------------------------------------------------"
    echo "Homebrew is installed, but it is not in your PATH."
    echo 'Please ensure you have run the "Next steps" instructions from the Homebrew installer'
    echo "--------------------------------------------------------"
    exit 1
fi
echo "Homebrew is installed correctly. Proceeding..."

echo -e "\033[1;32m ---------------------- installing deps from homebrew ---------------------\033[0m"
brew tap osx-cross/homebrew-avr
brew trust osx-cross/avr
brew install -y pkg-config wxwidgets openal-soft cmake avr-gcc libelf gpsim uv
echo -e "\033[1;32m ---------------------- installing python using uv--- ---------------------\033[0m"
uv python install
echo -e "\033[1;32m ---------------------- configuring the build environment -----------------\033[0m"
# Due to some macOS conflicts openal is not discovered automatically by pkg-config
export PKG_CONFIG_PATH="$(brew --prefix openal-soft)/lib/pkgconfig:${PKG_CONFIG_PATH-}"
rm -rf build_all
cl mkdir -p build_all
cd build_all
echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"
git clone --depth=1 https://github.com/lcgamboa/picsim.git
git clone --depth=1 https://github.com/lcgamboa/lxrad.git
git clone --depth=1 https://github.com/lcgamboa/simavr.git
git clone --depth=1 https://github.com/lcgamboa/uCsim_picsimlab.git
git clone --depth=1 --no-single-branch https://github.com/lcgamboa/qemu.git
echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cd picsim
cl git pull --no-rebase
cl make clean
cl make -j$(getconf _NPROCESSORS_ONLN)
cl make install
cd ..
echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
cd lxrad
git pull --no-rebase
cl ./make_deps.sh
cl ./configure --prefix=${DESTDIR}
cl make clean
cl make -j$(getconf _NPROCESSORS_ONLN)
cl make install
cd ..
echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
cd simavr
git pull --no-rebase
cl make clean
cl make build-simavr -j$(getconf _NPROCESSORS_ONLN)
cl make install-simavr
cd ../
echo -e "\033[1;32m ---------------------- build and install uCsim -------------------------- \033[0m"
cd uCsim_picsimlab
git pull --no-rebase
cl ./config_linux.sh
cl make clean
cl make -j$(getconf _NPROCESSORS_ONLN)
cd picsimlab
cl make clean
cl make -j$(getconf _NPROCESSORS_ONLN)
cl make install
cd ../../
echo -e "\033[1;32m ---------------------- build qemu  ---------------------- \033[0m"
cd qemu
cl git checkout -f picsimlab-stm32
git pull --no-rebase
cl ./build_libqemu-stm32.sh
cd build
cl install -d ../../../lib/qemu/
cl cp libqemu-stm32.dylib ../../../lib/qemu/
cd ..
cl git checkout -f picsimlab-esp32
cl ./build_libqemu-esp32.sh
cd build
cl cp libqemu-xtensa.dylib ../../../lib/qemu/
cl cp libqemu-riscv32.dylib ../../../lib/qemu/
cd ..
cl install -d ../../lib/qemu/fw
cl cp pc-bios/esp32-v3-rom*.bin pc-bios/esp32c3-rom.bin ../../lib/qemu/fw/
cd ..
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
cd ../
cl make clean
cl make -j$(getconf _NPROCESSORS_ONLN)
cl make install
echo -e "\033[1;32mPICSimLab has been built and installed successfully\033[0m"
echo -e "\033[1;32mYou can run it using \033[1;34m$DESTDIR/bin/picsimlab\033[1;32m command \033[0m"
