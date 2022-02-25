#!/bin/bash  
. VERSION
#show errors in color red 
set -o pipefail
cl()("$@" 2>&1>&3|sed $'s,.*,\e[31m&\e[m,'>&2)3>&1

cl sudo apt-get update
cl sudo apt-get -y upgrade
cl sudo apt-get -y install flatpak flatpak-builder

flatpak --user remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak --user install  flathub org.freedesktop.Platform//21.08 org.freedesktop.Sdk//21.08 -y

#VERSION="${VERSION_STABLE}"
VERSION="${VERSION}_${DATE}"

flatpak-builder --repo=repo --force-clean build-dir package/com.github.lcgamboa.picsimlab/com.github.lcgamboa.picsimlab.yaml
mkdir release_${VERSION}
flatpak build-bundle repo/ release_${VERSION}/picsimlab_Linux_${VERSION}.flatpak com.github.lcgamboa.picsimlab
