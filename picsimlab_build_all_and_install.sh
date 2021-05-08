#!/bin/bash 
#show errors in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

if [ "$#" -ne 0 ]; then
BUILD_EXPERIMETAL=1
fi

echo -e "\033[1;32m ---------------------- update and install packages ---------------------- \033[0m"
cl sudo apt-get update
cl sudo apt-get -y upgrade
cl sudo apt-get -y install git doxygen gcc g++ make libwxgtk3.0-gtk3-dev \
libelf-dev freeglut3-dev cutecom gcc-avr avr-libc libopenal-dev libncurses-dev gtkwave \
gedit cmake
cl sudo apt-get -y install linux-headers-`uname -r` dkms 
cl mkdir build_all
cd build_all
echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"
git clone --depth=1 https://github.com/lcgamboa/picsim.git
git clone --depth=1 https://github.com/lcgamboa/lxrad.git
git clone --depth=1 https://github.com/lcgamboa/tty0tty.git
git clone --depth=1 https://github.com/lcgamboa/simavr.git
if [[ -n "$BUILD_EXPERIMETAL" ]]; then
cl sudo apt-get -y install bison++ python libglib2.0-dev libpixman-1-dev libfdt-dev gpsim-dev gpsim
git clone --depth=1 https://github.com/lcgamboa/uCsim_picsimlab.git
git clone --depth=1 --no-single-branch https://github.com/lcgamboa/qemu_stm32.git
fi	
echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cd picsim
cl git pull --no-rebase
cl make clean;make -j4
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
cd lxrad
git pull --no-rebase
cl ./make_deps.sh
cl ./configure --prefix=/usr
cl make clean;make -j4
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install tty0tty ------------------------ \033[0m"
cd tty0tty/module
git pull --no-rebase
cl sudo ./dkms-install.sh
#cl make clean;make -j4
#cl sudo make install
sudo usermod -a -G dialout `whoami`
cl sudo modprobe tty0tty
cd ../../
echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
cd simavr
git pull --no-rebase
cl make clean;make -j4 
cl sudo make install
cd ../
if [[ -z "$BUILD_EXPERIMETAL" ]]; then
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
cd ../
#git pull --no-rebase
cl make clean;make -j4
cl sudo make install
else	
echo -e "\033[1;32m ---------------------- build and uCsim ---------------------------------- \033[0m"
cd uCsim_picsimlab
cl ./config_linux.sh
cl make clean;make -j4
cd picsimlab
cl make clean;make -j4
cl sudo make install
cd ../../
echo -e "\033[1;32m ---------------------- build and install qemu_stm32 --------------------- \033[0m"
cd qemu_stm32
cl git checkout picsimlab
cl ./configure --target-list="arm-softmmu" --disable-werror --disable-sdl --disable-vnc --disable-docs --disable-blobs --static --disable-virtfs --disable-libusb --disable-libnfs --disable-vhost-net --disable-vde --disable-bluez --disable-curses --disable-gtk
cl make clean;make -j4
cd arm-softmmu
cl cp qemu-system-arm qemu-stm32
cl strip qemu-stm32
cl sudo cp qemu-stm32 /usr/bin/
cd ../../
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
#git pull --no-rebase
cd ../
cl make clean;make -j4 exp
cl sudo make install
fi
user=`whoami`
cl sudo usermod -a -G dialout $user
echo -e "\033[1;32m ---------------------- done! -------------------------------------------- \033[0m"
echo -e "\033[1;32m logout your session and login to use serial \033[0m"
sudo ldconfig
if xhost > /dev/null 2>&1 ; then
cl picsimlab
fi
