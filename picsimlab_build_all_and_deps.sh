#!/bin/bash 
#show erros in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

echo -e "\033[1;32m ---------------------- update and install packages ---------------------- \033[0m"
cl sudo apt-get update
cl sudo apt-get -y upgrade
cl sudo apt-get -y install git doxygen autoconf gcc g++ make libwxgtk3.0-dev linux-headers-`uname -r` libelf-dev freeglut3-dev cutecom
cl mkdir build_all
cd build_all
echo -e "\033[1;32m ---------------------- download deps -------------------------------------\033[0m"
git clone https://github.com/lcgamboa/picsim.git
git clone https://github.com/lcgamboa/lxrad.git
git clone https://github.com/lcgamboa/tty0tty.git
git clone https://github.com/buserror/simavr
cd picsim
echo -e "\033[1;32m ---------------------- build and install picsim ------------------------- \033[0m"
cl git pull
cl make clean;make -j4
cl sudo make install
cd ..
cd lxrad
echo -e "\033[1;32m ---------------------- build and install lxrad -------------------------- \033[0m"
git pull
cl autoconf
cl ./configure --prefix=/usr
cl make clean;make -j4
cl sudo make install
cd ..
cd tty0tty/module
echo -e "\033[1;32m ---------------------- build and install tty0tty ------------------------ \033[0m"
git pull
cl make clean;make -j4
cl sudo make install
cd ../../
cd simavr
echo -e "\033[1;32m ---------------------- build and install simavr ------------------------- \033[0m"
git pull
cl make clean;make -j4 
cl sudo make install
cd ../../
echo -e "\033[1;32m ---------------------- build and install picsimlab ---------------------- \033[0m"
#git pull
cl make clean;make -j4
cl sudo make install
user=`whoami`
cl sudo usermod -a -G dialout $user
echo -e "\033[1;32m ---------------------- done! -------------------------------------------- \033[0m"
cl picsimlab
