//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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
#include <ctype.h>

#define MAXLINE 520

#define DATA_RECORD	00	//record contain the data bytes
#define END_RECORD	01	//record mark end of file
#define SEG_ADDR_RECORD	02	//record contain the new segmented address (HEX86)
#define START_RECORD	03	//record contain the program entry point (HEX86)
#define	LIN_ADDR_RECORD	04	//record contain the new linear address (HEX386)
#define EXT_START_RECORD	05	//record contain the program entry point (HEX386)

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


int IntelFileBuf::WriteRecord(FILE *fh, uint8_t *bptr, long curaddr, long recsize, int fmt)
{
	int rval = 1;

	int discard;
	int j;

	//check for empty data record (all FF's)
	if (fmt == DATA_RECORD)
	{
		discard = 1;
		for (j = 0; j < recsize; j++)
		{
			if ( bptr[curaddr + j] != 0xFF )
			{
				discard = 0;
				break;
			}
		}
	}
	else
		discard = 0;

	if (!discard)
	{
		int checksum = 0;
		int len = recsize;

		fprintf(fh, ":");

		//byte count
		fprintf(fh, "%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		fprintf(fh, "%04lX", curaddr & 0xFFFF);
		checksum += (curaddr >> 8) & 0xFF;
		checksum += curaddr & 0xFF;

		//record type
		fprintf(fh, "%02X", fmt & 0xFF);
		checksum += fmt & 0xFF;

		for (j = 0; j < recsize; j++)
		{
			fprintf(fh, "%02X", bptr[curaddr + j]);
			checksum += bptr[curaddr + j];
		}
		fprintf(fh, "%02X\n", (~checksum + 1) & 0xFF);
	}

	return rval;
}

int IntelFileBuf::WriteAddressRecord(FILE *fh, long curaddr, bool linear_address)
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

		if (linear_address)
		{
			//record type
			fprintf(fh, "%02X", LIN_ADDR_RECORD & 0xFF);
			checksum += LIN_ADDR_RECORD & 0xFF;

			//adjust extended linear address
			curaddr >>= 16;
		}
		else
		{
			//record type
			fprintf(fh, "%02X", SEG_ADDR_RECORD & 0xFF);
			checksum += SEG_ADDR_RECORD & 0xFF;

			//adjust extended segmented address
			curaddr >>= 4;
		}

		fprintf(fh, "%04lX", curaddr & 0xFFFF);
		checksum += (curaddr >> 8) & 0xFF;
		checksum += curaddr & 0xFF;

		fprintf(fh, "%02X\n", (~checksum + 1) & 0xFF);
	}

	return rval;
}


#define min(a,b)	( (a < b) ? a : b )

//======================>>> IntelFileBuf::Save <<<=======================
int IntelFileBuf::Save(int savetype, long relocation_offset)
{
	FILE *fh;
	int rval = OK;

	if ( (fh = fopen(FileBuf::GetFileName(), "w")) == NULL )
		return CREATEERROR;

	long dsize = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
	long size = FileBuf::GetBufSize();
	uint8_t *ptr = FileBuf::GetBufPtr();

	//Remove FF's tail
	while ( ptr[size-1] == 0xFF )
		size--;

	if (savetype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= dsize)
			size = GetSplitted();
		else
		{
			fclose(fh);
			return 0;
		}
	}
	else
	if (savetype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < dsize)
		{
			ptr += GetSplitted();
			size = dsize - GetSplitted();
		}
		else
		{
			fclose(fh);
			return 0;
		}
	}

	if (size > 0)
	{
		long curaddr = 0;

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

			if ( !WriteRecord(fh, ptr, curaddr, recsize, DATA_RECORD) )
			{
				rval = WRITEERROR;
				break;
			}
			curaddr += recsize;
		}
		WriteRecord(fh, ptr, 0, 0, END_RECORD);		//26/08/99

		rval = curaddr;
	}
	else
		rval = NOTHINGTOSAVE;

	fclose(fh);

	return rval;
}


