#!/bin/bash -x 

. VERSION

#VERSION="${VERSION_STABLE}"
VERSION="${VERSION}_${DATE}"

DIR=`pwd`

if [ "$#" -ne 0 ]; then
BUILD_EXPERIMENTAL=1
fi

if [[ -z "$BUILD_EXPERIMENTAL" ]]; then
  TARGET=all
else
  TARGET=exp
fi

#compila windows 32 e 64 bits
make clean
make CXX="ccache x86_64-w64-mingw32-g++" FILE=Makefile.cross -j$(nproc) PKG=\"DEV\" ${TARGET}
make clean
make CXX="ccache i686-w64-mingw32-g++" FILE=Makefile.cross_32 -j$(nproc) PKG=\"DEV\" ${TARGET}
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

if [[ -n "$BUILD_EXPERIMENTAL" ]]; then
  zip -r PICSimLab_${VERSION}_win64_experimental.zip picsimlab_win64/ 
  zip -r PICSimLab_${VERSION}_win32_experimental.zip picsimlab_win32/ 
else
  zip -r PICSimLab_${VERSION}_win64.zip picsimlab_win64/ 
  zip -r PICSimLab_${VERSION}_win32.zip picsimlab_win32/ 
fi

mkdir release_${VERSION}
mv *.zip  release_${VERSION}
