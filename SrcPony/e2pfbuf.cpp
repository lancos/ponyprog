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
#include <QDataStream>

#include "e2pfbuf.h"            // Header file
#include "crc.h"
#include "errcode.h"

#include "e2phead.h"
#include "eeptypes.h"


static char const *id_string = "E2P!Lanc";

//======================>>> e2pFileBuf::e2pFileBuf <<<=======================
e2pFileBuf::e2pFileBuf(e2AppWinInfo *wininfo)
	: FileBuf(wininfo)
{
	file_type = E2P;
	Q_ASSERT_X(sizeof(struct e2pHeader) == 152, "e2pFileBuf", "Bad size for e2pHeader");
}

//======================>>> e2pFileBuf::~e2pFileBuf <<<=======================
e2pFileBuf::~e2pFileBuf()
{
}

/***
void e2pFileBuf::check_offsets()
{
        printf("fileID: %lu\n", offsetof(e2pHeader, fileID));
        printf("e2pFuseBits: %lu\n", offsetof(e2pHeader, e2pFuseBits));
        printf("e2pLockBits: %lu\n", offsetof(e2pHeader, e2pLockBits));
        printf("e2pType: %lu\n", offsetof(e2pHeader, e2pType));
        printf("e2pSize: %lu\n", offsetof(e2pHeader, e2pSize));
        printf("flags: %lu\n", offsetof(e2pHeader, flags));
        printf("e2pExtFuseBits: %lu\n", offsetof(e2pHeader, e2pExtFuseBits));
        printf("e2pExtLockBits: %lu\n", offsetof(e2pHeader, e2pExtLockBits));
        printf("fversion: %lu\n", offsetof(e2pHeader, fversion));
        printf("split_size_Low: %lu\n", offsetof(e2pHeader, split_size_Low));
        printf("e2pStringID: %lu\n", offsetof(e2pHeader, e2pStringID));
        printf("e2pProgBits: %lu\n", offsetof(e2pHeader, e2pProgBits));
        printf("e2pComment: %lu\n", offsetof(e2pHeader, e2pComment));
        printf("split_size_High: %lu\n", offsetof(e2pHeader, split_size_High));
        printf("pad: %lu\n", offsetof(e2pHeader, pad));
        printf("e2pCrc: %lu\n", offsetof(e2pHeader, e2pCrc));
        printf("headCrc: %lu\n", offsetof(e2pHeader, headCrc));

        if (sizeof(struct e2pHeader) != 152)
                printf("Bad E2P Header size: %lu\n", sizeof(struct e2pHeader));
        else
                printf("E2P Header size OK (%lu)\n", sizeof(struct e2pHeader));
}
***/

//======================>>> e2pFileBuf::Load <<<=======================
int e2pFileBuf::Load(int loadtype, long relocation_offset)
{
	extern int GetE2PSubType(unsigned long x);
	extern int GetE2PPriType(unsigned long x);

	QFile fh(FileBuf::GetFileName());
	e2pHeader hdr;
	int rval;

	if (!fh.open(QIODevice::ReadOnly))
	{
		return FILENOTFOUND;
	}

	QDataStream datastream(&fh);

	// Controlla il tipo di file
	if (datastream.readRawData((char *)&hdr, sizeof(e2pHeader)) &&
			strncmp(hdr.fileID, id_string, E2P_ID_SIZE) == 0)
	{
		unsigned char *localbuf;
		localbuf = new unsigned char[hdr.e2pSize];

		if (localbuf)
		{
			//Controlla il CRC dell'Header
			if (mcalc_crc(&hdr, sizeof(hdr) - sizeof(hdr.headCrc)) == hdr.headCrc &&
					//Check for CRC in memory
					fcalc_crc(fh, sizeof(e2pHeader), 0) == hdr.e2pCrc &&
					//read buffer
					datastream.readRawData((char *)localbuf, hdr.e2pSize))
				//                      fread(FileBuf::GetBufPtr(), hdr.e2pSize, 1, fh) )
			{
				SetEEpromType(hdr.e2pType);  //set eeprom device type (and block size too)
				//FileBuf::SetNoOfBlock( hdr.e2pSize / FileBuf::GetBlockSize() );

				if (hdr.fversion > 0)
				{
					SetLockBits(((uint32_t)hdr.e2pExtLockBits << 8) | hdr.e2pLockBits);
					SetFuseBits(((uint32_t)hdr.e2pExtFuseBits << 8) | hdr.e2pFuseBits);
				}
				else
				{
					//Old file version
					if (GetE2PPriType(hdr.e2pType) == PIC16XX ||
							GetE2PPriType(hdr.e2pType) == PIC168XX ||
							GetE2PPriType(hdr.e2pType) == PIC125XX)
					{
						SetLockBits(((uint32_t)hdr.e2pLockBits << 8) | hdr.e2pFuseBits);
					}
					else
					{
						SetLockBits(hdr.e2pLockBits);
						SetFuseBits(hdr.e2pFuseBits);
					}

				}

				if (hdr.fversion > 1)
				{
					SetSplitted(((uint32_t)hdr.split_size_High << 16) | hdr.split_size_Low);
				}
				else
				{
					SetSplitted(hdr.split_size_Low);
				}

				SetStringID(hdr.e2pStringID);
				SetComment(hdr.e2pComment);
				SetRollOver(hdr.flags & 7);
				SetCRC(hdr.e2pCrc);

				//Copy the content into the buffer
				if (loadtype == ALL_TYPE)
				{
					if (hdr.e2pSize <= GetBufSize())
					{
						memcpy(FileBuf::GetBufPtr(), localbuf, hdr.e2pSize);
					}
				}
				else if (loadtype == PROG_TYPE)
				{
					long s = GetSplitted();

					if (s <= 0)
					{
						s = hdr.e2pSize;
					}

					//if splittedInfo == 0 then copy ALL
					if (s <= hdr.e2pSize && s <= GetBufSize())
					{
						memcpy(FileBuf::GetBufPtr(), localbuf, s);
					}
				}
				else if (loadtype == DATA_TYPE)
				{
					long s = GetSplitted();

					if (s >= 0 &&
							s < hdr.e2pSize &&
							hdr.e2pSize <= GetBufSize())
					{
						memcpy(FileBuf::GetBufPtr() + s, localbuf + s, hdr.e2pSize - s);
					}
				}

				rval = GetNoOfBlock();
			}
			else
			{
				rval = READERROR;
			}

			delete[] localbuf;
		}
		else
		{
			rval = OUTOFMEMORY;
		}
	}
	else
	{
		rval = BADFILETYPE;
	}

	fh.close();

	return rval;
}

