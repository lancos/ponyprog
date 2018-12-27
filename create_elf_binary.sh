#!/bin/bash

CMD=$(type -p cmake)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install cmake and restart this script"
   exit 1
fi

CPU_NUM=$(nproc --all)
echo "cpu cores: $CPU_NUM"

DIRECTORY="build"

if [ ! -d "$DIRECTORY" ]; then
  # Control will enter here if $DIRECTORY exists.
    mkdir "$DIRECTORY"
fi

# because of removing of all files in directory
if [ -d "$DIRECTORY" ]; then
    cd "$DIRECTORY"
    rm * -rf
    cmake -DUSE_DEBUGGER=OFF -DUSE_QT5=ON ..
    make -j$CPU_NUM
    cd ..
    mv ./$DIRECTORY/ponyprog .
fi
