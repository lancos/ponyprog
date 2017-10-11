#!/bin/bash


DIRECTORY="build"

if [ ! -d "$DIRECTORY" ]; then
  # Control will enter here if $DIRECTORY exists.
    mkdir "$DIRECTORY"
fi

# because of removing of all files in directory
if [ -d "$DIRECTORY" ]; then
    rm -f ./ponyprog-source-*.*
    cd "$DIRECTORY"
    rm * -rf
    cmake -DUSE_DEBUGGER=OFF -DUSE_QT5=OFF ..
    #make -j$CPU_NUM
    make package-source-tgz
    cd ..
    mv ./$DIRECTORY/distribution/source/PonyProg*/ponyprog-source-*.tar.gz .
fi
