//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
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
// along with this program (see LICENSE);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//=========================================================================//

// #include <stdio.h>
#include <QString>
#include <QTextStream>

// #include <stdlib.h>

#define MAXLINE 520

#define START_RECORD    '0'             //starting record (optional)
#define DATA_RECORD16   '1'             //16 bit address data record
#define DATA_RECORD24   '2'             //24 bit address data record
#define DATA_RECORD32   '3'             //32 bit address data record
#define SYMBOL_RECORD   '4'             //symbol record (LSI extension)
#define NBLOCK_RECORD   '5'             //number of data records in preceeding block
#define UNUSED_RECORD   '6'             //unused
#define END_RECORD32    '7'             //ending record for S3 records
#define END_RECORD24    '8'             //ending record for S2 records
#define END_RECORD16    '9'             //ending record for S1 records

#include "types.h"
#include "motsfbuf.h"           // Header file
#include "errcode.h"
#include "crc.h"

// #include "e2awinfo.h"

//======================>>> MotorolaSFileBuf::MotorolaSFileBuf <<<=======================
MotorolaSFileBuf::MotorolaSFileBuf(e2AppWinInfo *wininfo)
	: FileBuf(wininfo)
{
	file_type = MOTOS;

	highestPC = NULL;
	highestAddr = 0;
	lowestAddr = 0x7fffffff;
}

//======================>>> MotorolaSFileBuf::~MotorolaS1FileBuf <<<=======================
MotorolaSFileBuf::~MotorolaSFileBuf()
{
}


int MotorolaSFileBuf::WriteRecord(QTextStream &outs, uint8_t *bptr, long curaddr, long recsize, int fmt)
{
	int rval = 1;
	int len = 0;

	int j;
	int discard;

	//check for empty data record (all FF's)
	if (fmt == DATA_RECORD16 || fmt == DATA_RECORD24 || fmt == DATA_RECORD32)
	{
		discard = 1;

		for (j = 0; j < recsize; j++)
		{
			if (bptr[curaddr + j] != 0xFF)
			{
				discard = 0;
				break;
			}
		}
	}
	else
	{
		discard = 0;
	}

	if (discard)
	{
		return rval;
	}

	switch (fmt)
	{
	case START_RECORD:              //starting record (optional)
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	case DATA_RECORD16:             //16 bit address data record
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	case DATA_RECORD24:             //24 bit address data record
		len = recsize + 3 + 1;  //3 byte address + 1 byte checksum
		break;

	case DATA_RECORD32:             //32 bit address data record
		len = recsize + 4 + 1;  //4 byte address + 1 byte checksum
		break;

	case SYMBOL_RECORD:             //symbol record (LSI extension)
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	case NBLOCK_RECORD:             //number of data records in preceeding block
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	case UNUSED_RECORD:             //unused
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	case END_RECORD32:              //ending record for S3 records
		len = recsize + 4 + 1;  //4 byte address + 1 byte checksum
		break;

	case END_RECORD24:              //ending record for S2 records
		len = recsize + 3 + 1;  //3 byte address + 1 byte checksum
		break;

	case END_RECORD16:              //ending record for S1 records
		len = recsize + 2 + 1;  //2 byte address + 1 byte checksum
		break;

	default:
		rval = 0;
		break;
	}

	if (rval)
	{
		int checksum = 0;

		//type field
		outs << QString().sprintf("S%c", fmt);

		//len field
		outs << QString().sprintf("%02X", len & 0xFF);
		checksum += len & 0xFF;

		//addr field
		if (fmt == DATA_RECORD24 || fmt == END_RECORD24)
		{
			outs << QString().sprintf("%06lX", curaddr & 0xFFFFFF);
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else if (fmt == DATA_RECORD32 || fmt == END_RECORD32)
		{
			outs << QString().sprintf("%08lX", curaddr);
			checksum += (curaddr >> 24) & 0xFF;
			checksum += (curaddr >> 16) & 0xFF;
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}
		else        //all other have a 16 bit address field
		{
			outs << QString().sprintf("%04lX", curaddr & 0xFFFF);
			checksum += (curaddr >> 8) & 0xFF;
			checksum += curaddr & 0xFF;
		}

		for (j = 0; j < recsize; j++)
		{
			outs << QString().sprintf("%02X", bptr[curaddr + j]);
			checksum += bptr[curaddr + j];
		}

		outs << QString().sprintf("%02X\n", ~checksum & 0xFF);
	}

	return rval;
}

#define min(a,b)        ( (a < b) ? a : b )

//======================>>> MotorolaSFileBuf::Save <<<=======================
int MotorolaSFileBuf::Save(int savetype, long relocation_offset)
{
	QFile fh(FileBuf::GetFileName());
	int rval = OK;

	(void)relocation_offset;	//unused

	if (!fh.open(QIODevice::WriteOnly))
	{
		return CREATEERROR;
	}

	QTextStream out(&fh);

	long dsize = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
	long size = FileBuf::GetBufSize();
	uint8_t *ptr = FileBuf::GetBufPtr();

	//Remove FF's tail
	while (ptr[size - 1] == 0xFF)
	{
		size--;
	}

	if (savetype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= dsize)
		{
			size = GetSplitted();
		}
		else
		{
			fh.close();
			return 0;
		}
	}
	else if (savetype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < dsize)
		{
			ptr += GetSplitted();
			size = dsize - GetSplitted();
		}
		else
		{
			fh.close();
			return 0;
		}
	}

	if (size > 0)
	{
		char rectype;
		long curaddr = 0;

		WriteRecord(out, ptr, 0, 0, START_RECORD);

		while (curaddr < size)
		{
			int recsize = min((size - curaddr), 16);

			rectype = DATA_RECORD16;

			if (size > 0xffff)
			{
				rectype = DATA_RECORD24;
			}

			if (size > 0xffffff)
			{
				rectype = DATA_RECORD32;
			}

			if (!WriteRecord(out, ptr, curaddr, recsize, rectype))
			{
				rval = WRITEERROR;
				break;
			}

			curaddr += recsize;
		}

		rectype = END_RECORD16;

		if (size > 0xffff)
		{
			rectype = END_RECORD24;
		}

		if (size > 0xffffff)
		{
			rectype = END_RECORD32;
		}

		WriteRecord(out, ptr, 0, 0, rectype);

		rval = curaddr;
	}
	else
	{
		rval = NOTHINGTOSAVE;
	}

	fh.close();

	return rval;
}


