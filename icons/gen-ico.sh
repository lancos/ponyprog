#!/bin/bash

SIZES="16 32 48 64"

for x in ${SIZES}
do
	inkscape --export-png logo${x}.png -w ${x} ponyprog.svg
	echo ${x}
done

for x in 16 32 48 64
do
	CMD="${CMD} logo${x}.png"
done
CMDX="${CMD} ponyprog.png"

convert ${CMDX} ponyprog.ico

rm ${CMD}
