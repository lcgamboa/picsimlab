#!/usr/bin/bash -x
. VERSION

export VERSION=${VERSION}

rm -rf AppDir
make clean
make -j4 $1
make DESTDIR=`pwd`/AppDir install_app
rm -rf AppDir/usr/share/picsimlab/docs/
cp /usr/bin/cutecom AppDir/usr/bin
cp /usr/bin/gtkwave AppDir/usr/bin
install -d  "AppDir/usr/share/glib-2.0/schemas"
cp /usr/share/glib-2.0/schemas/com.geda.gtkwave.gschema.xml AppDir/usr/share/glib-2.0/schemas
glib-compile-schemas AppDir/usr/share/glib-2.0/schemas/ || echo "No AppDir/usr/share/glib-2.0/schemas/"
if [[ -n "$1" ]]; then
  cp /usr/bin/qemu-stm32 AppDir/usr/bin
  teste/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/qemu-stm32
fi
teste/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/cutecom
teste/linuxdeploy-x86_64.AppImage --appdir AppDir --executable=AppDir/usr/bin/gtkwave
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
teste/linuxdeploy-x86_64.AppImage --custom-apprun=/tmp/AppRun --appdir AppDir --output appimage
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
