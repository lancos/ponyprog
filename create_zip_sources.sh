#!/bin/bash


DIRECTORY="build"

if [ ! -d "$DIRECTORY" ]; then
  # Control will enter here if $DIRECTORY exists.
    mkdir "$DIRECTORY"
fi

# because of removing of all files in directory
if [ -d "$DIRECTORY" ]; then
    cd "$DIRECTORY"
    rm * -rf
    cmake -DUSE_DEBUGGER=ON ..
    #make -j$CPU_NUM
    make package-source-zip
    cd ..
    mv ./$DIRECTORY/distribution/source/PonyProg*/ponyprog-source-*.zip .
fi