//======================>>> e2pFileBuf::Save <<<=======================
int e2pFileBuf::Save(int savetype, long relocation_offset)
{
	e2pHeader hdr;
	int rval;

	if (FileBuf::GetNoOfBlock() <= 0)
	{
		return NOTHINGTOSAVE;
	}

	QFile fh(FileBuf::GetFileName());

	//Header settings
	memset(&hdr, 0, sizeof(hdr));           //Clear all to zero first
	strncpy(hdr.fileID, id_string, E2P_ID_SIZE);    //Id
	hdr.e2pSize = FileBuf::GetNoOfBlock() * FileBuf::GetBlockSize();

	unsigned char *localbuf;
	localbuf = new unsigned char[hdr.e2pSize];

	if (localbuf)
	{
		long s = GetSplitted();

		memset(localbuf, 0xff, hdr.e2pSize);

		if (fh.exists())
		{
			if (!fh.open(QIODevice::ReadOnly))
			{
				delete[] localbuf;
				return CREATEERROR;
			}
			QDataStream datastream(&fh);

			//Initialize local buffer
			//  if the file already exist read the current content
			//  otherwise set the localbuffer to 0xFF
			if (fh.seek(sizeof(hdr)))
			{
				datastream.readRawData((char *)localbuf, hdr.e2pSize);
			}
			fh.close();
		}

		if (savetype == ALL_TYPE)
		{
			memcpy(localbuf, FileBuf::GetBufPtr(), hdr.e2pSize);
		}
		else if (savetype == DATA_TYPE)
		{
			if (hdr.e2pSize > s)
			{
				memcpy(localbuf + s, FileBuf::GetBufPtr() + s, hdr.e2pSize - s);
			}
		}
		else if (savetype == PROG_TYPE)
		{
			if (s > 0 &&  s <= hdr.e2pSize)
			{
				memcpy(localbuf, FileBuf::GetBufPtr(), s);
			}
		}

		hdr.fversion = E2P_FVERSION;

		hdr.e2pLockBits = (uint8_t)(GetLockBits() & 0xFF);
		hdr.e2pExtLockBits = (uint16_t)(GetLockBits() >> 8);
		hdr.e2pFuseBits = (uint8_t)(GetFuseBits() & 0xFF);
		hdr.e2pExtFuseBits = (uint16_t)(GetFuseBits() >> 8);

		hdr.e2pType = GetEEpromType();
		strncpy(hdr.e2pStringID, GetStringID().toLatin1().constData(), 28);
		strncpy(hdr.e2pComment, GetComment().toLatin1().constData(), 85);
		hdr.flags = GetRollOver() & 7;
		hdr.split_size_Low = (uint16_t)GetSplitted();
		hdr.split_size_High = (uint16_t)(GetSplitted() >> 16);
		hdr.e2pCrc = mcalc_crc(localbuf, hdr.e2pSize);
		hdr.headCrc = mcalc_crc(&hdr, sizeof(hdr) - sizeof(hdr.headCrc));

		if (fh.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			QDataStream datastream(&fh);

			//Write to file
			if (datastream.writeRawData((char *)&hdr, sizeof(hdr)) &&         //Write the header
					datastream.writeRawData((char *)localbuf, hdr.e2pSize))   //Write the buffer
			{
				rval = GetNoOfBlock();
			}
			else
			{
				rval = WRITEERROR;
			}
			fh.close();
		}
		else
		{
			rval = CREATEERROR;
		}

		delete[] localbuf;
	}
	else
	{
		rval = OUTOFMEMORY;
	}

	return rval;
}
