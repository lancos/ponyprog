//=========================================================================//
//-------------------------------------------------------------------------//
// intfbuf.cpp -- Source for IntelFileBuf (hex Intel file buffer)          //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
//                                                                         //
//-------------------------------------------------------------------------//
//                                                                         //
// This program is free software; you can redistribute it and/or           //
// modify it under the terms of the GNU  General Public License            //
// as published by the Free Software Foundation; either version2 of        //
// the License, or (at your option) any later version.                     //
//                                                                         //
// This program is distributed in the hope that it will be useful,         //
// but WITHOUT ANY WARRANTY; without even the implied warranty of          //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
// General Public License for more details.                                //
//                                                                         //
// You should have received a copy of the GNU  General Public License      //
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include <stdio.h>

//Estensione al formato Intel Extended HEX
#include <string.h>
#include <stdlib.h>

#define MAXLINE 255

#define DATA_RECORD	00	//Il record contiene i dati da caricare
#define END_RECORD	01	//Il record marca la fine del file
#define ADDRESS_RECORD	02	//Il record contiene il nuovo indirizzo a cui caricare
#define START_RECORD	03	//Il record contiene l'Entry-Point

#include "types.h"
#include "intfbuf.h"		// Header file
#include "errcode.h"
#include "crc.h"

//======================>>> IntelFileBuf::IntelFileBuf <<<=======================
IntelFileBuf::IntelFileBuf(e2AppWinInfo *wininfo)
		: FileBuf(wininfo)
{
	file_type = INTEL;
}

//======================>>> IntelFileBuf::~IntelFileBuf <<<=======================
IntelFileBuf::~IntelFileBuf()
{
}


