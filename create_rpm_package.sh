#!/bin/bash

CMD=$(type -p cmake)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install cmake and restart this script"
   exit 1
fi

CMD=$(type -p rpmbuild)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install rpmbuild and restart this script"
   exit 1
fi

CPU_NUM=$(nproc --all)
echo "cpu cores: $CPU_NUM"

DIRECTORY="build-rpm"

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
    strip -s ponyprog
    make package-binary-rpm
    cd ..
    mv ./$DIRECTORY/ponyprog*.rpm .
fi

