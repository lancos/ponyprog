#!/bin/bash

which ftdi_eeprom >/dev/null
if [ $? -ne 0 ]; then
	echo "ftdi_eeprom command not found!"
	exit 1
fi

set -o errexit

FILECONF=ponyprogft.conf

if [ -f "${FILECONF}" ]; then
	NL=`grep -c -e "serial=\"FT[0-9]\+\"" "${FILECONF}"`
	if [ "$NL" == "1" ]; then
		ZNUM=`grep -e "serial=\"FT[0-9]\+\"" "${FILECONF}" | cut -d '=' -f 2 | cut -d '#' -f 1 | cut -d '"' -f 2 | cut -c 3-`
		#output e` il numero a 6 cifre preceduto da 0 per es. 000002
		#echo ${ZNUM}
		#rimuoviamo gli zeri inizali lasciandolo in base 10
		NUM=$((10#$ZNUM))
		#echo ${NUM}
		((NUM++)) || true
		NEWSERIAL=`printf "%06u" ${NUM}`
		echo "NewSerial: ${NEWSERIAL}"
		sed -i.old "s/serial=\"FT[0-9]\+\"/serial=\"FT${NEWSERIAL}\"/g" "${FILECONF}"

		ftdi_eeprom --device i:0x0403:0x6014 --flash-eeprom "${FILECONF}"
		if [ "$?" -eq 0 ]; then
			echo "Success!!!!!"
			exit 0
		else
			echo "Failed"
			#Restore original
			mv "${FILECONF}.old" "${FILECONF}"
			exit 3
		fi
	else
		echo "error NL != 1. $NL"
		exit 2
	fi
else
	echo "File non esiste: ${FILECONF}"
	exit 1
fi
