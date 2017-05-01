#!/bin/bash

CMD=`type -p astyle`

if [ ! -x "$CMD" ]; then
   # not found exit
   echo "please install astyle and restart this script"
   exit 1
fi

ASTYLE2_VERSION="Artistic Style Version 2."
ASTYLE3_VERSION="Artistic Style Version 3."

ARTISTIC_STYLE_OPTIONS="\
--mode=c \
--style=allman \
--indent=force-tab=4 \
--indent-modifiers \
--indent-preproc-define \
--pad-oper \
--unpad-paren \
--pad-header \
--align-pointer=name \
--align-reference=name \
--formatted \
--lineend=linux"

if [[ "`$CMD --version 2>&1`" == ${ASTYLE3_VERSION}* ]]; then

ARTISTIC_STYLE_OPTIONS="$ARTISTIC_STYLE_OPTIONS --pad-comma --add-braces"

elif [[ "`$CMD --version 2>&1`" == ${ASTYLE2_VERSION}* ]]; then

ARTISTIC_STYLE_OPTIONS="$ARTISTIC_STYLE_OPTIONS --add-brackets"

else
	echo "Requested version 2 or 3"
	exit 2
fi

set -e

$CMD $ARTISTIC_STYLE_OPTIONS --suffix=none --recursive  "SrcPony/*.cpp" "SrcPony/*.h"

#for i in $(find . -type f \( -name "*.cpp" -or -name "*.c"  -or -name "*.h" \));
#do
#    $CMD $ARTISTIC_STYLE_OPTIONS "$i";
#done

#for i in $(find . -type f \( -name "*.orig" -or -name "*~" -or -name "moc_*" \));
#do
#    rm -if "$i";
#done
