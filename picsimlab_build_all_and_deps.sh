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
cl sudo apt-get -y install git doxygen autoconf gcc g++ make libwxgtk3.0-gtk3-dev \
libelf-dev freeglut3-dev cutecom gcc-avr avr-libc libopenal-dev libncurses-dev gtkwave
cl sudo apt-get -y install linux-headers-`uname -r` dkms 
cl mkdir build_all
cd build_all
echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"
git clone https://github.com/lcgamboa/picsim.git
git clone https://github.com/lcgamboa/lxrad.git
git clone https://github.com/lcgamboa/tty0tty.git
git clone https://github.com/lcgamboa/simavr.git
if [[ -n "$BUILD_EXPERIMETAL" ]]; then
cl sudo apt-get -y install bison++ python libglib2.0-dev libpixman-1-dev libfdt-dev gpsim-dev gpsim
git clone https://github.com/lcgamboa/uCsim_picsimlab.git
git clone https://github.com/lcgamboa/qemu_stm32.git
fi	
echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cd picsim
cl git pull
cl make clean;make -j4
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
cd lxrad
git pull
cl autoconf
cl ./configure --prefix=/usr
cl make clean;make -j4
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install tty0tty ------------------------ \033[0m"
cd tty0tty/module
git pull
cl sudo ./dkms-install.sh
#cl make clean;make -j4
#cl sudo make install
sudo usermod -a -G dialout `whoami`
cd ../../
echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
cd simavr
git pull
cl make clean;make -j4 
cl sudo make install
cd ../
if [[ -z "$BUILD_EXPERIMETAL" ]]; then
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
cd ../
#git pull
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
cl sudo cp qemu-stm32 /usr/local/bin/
cd ../../
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
#git pull
cd ../
cl make clean;make -j4 exp
cl sudo make install
fi
user=`whoami`
cl sudo usermod -a -G dialout $user
echo -e "\033[1;32m ---------------------- done! -------------------------------------------- \033[0m"
echo -e "\033[1;32m logout your session and login to use serial \033[0m"
sudo ldconfig
cl picsimlab
