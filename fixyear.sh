#!/bin/bash

set -o errexit

OLD_YEAR=2019
CURRENT_YEAR=`date +%Y`

FILESC=`find -name "*.cpp" -type f`
FILESH=`find -name "*.h" -type f`
FILES="${FILESC} ${FILESH} ponyprog.pro CMakeLists.txt distribution/innosetup/ponyprog.iss"

for i in ${FILES}
do
  if [ -f "${i}" ]; then
    echo "Converting ${i}"
    sed -i "s@1997-${OLD_YEAR}@1997-${CURRENT_YEAR}@g" "${i}"
  else
    echo "File non esiste: ${i}"
  fi
done

exit 0
