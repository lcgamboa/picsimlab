#!/bin/bash -x 
. VERSION

rm -rf build_all/picsimlab*
sudo apt-get install debmake debhelper sed
git archive --format=tar master > build_all/picsimlab-${VERSION}.tar 
cd build_all
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
mv src/Makefile.static src/Makefile
mv tools/srtank/Makefile.static tools/srtank/Makefile
mv tools/espmsim/Makefile.static tools/espmsim/Makefile
mv tools/PinViewer/Makefile.static tools/PinViewer/Makefile
if [[ -n "$1" ]]; then
PWD=`pwd`	
echo -e "\noverride_dh_auto_build:\n\tmake -j4 $1\n\n" >> debian/rules
echo -e "\noverride_dh_auto_install:\n\tdh_auto_install\n\tinstall -p -D -m 0755 ../qemu_stm32/arm-softmmu/qemu-stm32 ${PWD}/debian/picsimlab/usr/bin/qemu-stm32\n\n" >> debian/rules
sed -i 's/gtkwave/gtkwave,gpsim/g' debian/control
fi
cd ..
tar cvfz  picsimlab-${VERSION}.tar.gz picsimlab-${VERSION}
mv picsimlab-${VERSION}.tar.gz picsimlab_${VERSION}.orig.tar.gz
cd picsimlab-${VERSION}
debuild -us -uc
cd ..
lversion=`lsb_release -d | cut -f2 | sed -e 's/ /_/g'`
if [[ -n "$1" ]]; then
	mv picsimlab_${VERSION}-1_amd64.deb  picsimlab_${VERSION}_experimetal_${lversion}_amd64.deb
else
mv picsimlab_${VERSION}-1_amd64.deb  picsimlab_${VERSION}_${lversion}_amd64.deb
fi
