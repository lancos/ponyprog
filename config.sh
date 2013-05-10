#!/bin/bash
#$Id$

CDHERE=`pwd`

if [ ! -d "v" ]; then
	echo "Invalid path. You must run $0 from PonyProg source dir."
	exit 1
fi

if [ ! -f "v/Config.mk" ]; then
	echo "File v/Config.mk not found. You must run $0 from PonyProg source dir."
	exit 1
fi

if [ -z "$1" ]; then
	echo "No parameter: default INSTALL_PREFIX (/usr/local)"
else
	echo "INSTALL_PREFIX=$1"
	sed -i "s@INSTALL_PREFIX = /usr/local@$1@" v/Config.mk
fi

sed -i "s@HOMEV = your_ponyprog_path/software/v@HOMEV = $CDHERE/v@" v/Config.mk
