//=========================================================================//
//-------------------------------------------------------------------------//
// motsfbuf.cpp -- Source for MotorolaS FileBuf                            //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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

#include <string.h>
#include <stdlib.h>

#define MAXLINE 255

#define DATA_RECORD16	1	//Il record contiene i dati da caricare (indirizzi 16 bit)
#define	DATA_RECORD24	2
#define	DATA_RECORD32	3
#define END_RECORD		9	//Il record marca la fine del file
#define START_RECORD	0

#include "types.h"
#include "motsfbuf.h"		// Header file
#include "errcode.h"
#include "crc.h"

//======================>>> MotorolaS1FileBuf::MotorolaS1FileBuf <<<=======================
MotorolaSFileBuf::MotorolaSFileBuf(e2AppWinInfo *wininfo)
		: FileBuf(wininfo)
{
	file_type = MOTOS;
	highestPC = NULL;
}

//======================>>> MotorolaS1FileBuf::~MotorolaS1FileBuf <<<=======================
MotorolaSFileBuf::~MotorolaSFileBuf()
{
}


int MotorolaSFileBuf::WriteRecord(FILE *fh, long curaddr, long recsize, int fmt)
{
	int rval = 1;

	switch(fmt)
	{
	case START_RECORD:		//Header
	case END_RECORD:		//Bottom
	case DATA_RECORD16:
	case DATA_RECORD24:
	case DATA_RECORD32:
		fprintf(fh, "S%d", fmt);
		break;
	default:
		rval = 0;
		break;
	}

	if (rval)
	{
		int checksum = 0;
		int len = recsize + 3;

		//len field
		fprintf(fh, "%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		if (fmt == DATA_RECORD24)
		{
			fprintf(fh, "%06lX", curaddr);
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else
		if (fmt == DATA_RECORD32)
		{
			fprintf(fh, "%08lX", curaddr);
			checksum += (curaddr >> 24) & 0xFF;
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else
		{
			fprintf(fh, "%04lX", curaddr);
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}

		int j;
		for (j = 0; j < recsize; j++)
		{
			fprintf(fh, "%02X", GetBufPtr()[curaddr + j]);
			checksum += GetBufPtr()[curaddr + j];
		}
		fprintf(fh, "%02X\n", ~checksum & 0xFF);
	}
	return rval;
}

#define min(a,b)	( (a < b) ? a : b )

//======================>>> MotorolaS1FileBuf::Save <<<=======================
int MotorolaSFileBuf::Save()
{
	FILE *fh;
	int rval = OK;

	if ( (fh = fopen(FileBuf::GetFileName(), "w")) == NULL )
		return CREATEERROR;

	if (FileBuf::GetNoOfBlock() > 0)
	{
		long size = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
		long curaddr = 0;

		WriteRecord(fh, curaddr, 0, START_RECORD);
		while (curaddr < size)
		{
			int recsize = min( (size - curaddr), 16 );

			if ( !WriteRecord(fh, curaddr, recsize, 1) )
			{
				rval = WRITEERROR;
				break;
			}
			curaddr += recsize;
		}
		WriteRecord(fh, curaddr, 0, END_RECORD);

		rval = curaddr;
	}
	else
		rval = NOTHINGTOSAVE;

	fclose(fh);

	return rval;
}


//======================>>> MotorolaS1FileBuf::Load <<<=======================
int MotorolaSFileBuf::Load(int bank)
{
	int rval = OK;

//	BYTE *endp = GetBufPtr() + GetBufSize();
	BYTE *dp = GetBufPtr();
	if (bank)
		dp += GetSplitted();

	FILE *fh;
	if ( (fh = fopen(GetFileName(), "r")) == NULL )
		return FILENOTFOUND;

	char riga[MAXLINE+1];
	riga[MAXLINE] = '\0';
	while ( fgets(riga, MAXLINE, fh) )
	{
		if ( (rval = ParseRecord(riga, dp)) != OK )
			break;
	}

	int img_size = highestPC + 1 - GetBufPtr();
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

/* Function: ParseRecord
**      Parses an S-record in the buffer and writes it into the buffer
**      if it is has a valid checksum.
**
** Args:        pointer to character buffer for null terminated line
** Returns:     int result code: 0 = success, else failure
*/
int MotorolaSFileBuf::ParseRecord(char *lbufPC, BYTE *bufAC)
{
	long addrL;
	BYTE cksmB,        /* checksum of addr, count, & data length */
		*bufPC;                 /* Pointer into memory array */
	int i, countN,              /* Number of bytes represented in record */
		oheadN,                 /* Number of overhead (addr + chksum) bytes */
		tvalN;                  /* Temp for check checksum */

	if (lbufPC[0] != 'S')
		return BADFILETYPE;

	switch( lbufPC[1] )       /* examine 2nd character on the line */
	{
	case '0':
		if ( sscanf(lbufPC, "S0%2X", &countN ) == 1 )
		{
			return OK;
		}
		else
			return BADFILETYPE;

	case '9':					/* END record */
		return OK;

	case '1':                   /* 16 bit address field */
		if( sscanf(lbufPC, "S1%2X%4lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;       /* Flag error in S1 record */
		oheadN = 3;             /* 2 address + 1 checksum */
		break;

	case '2':                   /* 24 bit address field */
		if( sscanf(lbufPC, "S2%2X%6lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;       /* Flag error in S2 record */
		oheadN = 4;             /* 3 address + 1 checksum */
		break;

	case '3':                   /* 32 bit address field */
		if( sscanf(lbufPC, "S3%2X%8lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;       /* Flag error in S3 record */
		oheadN = 5;             /* 4 address + 1 checksum */
		break;

	default:                    /* ignore all but S1,2,3 records. */
		return BADFILETYPE;
	}

	if ( addrL >= GetBufSize() )
		return BUFFEROVERFLOW; /* if address exceeds buffer size */

	bufPC = bufAC + addrL;      /* otherwise, point to right spot in buffer */

	/* OK now see if checksum is OK, while reading data to buffer */
	cksmB = 0;
	countN++;                   /* Bump counter to read final checksum too */
	for( i = 1; i <= countN; i++ )
	{
		sscanf( lbufPC + i*2, "%2X", &tvalN );  /* Scan a 2 hex digit byte  */
		cksmB += (BYTE)tvalN;
		if( ( i > oheadN ) && ( i < countN ) )  /* If scanned a data byte */
			*bufPC++ = (BYTE) tvalN;   /* write it to the buffer */
	}

	if( ++cksmB )
		return BADFILETYPE;      /* flag checksum error */

	if( (bufPC - 1) > highestPC )
		highestPC = bufPC - 1;          /* track highest address loaded */

	return OK;                        /* Successful return */
}
