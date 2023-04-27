#!/bin/bash 
#show errors in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

echo -e "\033[1;32m ---------------------- update and install packages ---------------------- \033[0m"
cl sudo apt-get update
cl sudo apt-get -y upgrade
cl sudo apt-get -y install git doxygen gcc g++ make libwxgtk3.0-gtk3-dev \
libelf-dev freeglut3-dev cutecom gcc-avr avr-libc libopenal-dev libncurses-dev gtkwave \
gedit cmake bison++ pulseview
cl sudo apt-get -y install linux-headers-`uname -r` dkms 
cl mkdir build_all
cd build_all
echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"
git clone --depth=1 https://github.com/lcgamboa/picsim.git
git clone --depth=1 https://github.com/lcgamboa/lxrad.git
git clone --depth=1 https://github.com/lcgamboa/tty0tty.git
git clone --depth=1 https://github.com/lcgamboa/simavr.git
git clone --depth=1 https://github.com/lcgamboa/uCsim_picsimlab.git
cl sudo apt-get -y install python3 libglib2.0-dev libpixman-1-dev libfdt-dev gpsim-dev gpsim \
ninja-build meson libgcrypt-dev libslirp-dev
git clone --depth=1 --no-single-branch https://github.com/lcgamboa/qemu.git	
echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cd picsim
cl git pull --no-rebase
cl make clean;make -j$(nproc)
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
cd lxrad
git pull --no-rebase
cl ./make_deps.sh
cl ./configure --prefix=/usr
cl make clean;make -j$(nproc)
cl sudo make install
cd ..
echo -e "\033[1;32m ---------------------- build and install tty0tty ------------------------ \033[0m"
cd tty0tty/module
git pull --no-rebase
cl sudo ./dkms-install.sh
#cl make clean;make -j$(nproc)
#cl sudo make install
sudo usermod -a -G dialout `whoami`
cl sudo modprobe tty0tty
cd ../../
echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
cd simavr
git pull --no-rebase
cl make clean;make build-simavr -j$(nproc) 
cl sudo make install-simavr
cd ../
echo -e "\033[1;32m ---------------------- build and install uCsim -------------------------- \033[0m"
cd uCsim_picsimlab
git pull --no-rebase
cl ./config_linux.sh
cl make clean;make -j$(nproc)
cd picsimlab
cl make clean;make -j$(nproc)
cl sudo make install
cd ../../
echo -e "\033[1;32m ---------------------- build qemu  ---------------------- \033[0m"
cd qemu
cl git checkout picsimlab-stm32
git pull --no-rebase
cl ./build_libqemu-stm32.sh
cd build
cl strip libqemu-stm32.so
cl install -d ../../../lib/qemu/
cl cp libqemu-stm32.so ../../../lib/qemu/
cd ..
cl git checkout picsimlab-esp32
cl ./build_libqemu-esp32.sh
cd build
cl strip libqemu-xtensa.so
cl cp libqemu-xtensa.so ../../../lib/qemu/
cd ..
cl install -d ../../lib/qemu/fw
cl cp pc-bios/esp32-v3-rom*.bin ../../lib/qemu/fw/
cd ..
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
#git pull --no-rebase
cd ../
cl make clean;make -j$(nproc) 
cl sudo make install
user=`whoami`
cl sudo usermod -a -G dialout $user
echo -e "\033[1;32m ---------------------- done! -------------------------------------------- \033[0m"
echo -e "\033[1;32m logout your session and login to use serial \033[0m"
sudo ldconfig
if xhost > /dev/null 2>&1 ; then
cl picsimlab
fi
