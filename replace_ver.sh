#!/bin/bash

set -o errexit

OLDVER="2.08d"
#OLDVER="2\.[0-9][0-9][a-z]"
NEWVER="2.08e"

FILES=`grep -r -l -I --exclude-dir=".git" "${OLDVER}"`

echo "Versione ${OLDVER} --> ${NEWVER}"

for i in ${FILES}
do
	if [ ! -x "${i}" ]; then
		if [ -f "${i}" ]; then
			echo "Sostituzione in ${i}"
			sed -i "s/${OLDVER}/${NEWVER}/g" "${i}"
		else
			echo "File doesn't exist ${i}"
		fi
	fi
done
