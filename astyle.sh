#!/bin/bash

CMD=`type -p astyle`

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install astyle and restart this script"
   exit 1
fi

ASTYLE_VERSION="Artistic Style Version 3."

if [[ "`$CMD --version 2>&1`" != ${ASTYLE_VERSION}* ]]; then
        echo "Requested ${ASTYLE_VERSION}"
	exit 2
fi

set -e
 
export ARTISTIC_STYLE_OPTIONS="\
--mode=c \
--style=allman \
--indent=force-tab=4 \
--indent-modifiers \
--indent-switches \
--indent-preproc-define \
--pad-oper \
--pad-comma \
--unpad-paren \
--pad-header \
--align-pointer=name \
--align-reference=name \
--add-braces \
--formatted \
--lineend=linux"

$CMD $ARTISTIC_STYLE_OPTIONS --suffix=none --recursive  "SrcPony/*.cpp" "SrcPony/*.h"

#for i in $(find . -type f \( -name "*.cpp" -or -name "*.c"  -or -name "*.h" \));
#do
#    $CMD $ARTISTIC_STYLE_OPTIONS "$i";
#done

#for i in $(find . -type f \( -name "*.orig" -or -name "*~" -or -name "moc_*" \));
#do
#    rm -if "$i";
#done
