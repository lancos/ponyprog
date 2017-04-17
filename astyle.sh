#!/bin/bash

CPPCHCK=`type -p astyle`

if [ ! -f "$CPPCHCK" ]; then
   # not found exit
   echo "please install astyle and restart this script"
   exit 0
fi

set -e
 
export ARTISTIC_STYLE_OPTIONS="\
--mode=c \
--style=ansi \
--indent=tab=8\
--indent-classes \
--indent-cases \
--indent-col1-comments \
--indent-preprocessor \
--break-blocks \
--pad-oper \
--add-brackets \
--convert-tabs \
--formatted \
--lineend=linux"

for i in $(find . -type f \( -name "*.cpp" -or -name "*.c"  -or -name "*.h" \));
do
    astyle $ARTISTIC_STYLE_OPTIONS "$i";
done

for i in $(find . -type f \( -name "*.orig" -or -name "*~" -or -name "moc_*" \));
do
    rm -if "$i";
done