int IntelFileBuf::WriteRecord(FILE *fh, long curaddr, long recsize, int fmt)
{
	int rval = 1;

	fprintf(fh, ":");

	if (rval)
	{
		int checksum = 0;
		int len = recsize;

		//byte count
		fprintf(fh, "%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		fprintf(fh, "%04lX", curaddr);
		checksum += (curaddr >> 8) & 0xFF;
		checksum += curaddr & 0xFF;

		//record type
		fprintf(fh, "%02X", fmt & 0xFF);
		checksum += fmt & 0xFF;

		int j;
		for (j = 0; j < recsize; j++)
		{
			fprintf(fh, "%02X", GetBufPtr()[curaddr + j]);
			checksum += GetBufPtr()[curaddr + j];
		}
		fprintf(fh, "%02X\n", (~checksum + 1) & 0xFF);
	}
	return rval;
}

int IntelFileBuf::WriteAddressRecord(FILE *fh, long curaddr)
{
	int rval = 1;

	fprintf(fh, ":");

	if (rval)
	{
		int checksum = 0;
		int len = 2;

		//byte count
		fprintf(fh, "%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		fprintf(fh, "%04X", 0);

		//record type
		fprintf(fh, "%02X", ADDRESS_RECORD & 0xFF);
		checksum += ADDRESS_RECORD & 0xFF;

		//adjust extended address
		curaddr >>= 4;

		fprintf(fh, "%04lX", curaddr);
		checksum += (curaddr >> 8) & 0xFF;
		checksum += curaddr & 0xFF;

		fprintf(fh, "%02X\n", (~checksum + 1) & 0xFF);
	}

	return rval;
}


#define min(a,b)	( (a < b) ? a : b )

//======================>>> IntelFileBuf::Save <<<=======================
int IntelFileBuf::Save()
{
	FILE *fh;
	int rval = OK;

	if ( (fh = fopen(FileBuf::GetFileName(), "w")) == NULL )
		return CREATEERROR;

	if (FileBuf::GetNoOfBlock() > 0)
	{
		long size = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
		long curaddr = 0;

	//	WriteRecord(fh, curaddr, 0, 0);
		while (curaddr < size)
		{
			//Write extended address record if needed
			if ( (curaddr / 0x10000) > 0 && (curaddr % 0x10000) == 0 )
				if ( !WriteAddressRecord(fh, curaddr) )
				{
					rval = WRITEERROR;
					break;
				}

			int recsize = min( (size - curaddr), 16 );

			if ( !WriteRecord(fh, curaddr, recsize, DATA_RECORD) )
			{
				rval = WRITEERROR;
				break;
			}
			curaddr += recsize;
		}
//		WriteRecord(fh, curaddr, 0, END_RECORD);
		WriteRecord(fh, 0, 0, END_RECORD);		//26/08/99

		rval = curaddr;
	}
	else
		rval = NOTHINGTOSAVE;

	fclose(fh);

	return rval;
}


//======================>>> IntelFileBuf::Load <<<=======================
int IntelFileBuf::Load(int bank)
{
	int rval = OK;

	BYTE *endp = GetBufPtr() + GetBufSize();
	BYTE *dp = GetBufPtr();
	if (bank)
		dp += GetSplitted();
	DWORD laddr = 0;

	FILE *fh;
	if ( (fh = fopen(GetFileName(), "r")) == NULL )
		return FILENOTFOUND;

	int img_size = 0;
	char riga[MAXLINE+1];
	riga[MAXLINE] = '\0';
	while ( fgets(riga, MAXLINE, fh) )
	{
		char *s;
		int k;

		if ( (s = strchr(riga, ':')) == NULL )
			continue;
		else
			s++;

		//Byte Count
		int bcount = (int)ScanHex(&s, 2);
		BYTE checksum = (BYTE)bcount;

		//Address
		WORD addr = (WORD)ScanHex(&s, 4);
		checksum += (BYTE)(addr >> 8);
		checksum += (BYTE)addr;

		//affect only low 16 bits of address
		laddr &= 0xFFFF0000;
		laddr |= addr;

		//Record Type
		BYTE rectype = (BYTE)ScanHex(&s, 2);
		checksum += rectype;

		//Data Byte
		BYTE data;

		if (rectype == DATA_RECORD)
		{
			//controllo overflow
			if (dp+laddr+bcount > endp)
			{
				rval = BUFFEROVERFLOW;
				break;
			}

			BYTE *p;
			for (k = 0, p = dp+laddr; k < bcount; k++)
			{
				data = (BYTE)ScanHex(&s, 2);
				checksum += data;
				*p++ = data;
			}
			img_size = laddr + bcount;
		}
		else if (rectype == ADDRESS_RECORD)
		{
			if (bcount != 2)
			{
				rval = BADFILETYPE;
				break;
			}
			else
			{
				//Address
				WORD addr = (WORD)ScanHex(&s, 4);
				checksum += (BYTE)(addr >> 8);
				checksum += (BYTE)addr;

				//We need a confirmation on how to calculate the
				//  address when we get a EXTENDED ADDRESS RECORD
				//laddr = (DWORD)addr << 16;
				laddr = (DWORD)addr << 4;

			//	rval = NOTSUPPORTED;
			}
		}
		else if (rectype == START_RECORD)
		{
			if (bcount != 4)
			{
				rval = BADFILETYPE;
				break;
			}

			// Lo possiamo ignorare, dovrebbe
			// essere il linker a mettere tutto
			// a posto. Noi non fungiamo da loader!
			//jmpaddr = ScanHex(&s, 8);
			ScanHex(&s, 8);
		}
		else if (rectype != END_RECORD)
		{
			rval = BADFILETYPE;
			break;
		}

		data = (BYTE)ScanHex(&s, 2);
		if ( data != (BYTE)(~checksum + 1) )
		{
			rval = BADFILETYPE;
			break;
		}

		if (rectype == END_RECORD)
		{
			break;
		}
	}

	fclose(fh);

//In questi formati di file "stupidi" la dimensione
//deve rimanere quella della eeprom attualmente selezionata

	if (rval == OK)
	{
	//	SetStringID("");	//????
		SetComment("");
		SetRollOver(0);		//2 (che significa NO) ??
		SetCRC( mcalc_crc(GetBufPtr(), img_size) );

		rval = img_size;
	}

	return rval;
}

/*Passandogli un indirizzo di stringa, converte le prime <len> cifre di tale
* stringa in Hesadecimali, incrementa il puntatore, e restituisce il numero.
* Attenzione! Poiche` il numero restituito e` ULONG (4Byte), il numero max
* di <len> e` 8 (8 cifre esadecimali 0xABCDEF12).
*/
DWORD IntelFileBuf::ScanHex(char **sp, int len)
{
	char cifra[20];
	int j;

	if (len > 8)
		len = 8;
	for (j = 0; j < len && **sp; j++)
		cifra[j] = *(*sp)++;
	cifra[j] = '\0';
	return strtoul(cifra, NULL, 16);
}
