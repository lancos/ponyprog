//=========================================================================//
//-------------------------------------------------------------------------//
// e2pfbuf.cpp -- Source for e2pFileBuf class (derived from File buffer)   //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#include "e2pfbuf.h"		// Header file
#include "crc.h"
#include "errcode.h"

#include "e2phead.h"

static char const *id_string = "E2P!Lanc";

//======================>>> e2pFileBuf::e2pFileBuf <<<=======================
e2pFileBuf::e2pFileBuf(e2AppWinInfo *wininfo)
		: FileBuf(wininfo)
{
	file_type = E2P;
}

//======================>>> e2pFileBuf::~e2pFileBuf <<<=======================
e2pFileBuf::~e2pFileBuf()
{
}

//======================>>> e2pFileBuf::Load <<<=======================
int e2pFileBuf::Load(int bank)
{
	int GetE2PSubType(long x);
	int GetE2PPriType(long x);

	FILE *fh;
	e2pHeader hdr;
	int rval;

	if ( (fh = fopen(FileBuf::GetFileName(), "rb")) == NULL )
		return FILENOTFOUND;

	// Controlla il tipo di file
	if ( fread(&hdr, sizeof(e2pHeader), 1, fh) &&
				strncmp(hdr.fileID, id_string, 8) == 0 )
	{
			//Controlla il CRC dell'Header
		if ( mcalc_crc(&hdr, sizeof(hdr)-sizeof(hdr.headCrc)) == hdr.headCrc &&
			//Controlla il CRC della memoria
			fcalc_crc(fh, sizeof(e2pHeader), 0) == hdr.e2pCrc &&
			//Legge il contenuto nel buffer
			fread(FileBuf::GetBufPtr(), hdr.e2pSize, 1, fh) )
		{
			SetEEpromType(GetE2PPriType(hdr.e2pType), GetE2PSubType(hdr.e2pType));	//Questa imposta il tipo di eeprom, e indirettamente la dimensione del block
//			FileBuf::SetNoOfBlock( hdr.e2pSize / FileBuf::GetBlockSize() );

			SetLockBits(hdr.e2pLockBits);
			SetFuseBits(hdr.e2pFuseBits);

			SetStringID(hdr.e2pStringID);
			SetComment(hdr.e2pComment);
			SetRollOver(hdr.flags & 7);
			SetSplitted(hdr.split_size);
			SetCRC(hdr.e2pCrc);

			rval = GetNoOfBlock();
		}
		else
			rval = READERROR;
	}
	else
		rval = BADFILETYPE;

	fclose(fh);
	return rval;
}

//======================>>> e2pFileBuf::Save <<<=======================
int e2pFileBuf::Save()
{
	FILE *fh;
	e2pHeader hdr;
	int rval;

	if (FileBuf::GetNoOfBlock() <= 0)
		return NOTHINGTOSAVE;

	if ( (fh = fopen(FileBuf::GetFileName(), "w+b")) == NULL )
		return FILENOTFOUND;

	// Prepara l'header
	memset(&hdr, 0, sizeof(hdr));
	strncpy(hdr.fileID, id_string, 8);
	hdr.e2pSize = FileBuf::GetNoOfBlock() * FileBuf::GetBlockSize();

	//	hdr.e2pBusType = BUSTYPE_I2C;
	hdr.e2pLockBits = GetLockBits();
	hdr.e2pFuseBits = GetFuseBits();

	hdr.e2pType = GetEEpromType();
	GetStringID(hdr.e2pStringID);
	GetComment(hdr.e2pComment);
	hdr.flags = GetRollOver() & 7;
	hdr.split_size = GetSplitted();
	hdr.e2pCrc = mcalc_crc(FileBuf::GetBufPtr(), hdr.e2pSize);
	hdr.headCrc = mcalc_crc(&hdr, sizeof(hdr)-sizeof(hdr.headCrc));

	// Scrive l'header e il contenuto dell'eeeprom nel file
	if ( fwrite(&hdr, sizeof(hdr), 1, fh)  &&
			fwrite(FileBuf::GetBufPtr(), hdr.e2pSize, 1, fh) )
	{
		rval = GetNoOfBlock();
	}
	else
	{
		rval = WRITEERROR;
	}

	fclose(fh);
	return rval;
}

