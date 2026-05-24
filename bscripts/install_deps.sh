#!/bin/bash 
#show errors in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

echo -e "\033[1;32m ---------------------- update and install packages ---------------------- \033[0m"
cl sudo apt-get update
cl sudo apt-get -y upgrade

#common
cl sudo apt-get -y install git gcc g++ make libncurses-dev sed wget unzip ccache

#lxrad
cl sudo apt-get -y install libwxgtk3.*-dev  libopenal-dev cmake doxygen

#picsimlab
cl sudo apt-get -y install  cutecom gtkwave gedit pulseview libcurl4-openssl-dev 

#simavr
cl sudo apt-get -y install libelf-dev freeglut3-dev gcc-avr avr-libc

#ucsim
cl sudo apt-get -y install bison flex file

#gpsim
cl sudo apt-get -y install gpsim-dev gpsim libglib2.0-dev

#tty0tty
cl sudo apt-get -y install linux-headers-`uname -r` dkms 

#qemu
cl sudo apt-get -y install python3 libpixman-1-dev libfdt-dev  \
 ninja-build meson libgcrypt-dev libslirp-dev python3-distlib

#appimage
cl sudo apt-get -y install libminizip-dev 

#flatpak
cl sudo apt-get -y install flatpak flatpak-builder bzip2

#debian package
sudo apt-get -y install devscripts debmake debhelper 

#cross w32
cl sudo apt-get -y install mingw-w64-common mingw-w64-i686-dev gcc-mingw-w64-i686 g++-mingw-w64-i686 

#cross w64
cl sudo apt-get -y install mingw-w64-common mingw-w64-x86-64-dev gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 