//======================>>> MotorolaSFileBuf::Load <<<=======================
int MotorolaSFileBuf::Load(int loadtype, long relocation_offset)
{
	int rval = OK;
	int nocopy_mode = 0;

	uint8_t *endp = GetBufPtr() + GetBufSize();
	uint8_t *dp = GetBufPtr();

	if (loadtype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < GetBufSize())
		{
			dp += GetSplitted();
		}
		else
		{
			return 0;
		}
	}

	//Relocation check
	if (dp + relocation_offset > endp)
	{
		return BADPARAM;
	}
	else
	{
		dp += relocation_offset;
	}

	highestPC = NULL;
	highestAddr = 0;
	lowestAddr = 0x7fffffff;

	QFile fh(GetFileName());

	if (!fh.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return FILENOTFOUND;
	}

	valid_record_count = 0;

	char riga[MAXLINE + 1];
	riga[MAXLINE] = '\0';

	while (!fh.atEnd())
	{
		fh.readLine(riga, MAXLINE);

		if ((rval = ParseRecord(riga, dp, endp, 0, nocopy_mode)) != OK)
		{
			if (rval == BUFFEROVERFLOW)
			{
				nocopy_mode = 1;
			}
			else
			{
				break;
			}
		}
	}

	if (nocopy_mode)
	{
		if ((highestAddr - lowestAddr) > GetBufSize())
		{
			rval = BUFFEROVERFLOW;
		}
		else
		{
			long l_offset = lowestAddr;

			nocopy_mode = 0;

			highestPC = NULL;
			highestAddr = 0;
			lowestAddr = 0x7fffffff;

			fh.seek(0);
			//                      rewind(fh);

			riga[MAXLINE] = '\0';

			while (!fh.atEnd())
			{
				fh.readLine(riga, MAXLINE);

				if ((rval = ParseRecord(riga, dp, endp, l_offset, nocopy_mode)) != OK)
				{
					break;
				}
			}
		}
	}

	int img_size;
	//      img_size = 1;   //Srec file with only header and end record, no data, should return success
	img_size = valid_record_count ? 1 : 0;

	if (highestPC)
	{
		img_size = highestPC + 1 - dp;
	}

	fh.close();

	//This format doesn't contain information about the device size,
	// so keep the size of the selected eeprom

	if (rval == OK)
	{
		SetComment("");
		SetRollOver(0);         //2 (that means NO) ??
		//              SetCRC( mcalc_crc(GetBufPtr(), img_size) );

		rval = img_size;
	}

	return rval;
}

