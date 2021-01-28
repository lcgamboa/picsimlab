
. VERSION

export VERSION=${VERSION}

rm -rf AppDir
make clean
make -j4 $1
make DESTDIR=`pwd`/AppDir install_app
rm -rf AppDir/usr/share/picsimlab/docs/
cp /usr/bin/cutecom AppDir/usr/bin
cp /usr/bin/gtkwave AppDir/usr/bin
teste/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/cutecom
teste/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/gtkwave
teste/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
if [[ -n "$1" ]]; then
  mv PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}/PICSimLab-${VERSION}_experimetal-x86_64.AppImage 
else
  mv PICSimLab-${VERSION}-x86_64.AppImage release_${VERSION}
fi	
rm -rf AppDir


cd src
rm -rf AppDir
make clean
make -f Makefile.NOGUI -j4 $1
make -f Makefile.NOGUI DESTDIR=AppDir install
rm -rf AppDir/usr/share/picsimlab/docs/
cd AppDir/usr/share/picsimlab 
find . -type f -name '*.png' -exec rm {} +
find . -type f -name '*.xcf' -exec rm {} +
cd -
../teste/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
if [[ -n "$1" ]]; then
  mv PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}/PICSimLab_NOGUI-${VERSION}_experimental-x86_64.AppImage 
else
  mv PICSimLab_NOGUI-${VERSION}-x86_64.AppImage ../release_${VERSION}
fi	
rm -rf AppDir
