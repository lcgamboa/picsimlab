#!/bin/bash -x 

. VERSION

#VERSION="${VERSION_STABLE}"
VERSION="${VERSION}_${DATE}"

DIR=`pwd`

if [ "$#" -ne 0 ]; then
BUILD_EXPERIMETAL=1
fi

if [[ -z "$BUILD_EXPERIMETAL" ]]; then
  TARGET=all
else
  TARGET=exp
fi

#compila windows 32 e 64 bits
make clean
make CXX="ccache x86_64-w64-mingw32-g++" FILE=Makefile.cross -j$(nproc) ${TARGET}
make clean
make CXX="ccache i686-w64-mingw32-g++" FILE=Makefile.cross_32 -j$(nproc) ${TARGET}
make clean
cd ..
rm -Rf picsimlab_win32/share/ 
rm -Rf picsimlab_win64/share/ 

cd picsimlab
git archive --format=tar master > /tmp/picsimlab.tar
cd /tmp
rm -Rf picsimlab
mkdir picsimlab
cd picsimlab
tar xvf ../picsimlab.tar
cp -Rv share/ ${DIR}/picsimlab_win32/
cp -Rv docs/  ${DIR}/picsimlab_win32/share/
cp -Rv share/ ${DIR}/picsimlab_win64/
cp -Rv docs/  ${DIR}/picsimlab_win64/share/

#cria instalacao windows
cd ${DIR}
rm -Rf picsimlab_win32/share/docs
rm -Rf picsimlab_win64/share/docs 
if [[ -z "$BUILD_EXPERIMETAL" ]]; then
rm -Rf picsimlab_win64/qemu-stm32.exe
rm -Rf picsimlab_win64/libglib-2.0-0.dll
rm -Rf picsimlab_win64/libgthread-2.0-0.dll
rm -Rf picsimlab_win64/libiconv-2.dll
rm -Rf picsimlab_win64/libintl-8.dll
rm -Rf picsimlab_win64/libssp-0.dll
rm -Rf picsimlab_win64/libwinpthread-1.dll
#rm -Rf picsimlab_win64/zlib1.dll
#rm -Rf picsimlab_win64/ucsim.dll
rm -Rf picsimlab_win64/libgpsim-0.dll
rm -Rf picsimlab_win32/qemu-stm32.exe
rm -Rf picsimlab_win32/libglib-2.0-0.dll
rm -Rf picsimlab_win32/libgthread-2.0-0.dll
rm -Rf picsimlab_win32/libiconv-2.dll
rm -Rf picsimlab_win32/libintl-8.dll
rm -Rf picsimlab_win32/libssp-0.dll
rm -Rf picsimlab_win32/libwinpthread-1.dll
#rm -Rf picsimlab_win32/zlib1.dll
#rm -Rf picsimlab_win32/ucsim.dll
rm -Rf picsimlab_win32/libgpsim-0.dll
else
cp -d exp_w64/bin/* picsimlab_win64/ 
cp -d exp_w64/lib/* picsimlab_win64/
cp -d exp_w32/bin/* picsimlab_win32/ 
cp -d exp_w32/lib/* picsimlab_win32/
fi


if [[ -n "$BUILD_EXPERIMETAL" ]]; then
  zip -r picsimlab_${VERSION}_experimental_win64.zip picsimlab_win64/ 
  zip -r picsimlab_${VERSION}_experimental_win32.zip picsimlab_win32/ 
else
  zip -r picsimlab_${VERSION}_win64.zip picsimlab_win64/ 
  zip -r picsimlab_${VERSION}_win32.zip picsimlab_win32/ 
fi

mkdir release_${VERSION}
mv *.zip  release_${VERSION}