//======================>>> MotorolaSFileBuf::ParseRecord <<<=======================
/* Function: ParseRecord
**      Parses an S-record in the buffer and writes it into the buffer
**      if it is has a valid checksum.
**
** Args:        pointer to character buffer for null terminated line
** Returns:     int result code: 0 = success, else failure
*/
int MotorolaSFileBuf::ParseRecord(char *lbufPC, uint8_t *buf_startP, uint8_t *buf_endP, long offset, int nocopy)
{
	long addrL;
	uint8_t cksmB,              // checksum of addr, count, & data length
			*bufPC = 0;      // Pointer into memory array
	int i, countN,           // Number of bytes represented in record
		oheadN = 0,      // Number of overhead (addr + chksum) bytes
		tvalN;           // Temp for check checksum
	int unknow_rec = 0;

	if (lbufPC[0] == '\n' || lbufPC[0] == '\r')
	{
		return OK;        // discard empty lines
	}

	if (lbufPC[0] != 'S')
	{
		return BADFILETYPE;
	}

	switch (lbufPC[1])                              /* examine 2nd character on the line */
	{
	case DATA_RECORD16:                             /* 16 bit address field */
		if (sscanf(lbufPC, "S1%2X%4lX", &countN, &addrL) != 2)
		{
			return BADFILETYPE;        /* Flag error in S1 record */
		}

		oheadN = 2 + 1;                         /* 2 address + 1 checksum */
		break;

	case DATA_RECORD24:                             /* 24 bit address field */
		if (sscanf(lbufPC, "S2%2X%6lX", &countN, &addrL) != 2)
		{
			return BADFILETYPE;        /* Flag error in S2 record */
		}

		oheadN = 3 + 1;                         /* 3 address + 1 checksum */
		break;

	case DATA_RECORD32:                             /* 32 bit address field */
		if (sscanf(lbufPC, "S3%2X%8lX", &countN, &addrL) != 2)
		{
			return BADFILETYPE;        /* Flag error in S3 record */
		}

		oheadN = 4 + 1;                         /* 4 address + 1 checksum */
		break;

	/* ignore all but S1,2,3 records. */
	case START_RECORD:      //starting record (optional)
	case SYMBOL_RECORD:     //symbol record (LSI extension)
	case NBLOCK_RECORD:     //number of data records in preceeding block
	case UNUSED_RECORD:     //unused
	case END_RECORD32:      //ending record for S3 records
	case END_RECORD24:      //ending record for S2 records
	case END_RECORD16:      //ending record for S1 records

		//check only for correct checksum
		if (sscanf(lbufPC + 2, "%2X", &countN) != 1)
		{
			return BADFILETYPE;        /* Flag error in SX record */
		}

		unknow_rec = 1;
		break;

	default:                                // bad record
		return BADFILETYPE;
	}

	if (unknow_rec)
	{
		cksmB = 0;
		countN++;                   /* Bump counter to read final checksum too */

		for (i = 1; i <= countN; i++)
		{
			sscanf(lbufPC + i * 2, "%2X", &tvalN);   /* Scan a 2 hex digit byte  */
			cksmB += (uint8_t)tvalN;
		}

		if (++cksmB)
		{
			return BADFILETYPE;        /* flag checksum error */
		}
	}
	else
	{
		if (addrL > highestAddr)
		{
			highestAddr = addrL;
		}

		if (addrL < lowestAddr)
		{
			lowestAddr = addrL;
		}

		if (addrL < offset)
		{
			return BUFFERUNDERFLOW;
		}

		addrL -= offset;

		if (!nocopy)
		{
			bufPC = buf_startP + addrL;        // point to right spot in buffer
		}

		/* OK now see if checksum is OK, while reading data to buffer */
		cksmB = 0;
		countN++;                   /* Bump counter to read final checksum too */

		for (i = 1; i <= countN; i++)
		{
			sscanf(lbufPC + i * 2, "%2X", &tvalN);   /* Scan a 2 hex digit byte  */
			cksmB += (uint8_t)tvalN;

			if ((i > oheadN) && (i < countN))        /* If scanned a data byte */
			{
				if (!nocopy)
				{
					//Check for buffer overflow
					if (bufPC >= buf_endP)
					{
						return BUFFEROVERFLOW;
					}

					*bufPC++ = (uint8_t) tvalN;   // write it to the buffer
				}
			}
		}

		if (++cksmB)
		{
			return BADFILETYPE;        /* flag checksum error */
		}

		if (!nocopy)
		{
			if ((bufPC - 1) > highestPC)
			{
				highestPC = bufPC - 1;        /* track highest address loaded */
			}
		}
	}

	valid_record_count++;

	return OK;                        /* Successful return */
}
