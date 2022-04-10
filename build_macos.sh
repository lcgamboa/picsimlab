#!/bin/bash
set -e
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

if [ "$#" -ne 0 ]; then
BUILD_EXPERIMETAL=1
fi

BUILD_DIR=`pwd`/build_all
INSTALL_DIR=`pwd`/install_all
pushd .
if [ -d "$BUILD_DIR" ]; then rm -rf $BUILD_DIR; fi
mkdir -p $BUILD_DIR

if [ -d "$INSTALL_DIR" ]; then rm -rf $INSTALL_DIR; fi
mkdir -p $INSTALL_DIR/lib $INSTALL_DIR/include


echo -e "\033[1;32m ---------------------- update and install packages ---------------------- \033[0m"
#brew stuff
#brew install wxwidgets
#brew install openal-soft
#brew install cmake
#brew install doxygen
#brew tap osx-cross/homebrew-avr
#brew install avr-gcc

echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"

cd $BUILD_DIR
git clone --depth=1 https://github.com/lcgamboa/picsim.git
git clone --depth=1 https://github.com/lcgamboa/lxrad.git
#git clone --depth=1 https://github.com/lcgamboa/tty0tty.git
git clone --depth=1 https://github.com/lcgamboa/simavr.git
git clone --depth=1 https://github.com/lcgamboa/uCsim_picsimlab.git

if [[ -n "$BUILD_EXPERIMETAL" ]]; then
#cl sudo apt-get -y install python libglib2.0-dev libpixman-1-dev libfdt-dev gpsim-dev gpsim \
#ninja-build meson
git clone --depth=1 --no-single-branch https://github.com/lcgamboa/qemu_stm32.git
fi	

echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cd $BUILD_DIR/picsim
make
#make DESTDIR=$INSTALL_DIR install

echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
cd $BUILD_DIR/lxrad
./make_deps.sh
./configure --prefix=$INSTALL_DIR LDFLAGS=-L/usr/local/opt/openal-soft/lib CFLAGS=-I/usr/local/opt/openal-soft/include
make
#make install
cd $BUILD_DIR/lunasvg/build
make DESTDIR=$INSTALL_DIR install

echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
cd $BUILD_DIR/simavr
# cl make clean;
make build-simavr
# #cl sudo make install-simavr

echo -e "\033[1;32m ---------------------- build and install uCsim -------------------------- \033[0m"
cd $BUILD_DIR/uCsim_picsimlab
./configure --prefix=$INSTALL_DIR
make 
#make install
cd picsimlab
make
#make install
popd

if [[ -z "$BUILD_EXPERIMETAL" ]]; then
echo "Skipping qemu"
else
echo -e "\033[1;32m ---------------------- build and install qemu_stm32 --------------------- \033[0m"
cd qemu_stm32
cl git checkout picsimlab
git pull --no-rebase
cl ./build_libqemu-stm32.sh
cd build
cl cp qemu-system-arm qemu-stm32
cl strip qemu-stm32
cl sudo cp qemu-stm32 /usr/bin/
fi

echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
make clean
if [[ -z "$BUILD_EXPERIMETAL" ]]; then
mkdir build_all/picsim/include/picsim
cp build_all/picsim/include/*.* build_all/picsim/include/picsim
mkdir build_all/uCsim_picsimlab/picsimlab/ucsim
cp build_all/uCsim_picsimlab/picsimlab/*.* build_all/uCsim_picsimlab/picsimlab/ucsim
mkdir build_all/simavr/simavr/sim/simavr
cp build_all/simavr/simavr/sim/*.* build_all/simavr/simavr/sim/simavr/

pushd build_all/simavr/simavr
ln -s obj-x86_64-apple-darwin20.6.0 obj-x86_64-linux-gnu
popd

make LIBPATH="../build_all/" FILE=Makefile.macos.static
else
make exp
fi
#make install

#
#user=`whoami`
#cl sudo usermod -a -G dialout $user
#echo -e "\033[1;32m ---------------------- done! -------------------------------------------- \033[0m"
#echo -e "\033[1;32m logout your session and login to use serial \033[0m"
#sudo ldconfig
#if xhost > /dev/null 2>&1 ; then
#cl picsimlab
#fi
#