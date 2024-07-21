#!/bin/bash 
. VERSION

VERSION_="${VERSION}_${DATE}"

export VERSION=${VERSION_}

sudo apt-get -y install libminizip-dev wget fuse
install -d build_all
cd build_all
git clone https://github.com/lcgamboa/lxrad_nogui.git
echo -e "\033[1;32m ---------------------- build lxrad nogui -------------------------- \033[0m"
cd lxrad_nogui
git pull
make clean;make -j$(nproc)
sudo make install
cd ..
cd ..

rm -rf AppDir
echo -e "\033[1;32m ---------------------- build picsimlab -------------------------- \033[0m"
make clean
make -j$(nproc) LIBPATH="../build_all/" FILE=Makefile.static PKG=\"Appimage\" $1
make LIBPATH="../build_all/" FILE=Makefile.static DESTDIR=`pwd`/AppDir install_app
rm -rf AppDir/usr/share/picsimlab/docs/
wget -c "https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh"
wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod a+x linuxdeploy-x86_64.AppImage linuxdeploy-plugin-gtk.sh
mv linuxdeploy-x86_64.AppImage /tmp/
mv linuxdeploy-plugin-gtk.sh /tmp/
install -d AppDir/usr/lib/picsimlab/
cp -Rv lib/qemu AppDir/usr/lib/picsimlab/

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
/tmp/linuxdeploy-x86_64.AppImage --plugin gtk --custom-apprun=/tmp/AppRun --appdir AppDir --output appimage
install -d "release_${VERSION}"
#remove libgmodule
#wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
#chmod +x appimagetool-x86_64.AppImage
#mv appimagetool-x86_64.AppImage /tmp/
#./PICSimLab-${VERSION}-x86_64.AppImage --appimage-extract
#rm -rf squashfs-root/usr/lib/libgmodule*
#rm -rf squashfs-root/usr/lib/libgdk_pixbuf*
#/tmp/appimagetool-x86_64.AppImage -v squashfs-root
#rm -rf squashfs-root/
#rename package
#mv PICSimLab-x86_64.AppImage PICSimLab-${VERSION}-x86_64.AppImage 
lversion=`lsb_release -d | cut -f2 | sed -e 's/ /_/g' | sed -e 's/\//_/g'`
if [[ -n "$1" ]]; then
  mv -f PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}/PICSimLab-${VERSION}_${lversion}_experimetal_x86_64.AppImage 
else
  mv -f PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}/PICSimLab-${VERSION}_${lversion}_x86_64.AppImage 
fi	
rm -rf AppDir

cd src
rm -rf AppDir
echo -e "\033[1;32m ---------------------- build picsimlab nogui -------------------------- \033[0m"
make clean
make -j$(nproc) LIBPATH="../build_all/" -f Makefile.NOGUI PKG=\"Appimage\" $1
make LIBPATH="../build_all/" -f Makefile.NOGUI DESTDIR=AppDir install
rm -rf AppDir/usr/share/picsimlab/docs/
cd AppDir/usr/share/picsimlab 
find . -type f -name '*.png' -exec rm {} +
find . -type f -name '*.xcf' -exec rm {} +
cd -
install -d AppDir/usr/lib/picsimlab/
cp -Rv lib/qemu AppDir/usr/lib/picsimlab/

/tmp/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
if [[ -n "$1" ]]; then
  mv -f PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}/PICSimLab_NOGUI-${VERSION}_${lversion}_experimental_x86_64.AppImage 
else
  mv -f PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}/PICSimLab_NOGUI-${VERSION}_${lversion}_x86_64.AppImage 
fi	
rm -rf AppDir
