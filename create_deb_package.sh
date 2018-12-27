#!/bin/bash

CMD=$(type -p cmake)

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install cmake and restart this script"
   exit 1
fi

# CMD=$(type -p dh)
# 
# if [ ! -x "$CMD" ]; then
#    # not found exit
#    echo "please install debhelper and restart this script"
#    exit 1
# fi

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
    cmake -DUSE_DEBUGGER=OFF -DUSE_QT5=ON ..
    make -j$CPU_NUM
    strip -s ponyprog
    make package-binary-deb
    DEB_PACKAGE_FILE_NAME=$(ls ponyprog*.deb)
    PROJECT_NAME=$(echo ${DEB_PACKAGE_FILE_NAME%.*})
    cd ..
    mv ./$DIRECTORY/$DEB_PACKAGE_FILE_NAME .

    # warning! for the strange cmake versions
    # for example cmake 3.7.2
    # now fix the permission errors and md5sums
    mkdir fix_up_deb
    dpkg-deb -x $DEB_PACKAGE_FILE_NAME fix_up_deb
    dpkg-deb --control $DEB_PACKAGE_FILE_NAME fix_up_deb/DEBIAN
    rm $DEB_PACKAGE_FILE_NAME
    chmod 0644 fix_up_deb/DEBIAN/md5sums
    sed -i -e "s/\(\s.*\)${PROJECT_NAME}\//  /g" fix_up_deb/DEBIAN/md5sums
    find -type d -print0 |xargs -0 chmod 755
    fakeroot dpkg -b fix_up_deb $DEB_PACKAGE_FILE_NAME
    rm -rf fix_up_deb
fi
