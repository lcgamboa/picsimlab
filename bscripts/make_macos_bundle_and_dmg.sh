#!/usr/bin/env zsh
#show errors in color red
set -euo pipefail

# We do not support installation into system directories on macOS
if [ -z "${DESTDIR-}" ]; then
    echo "Error: DESTDIR environment variable is not set." >&2
    echo "You should set the same value as while calling build_all_and_install_macos.sh" >&2
    exit 1
fi
mkdir -p ${DESTDIR/#\~/$HOME}
DESTDIR="$(realpath "${DESTDIR/#\~/$HOME}")"

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "--------------------------------------------------------"
    echo "Homebrew is required but not installed."
    echo "Please install it from the official website: https://brew.sh"
    echo 'ATTENTION! Follow the "Next Steps" instructions to update your .zprofile?'
    echo "--------------------------------------------------------"
    exit 1
fi
# Check if brew is actually accessible in the path
# (Sometimes 'brew' is installed but the paths are not set up)
if [[ ":$PATH:" != *":$(brew --prefix)/bin:"* ]]; then
    echo "--------------------------------------------------------"
    echo "Homebrew is installed, but it is not in your PATH."
    echo 'Please ensure you have run the "Next steps" instructions from the Homebrew installer'
    echo "--------------------------------------------------------"
    exit 1
fi
echo "Homebrew is installed correctly. Proceeding..."
HOMEBREW_PREFIX=$(brew --prefix)

# Install imagemagick to convert svg to png
brew install -y imagemagick create-dmg

# The path to this script
SCRIPTDIR=$( dirname -- "$( readlink -f -- "$0"; )"; )

# Create an app bundle template
cd ${SCRIPTDIR}/../macos
rm -rf PICSimLab.app
mkdir PICSimLab.app
mkdir PICSimLab.app/Contents
mkdir PICSimLab.app/Contents/MacOS
mkdir PICSimLab.app/Contents/lib
mkdir PICSimLab.app/Contents/Resources

# Substitute the correct version into Info.plist
source ../VERSION
export MACOSX_BUNDLE_SHORT_VERSION_STRING="$VERSION"
export MACOSX_BUNDLE_BUNDLE_VERSION="$VERSION"
envsubst < Info.plist.in > PICSimLab.app/Contents/Info.plist

cd PICSimLab.app/Contents/

# Copy executables, libraries and shared files
cp -P ${DESTDIR}/bin/* MacOS
cp -P ${DESTDIR}/lib/*.dylib* lib
cp -rP ${DESTDIR}/lib/picsimlab lib/
cp -rP ${DESTDIR}/share ./

# Fix library paths and IDs
typeset -A UNPROCESSED
for p in MacOS/*; do
    [[ -e "$p" ]] && UNPROCESSED[$p]=1
done
for p in lib/*(.-.); do
    UNPROCESSED[$p]=1
done
for p in lib/picsimlab/qemu/*(.-.); do
    UNPROCESSED[$p]=1
done
while (( ${#UNPROCESSED[@]} > 0 )); do
    # Pick any key from the associative array
    exec_path=${${(k)UNPROCESSED}[1]}
    exec_name=${exec_path##*/}
    exec_path_original=${UNPROCESSED[$exec_path]}

    echo "Processing: $exec_path"
    typeset -a LIBS
    LIBS=( ${(f)"$(otool -L $exec_path | awk 'NR>1 {print $1}')"} )
    for lib_path in "${LIBS[@]}"; do
        if [[ "$lib_path" == "${HOMEBREW_PREFIX}"* ]]; then
            lib_resolved_path=${lib_path:A}
            lib_name=${lib_resolved_path##*/}
            lib_install_path="lib/$lib_name"
            if [[ ! -e "$lib_install_path" ]]; then
                cp "$lib_resolved_path" "$lib_install_path"
                UNPROCESSED[$lib_install_path]=${lib_path%/*}
            fi
            install_name_tool -change "$lib_path" "@rpath/$lib_name" "$exec_path"
        elif [[ "$lib_path" == @rpath/* ]]; then
            # At the moment we assume all the @rpath libs are in the same dir with the lib which references them
            lib_name=${lib_path##*/}
            lib_path_file=${exec_path_original}/${lib_name}
            lib_resolved_path=${lib_path_file:A}
            lib_name=${lib_resolved_path##*/}
            lib_install_path="lib/$lib_name"
            if [[ ! -e $lib_install_path ]]; then
                cp "$lib_resolved_path" "$lib_install_path"
                UNPROCESSED[$lib_install_path]=${exec_path_original}
            fi
            install_name_tool -change "$lib_path" "@rpath/$lib_name" "$exec_path"
        fi
    done
    if [[ "$exec_path" == *.dylib ]]; then
        install_name_tool -id "@rpath/$exec_name" "$exec_path"
        for rpath in ${(f)"$(otool -l "$exec_path" | awk '/cmd LC_RPATH/{getline; getline; print $2}')"}; do
            install_name_tool -delete_rpath "$rpath" "$exec_path"
        done
    fi
    codesign --force -s - "$exec_path"
    unset "UNPROCESSED[$exec_path]"
done

# Create an iconset
rm -rf MyIcon.iconset
mkdir MyIcon.iconset
magick ${DESTDIR}/share/picsimlab/logo.svg logo.png
sips -z 16 16     logo.png --out MyIcon.iconset/icon_16x16.png
sips -z 32 32     logo.png --out MyIcon.iconset/icon_16x16@2x.png
sips -z 32 32     logo.png --out MyIcon.iconset/icon_32x32.png
sips -z 64 64     logo.png --out MyIcon.iconset/icon_32x32@2x.png
sips -z 128 128   logo.png --out MyIcon.iconset/icon_128x128.png
sips -z 256 256   logo.png --out MyIcon.iconset/icon_128x128@2x.png
sips -z 256 256   logo.png --out MyIcon.iconset/icon_256x256.png
sips -z 512 512   logo.png --out MyIcon.iconset/icon_256x256@2x.png
sips -z 512 512   logo.png --out MyIcon.iconset/icon_512x512.png
mv logo.png MyIcon.iconset/icon_512x512@2x.png
iconutil -c icns MyIcon.iconset
rm -R MyIcon.iconset
mv MyIcon.icns Resources/AppIcon.icns

cd ../..
codesign --force --sign - "PICSimLab.app"

create-dmg \
  --volname "PICSimLab Installer" \
  --window-pos 200 120 \
  --window-size 600 400 \
  --icon-size 100 \
  --icon "PICSimLab.app" 175 120 \
  --hide-extension "PICSimLab.app" \
  --app-drop-link 425 120 \
  --overwrite \
  "PICSimLab.dmg" \
  "PICSimLab.app"

codesign --force --sign - "PICSimLab.dmg"
