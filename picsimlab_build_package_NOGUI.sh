#!/bin/bash -x 
. VERSION

rm -rf build_all/picsimlab*
sudo apt-get install debmake debhelper sed
git archive --format=tar master > build_all/picsimlab-${VERSION}.tar 
cd build_all
git clone https://github.com/lcgamboa/lxrad_nogui.git
echo -e "\033[1;32m ---------------------- build and install lxrad nogui -------------------------- \033[0m"
mv lxrad_nogui lxrad_NOGUI
cd lxrad_NOGUI
git pull
make clean;make -j$(nproc)
sudo make install
cd ..
tar  --delete -f picsimlab-${VERSION}.tar "docs/kicad"
tar  --delete -f picsimlab-${VERSION}.tar "docs/examples"
tar  --delete -f picsimlab-${VERSION}.tar "docs/pdf"
tar  --delete -f picsimlab-${VERSION}.tar "docs/srtank"
tar  --delete -f picsimlab-${VERSION}.tar "docs/espmsim"
tar  --delete --wildcards -f picsimlab-${VERSION}.tar "docs/*.png"
tar  --delete --wildcards -f picsimlab-${VERSION}.tar "docs/*.html" 
tar  --delete --wildcards -f picsimlab-${VERSION}.tar "docs/*.pdf" 
tar  --delete --wildcards -f picsimlab-${VERSION}.tar "docs/*.css" 
gzip -q picsimlab-${VERSION}.tar
mkdir picsimlab-${VERSION}
cd picsimlab-${VERSION}
tar xvfz ../picsimlab-${VERSION}.tar.gz
sed -i 's/src tools\/espmsim tools\/srtank/src/g' Makefile
mv src/Makefile.static src/Makefile
sed -i 's/lxrad/lxrad_NOGUI/g' src/Makefile
sed -i 's/Linux64_WX/Linux64_NOGUI/g' src/Makefile
sed -i 's/`wx-config --libs` `wx-config --libs stc`/ -lminizip -lpthread/g' src/Makefile 
sed -i 's/picsimlab /picsimlab_NOGUI /g' src/Makefile
mv tools/srtank/Makefile.static tools/srtank/Makefile
mv tools/espmsim/Makefile.static tools/espmsim/Makefile
mv tools/PinViewer/Makefile.static tools/PinViewer/Makefile
rm -rf share/*/*/*.png
if [[ -n "$1" ]]; then
 PWD=`pwd`	
 echo -e "\noverride_dh_auto_build:\n\tmake -j$(nproc) $1\n\n" >> debian/rules
 echo -e "\noverride_dh_auto_install:\n\tdh_auto_install\n\tinstall -p -D -m 0755 ../qemu_stm32/arm-softmmu/qemu-stm32 ${PWD}/debian/picsimlab/usr/bin/qemu-stm32\n\n" >> debian/rules
 sed -i 's/Depends: gtkwave, cutecom, gedit,/Depends: gpsim, /g' debian/control
else
 sed -i 's/Depends: gtkwave, cutecom, gedit,/Depends: /g' debian/control
fi
cd ..
tar cvfz  picsimlab-${VERSION}.tar.gz picsimlab-${VERSION}
mv picsimlab-${VERSION}.tar.gz picsimlab_${VERSION}.orig.tar.gz
cd picsimlab-${VERSION}
debuild -us -uc
cd ..
lversion=`lsb_release -d | cut -f2 | sed -e 's/ /_/g' | sed -e 's/\//_/g'`
if [[ -n "$1" ]]; then
	mv picsimlab_${VERSION}-1_amd64.deb  picsimlab_NOGUI_${VERSION}_experimetal_${lversion}_amd64.deb
else
mv picsimlab_${VERSION}-1_amd64.deb  picsimlab_NOGUI_${VERSION}_${lversion}_amd64.deb
fi
