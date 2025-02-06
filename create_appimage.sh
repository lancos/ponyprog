#!/bin/bash

CMD=$(type -p cmake)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install cmake and restart this script"
   exit 1
fi

if [ ! -x linuxdeployqt-continuous-x86_64.AppImage ]; then
    wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
    chmod +x linuxdeployqt-continuous-x86_64.AppImage
fi

WORKDIR="$PWD"
BUILDDIR="$WORKDIR/build"

mkdir -p "$BUILDDIR"

if [ ! -d "$BUILDDIR" ]; then
    cmake -E make_directory "$BUILDDIR"
fi

# Build elf
if [ -d "$BUILDDIR" ]; then
    cd "$BUILDDIR"
    rm * -rf
    cmake -DUSE_DEBUGGER=OFF -DUSE_QT5=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/usr" ..
    make -j$(nproc)
    make install DESTDIR=AppDir; find AppDir/
    #cd ..
    export APPIMAGE_EXTRACT_AND_RUN=1
    ../linuxdeployqt-continuous-x86_64.AppImage AppDir/usr/share/applications/ponyprog.desktop -appimage
fi
