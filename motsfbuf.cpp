//=========================================================================//
//-------------------------------------------------------------------------//
// motsfbuf.cpp -- Source for MotorolaS FileBuf                            //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000  Claudio Lanconelli                            //
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

#define START_RECORD	'0'		//starting record (optional)
#define DATA_RECORD16	'1'		//16 bit address data record
#define	DATA_RECORD24	'2'		//24 bit address data record
#define	DATA_RECORD32	'3'		//32 bit address data record
#define	SYMBOL_RECORD	'4'		//symbol record (LSI extension)
#define	NBLOCK_RECORD	'5'		//number of data records in preceeding block
#define	UNUSED_RECORD	'6'		//unused
#define	END_RECORD32	'7'		//ending record for S3 records
#define	END_RECORD24	'8'		//ending record for S2 records
#define	END_RECORD16	'9'		//ending record for S1 records

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
	highestAddr = 0;
	lowestAddr = 0x7fffffff;
}

//======================>>> MotorolaS1FileBuf::~MotorolaS1FileBuf <<<=======================
MotorolaSFileBuf::~MotorolaSFileBuf()
{
}


int MotorolaSFileBuf::WriteRecord(FILE *fh, long curaddr, long recsize, char fmt)
{
	int rval = 1;
	int len = 0;

	switch(fmt)
	{
	case START_RECORD:		//starting record (optional)
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	case DATA_RECORD16:		//16 bit address data record
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	case DATA_RECORD24:		//24 bit address data record
		len = recsize + 3 + 1;	//3 byte address + 1 byte checksum
		break;
	case DATA_RECORD32:		//32 bit address data record
		len = recsize + 4 + 1;	//4 byte address + 1 byte checksum
		break;
	case SYMBOL_RECORD:		//symbol record (LSI extension)
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	case NBLOCK_RECORD:		//number of data records in preceeding block
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	case UNUSED_RECORD:		//unused
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	case END_RECORD32:		//ending record for S3 records
		len = recsize + 4 + 1;	//4 byte address + 1 byte checksum
		break;
	case END_RECORD24:		//ending record for S2 records
		len = recsize + 3 + 1;	//3 byte address + 1 byte checksum
		break;
	case END_RECORD16:		//ending record for S1 records
		len = recsize + 2 + 1;	//2 byte address + 1 byte checksum
		break;
	default:
		rval = 0;
		break;
	}

	if (rval)
	{
		int checksum = 0;

		//type field
		fprintf(fh, "S%c", fmt);

		//len field
		fprintf(fh, "%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		if (fmt == DATA_RECORD24 || fmt == END_RECORD24)
		{
			fprintf(fh, "%06lX", curaddr);
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else
		if (fmt == DATA_RECORD32 || fmt == END_RECORD32)
		{
			fprintf(fh, "%08lX", curaddr);
			checksum += (curaddr >> 24) & 0xFF;
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else	//all other have a 16 bit address field
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
		char rectype;
		long size = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
		long curaddr = 0;

		WriteRecord(fh, 0, 0, START_RECORD);
		while (curaddr < size)
		{
			int recsize = min( (size - curaddr), 16 );

			rectype = DATA_RECORD16;
			if (size > 0xffff)
				rectype = DATA_RECORD24;
			if (size > 0xffffff)
				rectype = DATA_RECORD32;

			if ( !WriteRecord(fh, curaddr, recsize, rectype) )
			{
				rval = WRITEERROR;
				break;
			}
			curaddr += recsize;
		}

		rectype = END_RECORD16;
		if (size > 0xffff)
			rectype = END_RECORD24;
		if (size > 0xffffff)
			rectype = END_RECORD32;
		WriteRecord(fh, 0, 0, rectype);

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
	int nocopy_mode = 0;

	BYTE *dp = GetBufPtr();
	if (bank)
		dp += GetSplitted();

	highestPC = NULL;
	highestAddr = 0;
	lowestAddr = 0x7fffffff;

	FILE *fh;
	if ( (fh = fopen(GetFileName(), "r")) == NULL )
		return FILENOTFOUND;

	char riga[MAXLINE+1];
	riga[MAXLINE] = '\0';
	while ( fgets(riga, MAXLINE, fh) )
	{
		if ( (rval = ParseRecord(riga, dp, 0, nocopy_mode)) != OK )
		{
			if (rval == BUFFEROVERFLOW)
			{
				nocopy_mode = 1;
			}
			else
				break;
		}
	}

	if (nocopy_mode)
	{
		if ( (highestAddr - lowestAddr) > GetBufSize() )
			rval = BUFFEROVERFLOW;
		else
		{
			long l_offset = lowestAddr;

			nocopy_mode = 0;

			highestPC = NULL;
			highestAddr = 0;
			lowestAddr = 0x7fffffff;

			rewind(fh);

			riga[MAXLINE] = '\0';
			while ( fgets(riga, MAXLINE, fh) )
			{
				if ( (rval = ParseRecord(riga, dp, l_offset, nocopy_mode)) != OK )
					break;
			}
		}
	}

	int img_size = highestPC + 1 - GetBufPtr();
	fclose(fh);

//In questi formati di file "stupidi" la dimensione
//deve rimanere quella della eeprom attualmente selezionata

	if (rval == OK)
	{
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
int MotorolaSFileBuf::ParseRecord(char *lbufPC, BYTE *bufAC, long offset, int nocopy)
{
	long addrL;
	BYTE cksmB,        /* checksum of addr, count, & data length */
		*bufPC = 0;                 /* Pointer into memory array */
	int i, countN,              /* Number of bytes represented in record */
		oheadN,                 /* Number of overhead (addr + chksum) bytes */
		tvalN;                  /* Temp for check checksum */
	int unknow_rec = 0;

	if (lbufPC[0] == '\n' || lbufPC[0] == '\r')
	    return OK;          /* discard empty lines */

	if (lbufPC[0] != 'S')
		return BADFILETYPE;

	switch( lbufPC[1] )			/* examine 2nd character on the line */
	{
	case DATA_RECORD16:			/* 16 bit address field */
		if( sscanf(lbufPC, "S1%2X%4lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;	/* Flag error in S1 record */
		oheadN = 2 + 1;			/* 2 address + 1 checksum */
		break;

	case DATA_RECORD24:			/* 24 bit address field */
		if( sscanf(lbufPC, "S2%2X%6lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;	/* Flag error in S2 record */
		oheadN = 3 + 1;			/* 3 address + 1 checksum */
		break;

	case DATA_RECORD32:			/* 32 bit address field */
		if( sscanf(lbufPC, "S3%2X%8lX", &countN, &addrL ) != 2 )
			return BADFILETYPE;	/* Flag error in S3 record */
		oheadN = 4 + 1;			/* 4 address + 1 checksum */
		break;

	/* ignore all but S1,2,3 records. */
	case START_RECORD:	//starting record (optional)
	case SYMBOL_RECORD:	//symbol record (LSI extension)
	case NBLOCK_RECORD:	//number of data records in preceeding block
	case UNUSED_RECORD:	//unused
	case END_RECORD32:	//ending record for S3 records
	case END_RECORD24:	//ending record for S2 records
	case END_RECORD16:	//ending record for S1 records
								/* check only for correct checksum */
		if( sscanf(lbufPC+2, "%2X", &countN ) != 1 )
			return BADFILETYPE;       /* Flag error in SX record */
		unknow_rec = 1;
		break;

	default:	/* bad record */
		return BADFILETYPE;
	}

	if (unknow_rec)
	{
    	cksmB = 0;
    	countN++;                   /* Bump counter to read final checksum too */
    	for( i = 1; i <= countN; i++ )
    	{
    		sscanf( lbufPC + i*2, "%2X", &tvalN );  /* Scan a 2 hex digit byte  */
    		cksmB += (BYTE)tvalN;
    	}
    	
    	if( ++cksmB )
    		return BADFILETYPE;      /* flag checksum error */
	}
	else
	{
		if ( addrL > highestAddr)
			highestAddr = addrL;
		if ( addrL < lowestAddr)
			lowestAddr = addrL;

		if (addrL < offset)
			return BUFFERUNDERFLOW;

		addrL -= offset;

		if (!nocopy)
		{
			if ( addrL >= GetBufSize() )
				return BUFFEROVERFLOW; /* if address exceeds buffer size */

			bufPC = bufAC + addrL;      /* otherwise, point to right spot in buffer */
		}

		/* OK now see if checksum is OK, while reading data to buffer */
		cksmB = 0;
		countN++;                   /* Bump counter to read final checksum too */
		for( i = 1; i <= countN; i++ )
		{
			sscanf( lbufPC + i*2, "%2X", &tvalN );  /* Scan a 2 hex digit byte  */
			cksmB += (BYTE)tvalN;
			if( ( i > oheadN ) && ( i < countN ) )  /* If scanned a data byte */
				if (!nocopy)
					*bufPC++ = (BYTE) tvalN;   /* write it to the buffer */
		}

		if( ++cksmB )
			return BADFILETYPE;      /* flag checksum error */

		if (!nocopy)
		{
			if( (bufPC - 1) > highestPC )
				highestPC = bufPC - 1;          /* track highest address loaded */
		}
    }

	return OK;                        /* Successful return */
}
