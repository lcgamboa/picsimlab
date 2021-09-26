#!/bin/bash 
. VERSION

VERSION_="${VERSION}_${DATE}"

export VERSION=${VERSION_}

sudo apt-get -y install libminizip-dev
install -d build_all
cd build_all
git clone https://github.com/lcgamboa/lxrad_nogui.git
echo -e "\033[1;32m ---------------------- build lxrad nogui -------------------------- \033[0m"
cd lxrad_nogui
git pull
make clean;make -j4
sudo make install
cd ..
cd ..

rm -rf AppDir
echo -e "\033[1;32m ---------------------- build picsimlab -------------------------- \033[0m"
make clean
make -j4 LIBPATH="../build_all/" FILE=Makefile.static $1
make LIBPATH="../build_all/" FILE=Makefile.static DESTDIR=`pwd`/AppDir install_app
rm -rf AppDir/usr/share/picsimlab/docs/
#cp /usr/bin/cutecom AppDir/usr/bin
cp /usr/bin/gtkwave AppDir/usr/bin
install -d  "AppDir/usr/share/glib-2.0/schemas"
cp /usr/share/glib-2.0/schemas/com.geda.gtkwave.gschema.xml AppDir/usr/share/glib-2.0/schemas
glib-compile-schemas AppDir/usr/share/glib-2.0/schemas/ || echo "No AppDir/usr/share/glib-2.0/schemas/"
install -d AppDir/usr/lib/tcl8.6
cp -rv /usr/share/tcltk/tcl8.6/*  AppDir/usr/lib/tcl8.6/
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod a+x linuxdeploy-x86_64.AppImage
mv linuxdeploy-x86_64.AppImage /tmp/
if [[ -n "$1" ]]; then
  cp build_all/qemu_stm32/arm-softmmu/qemu-stm32 AppDir/usr/bin
  /tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/qemu-stm32
fi
#/tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/cutecom
/tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/gtkwave
rm -rf /tmp/AppRun
echo "#!/usr/bin/bash" >> /tmp/AppRun
echo "" >> /tmp/AppRun
echo "export APPDIR=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"" >> /tmp/AppRun
echo "export PATH=\"\$APPDIR/usr/bin/:\$PATH\"" >> /tmp/AppRun
echo "export LD_LIBRARY_PATH=\"\$APPDIR/usr/lib:\$PATH\"" >> /tmp/AppRun
echo "export XDG_DATA_DIRS=\"\$APPDIR/usr/share/:/usr/share/:\$XDG_DATA_DIRS\"" >> /tmp/AppRun
echo "" >> /tmp/AppRun
echo "\$APPDIR/usr/bin/picsimlab \$@" >> /tmp/AppRun
chmod a+x /tmp/AppRun
/tmp/linuxdeploy-x86_64.AppImage --custom-apprun=/tmp/AppRun --appdir AppDir --output appimage
install -d "release_${VERSION}"
if [[ -n "$1" ]]; then
  mv -f PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}/PICSimLab-${VERSION}_experimetal-x86_64.AppImage 
else
  mv -f PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}
fi	
rm -rf AppDir

cd src
rm -rf AppDir
echo -e "\033[1;32m ---------------------- build picsimlab nogui -------------------------- \033[0m"
make clean
make -j4 LIBPATH="../build_all/" -f Makefile.NOGUI $1
make LIBPATH="../build_all/" -f Makefile.NOGUI DESTDIR=AppDir install
rm -rf AppDir/usr/share/picsimlab/docs/
cd AppDir/usr/share/picsimlab 
find . -type f -name '*.png' -exec rm {} +
find . -type f -name '*.xcf' -exec rm {} +
cd -
if [[ -n "$1" ]]; then
  cp build_all/qemu_stm32/arm-softmmu/qemu-stm32 AppDir/usr/bin
  /tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/qemu-stm32
fi
/tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
if [[ -n "$1" ]]; then
  mv -f PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}/PICSimLab_NOGUI-${VERSION}_experimental-x86_64.AppImage 
else
  mv -f PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}
fi	
rm -rf AppDir
