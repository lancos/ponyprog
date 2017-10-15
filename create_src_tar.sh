#!/bin/bash

which git >/dev/null
if [ $? != 0 ]; then
	echo "Command git not found. Install git and try again."
	exit 1
fi

set -o errexit

BUILD_DATE=`date +%Y%m%d`

MYDIR=`pwd`
TMPDIR=`mktemp -d`
echo "${TMPDIR}"
cd "${TMPDIR}"
git clone --recursive https://github.com/lancos/ponyprog.git

if [ ! -d ponyprog ]; then
	echo "ponyprog dir not found!"
	exit 1
fi

echo "Creating tar..."
tar cfz "${MYDIR}/ponyprog-${BUILD_DATE}.tar.gz" --exclude=.git --exclude=InpOutLib ponyprog
echo "Done."

echo "Creating zip..."
zip -q -l --exclude='*/.git*' -r "${MYDIR}/ponyprog-${BUILD_DATE}.zip" ponyprog
echo "Done."

cd ${MYDIR}
rm -rf "${TMPDIR}"

exit 0