//======================>>> IntelFileBuf::Load <<<=======================
int IntelFileBuf::Load(int loadtype, long relocation_offset)
{
	int rval = OK;
	int okline_counter = 0;

	uint8_t *endp = GetBufPtr() + GetBufSize();
	uint8_t *dp = GetBufPtr();
	if ( loadtype == DATA_TYPE)
	{
		if ( GetSplitted() >= 0 && GetSplitted() < GetBufSize() )
			dp += GetSplitted();
		else
			return 0;
	}

	//Relocation check
	if (dp + relocation_offset > endp)
		return BADPARAM;
	else
		dp += relocation_offset;

	uint32_t laddr = 0;

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
		uint16_t bcount;
		if ( ScanHex(&s, 2, bcount) != OK )
		{
			rval = BADFILETYPE;
			break;
		}
		uint8_t checksum = (uint8_t)bcount;

		//Address
		uint16_t addr;
		if ( ScanHex(&s, 4, addr) != OK )
		{
			rval = BADFILETYPE;
			break;
		}
		checksum += (uint8_t)(addr >> 8);
		checksum += (uint8_t)addr;

		//affect only low 16 bits of address
		laddr &= 0xFFFF0000;
		laddr |= addr;

		//Record Type
		uint16_t rectype;
		if ( ScanHex(&s, 2, rectype) != OK )
		{
			rval = BADFILETYPE;
			break;
		}
		checksum += (uint8_t)rectype;

		//Data Byte
		uint16_t data;

		if (rectype == DATA_RECORD)
		{
			//controllo overflow
			if (dp+laddr+bcount > endp)
			{
				rval = BUFFEROVERFLOW;
				break;
			}

			bool ok = true;
			uint8_t *p;
			for (k = 0, p = dp+laddr; k < bcount && ok; k++)
			{
				if ( ScanHex(&s, 2, data) != OK )
					ok = false;

				checksum += (uint8_t)data;
				*p++ = (uint8_t)data;
			}
			if (!ok)	//salta alla riga successiva
			{
				rval = BADFILETYPE;
				break;
			}
			img_size = laddr + bcount;
		}
		else if (rectype == SEG_ADDR_RECORD)
		{
			if (bcount != 2)
			{
				rval = BADFILETYPE;
				break;
			}
			else
			{
				//Address
				uint16_t addr;
				if ( ScanHex(&s, 4, addr) != OK )
				{
					rval = BADFILETYPE;
					break;
				}
				checksum += (uint8_t)(addr >> 8);
				checksum += (uint8_t)addr;

				laddr = (uint32_t)addr << 4;
			}
		}
		else if (rectype == LIN_ADDR_RECORD)
		{
			if (bcount != 2)
			{
				rval = BADFILETYPE;
				break;
			}
			else
			{
				//Address
				uint16_t addr;
				if ( ScanHex(&s, 4, addr) != OK )
				{
					rval = BADFILETYPE;
					break;
				}
				checksum += (uint8_t)(addr >> 8);
				checksum += (uint8_t)addr;

				laddr = (uint32_t)addr << 16;
			}
		}
	/**	just ignored
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
		//	ScanHex(&s, 8);
			while (bcount--)
			{
				data = (uint8_t)ScanHex(&s, 2);
				checksum += data;
			}
		}
	**/
		else	// Unknown record type: discard data bytes (but check for validity)
		{
			bool ok = true;
			while (bcount-- && ok)
			{
				if ( ScanHex(&s, 2, data) != OK )
					ok = false;
				checksum += (uint8_t)data;
			}
			if (!ok)
			{
				rval = BADFILETYPE;
				break;
			}
		}

		if ( ScanHex(&s, 2, data) != OK )
		{
			rval = BADFILETYPE;
			break;
		}
		if ( (uint8_t)data != (uint8_t)(~checksum + 1) )
		{
			rval = BADFILETYPE;
			break;
		}
		else
			okline_counter++;

		if (rectype == END_RECORD)
		{
			break;
		}
	}

	fclose(fh);

	if (okline_counter == 0)
		rval = BADFILETYPE;
	else
	{
		if (img_size == 0)	//nessun dato ma il formato e` corretto
			img_size++;
	}

//In questi formati di file "stupidi" la dimensione
//deve rimanere quella della eeprom attualmente selezionata

	if (rval == OK)
	{
	//	SetStringID("");	//????
		SetComment("");
		SetRollOver(0);		//2 (che significa NO) ??
	//	SetCRC( mcalc_crc(GetBufPtr(), img_size) );

		rval = img_size;
	}

	return rval;
}

/*Passandogli un indirizzo di stringa, converte le prime <len> cifre di tale
* stringa in Hesadecimali, incrementa il puntatore, e restituisce il numero.
* Attenzione! Poiche` il numero restituito e` uint32_t (4Byte), il numero max
* di <len> e` 8 (8 cifre esadecimali 0xABCDEF12).
*/
int IntelFileBuf::ScanHex(char **sp, int len, uint32_t &result)
{
	char cifra[20];
	int j;

	if (len > 8)
		return -2;
	for (j = 0; j < len && **sp; j++)
	{
		cifra[j] = *(*sp)++;
		if ( !isxdigit(cifra[j]) )
			return -1;
	}
	cifra[j] = '\0';
	result = strtoul(cifra, NULL, 16);

	return 0;
}

int IntelFileBuf::ScanHex(char **sp, int len, uint16_t &result)
{
	uint32_t res;
	int rval = ScanHex(sp, len, res);
	result = (uint16_t)res;

	return rval;
}
