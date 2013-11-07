#!/bin/bash
#$Id$

set -o errexit

CDHERE=`pwd`
VCONFIG="v/Config.mk"

if [ ! -d "v" ]; then
	echo "Invalid path. You must run $0 from PonyProg source dir."
	exit 1
fi

if [ ! -f "$VCONFIG" ]; then
	echo "File $VCONFIG not found. You must run $0 from PonyProg source dir."
	exit 1
fi

if ! grep "HOMEV = your_ponyprog_path" $VCONFIG > /dev/null; then
	if [ -f "$VCONFIG.ori" ]; then
		echo "Already configured, restore original"
		mv $VCONFIG.ori $VCONFIG
	fi
fi

if ! grep "HOMEV = your_ponyprog_path" $VCONFIG > /dev/null; then
	echo "Already configured, exiting"
	exit 1
fi

sed -i.ori "s@HOMEV = your_ponyprog_path/software/v@HOMEV = $CDHERE/v@" $VCONFIG

if [ -z "$1" ]; then
	echo "No parameter: default INSTALL_PREFIX (/usr/local)"
else
	echo "INSTALL_PREFIX=$1"
	sed -i "s@INSTALL_PREFIX = /usr/local@INSTALL_PREFIX = $1@" $VCONFIG
fi

