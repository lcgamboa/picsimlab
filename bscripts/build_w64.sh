#!/bin/bash 
. VERSION

if [[ "$VERSION" == "$STABLE_VERSION" ]]; then
	VERSION="${STABLE_VERSION}_${STABLE_DATE}"
	RELEASE="v${STABLE_VERSION}"
else
    VERSION="${VERSION}_${DATE}"
	RELEASE="latestbuild"
fi

#show errors in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

cl wget https://github.com/lcgamboa/picsimlab/releases/download/${RELEASE}/picsimlab_${VERSION}_w64-dev.zip
cl unzip -o picsimlab_${VERSION}_w64-dev.zip
make clean
make CXX="ccache x86_64-w64-mingw32-g++" FILE=Makefile.cross -j$(nproc) PKG=\"DEV\" ${1}
