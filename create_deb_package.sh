#!/bin/bash

CMD=$(type -p cmake)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install cmake and restart this script"
   exit 1
fi

CPU_NUM=$(nproc --all)
echo "cpu cores: $CPU_NUM"

DIRECTORY="build-deb"

if [ ! -d "$DIRECTORY" ]; then
  # Control will enter here if $DIRECTORY exists.
    mkdir "$DIRECTORY"
fi

# because of removing of all files in directory
if [ -d "$DIRECTORY" ]; then
    cd "$DIRECTORY"
    rm * -rf
    cmake -DUSE_DEBUGGER=OFF ..
    make -j$CPU_NUM
    make package-binary-deb
    cd ..
    mv ./$DIRECTORY/ponyprog*.deb .
fi
