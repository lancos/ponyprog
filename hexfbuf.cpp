//=========================================================================//
//-------------------------------------------------------------------------//
// hexfbuf.cpp -- Source for hexFileBuf class (raw binary file buffer)     //
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

#include "types.h"
#include "hexfbuf.h"		// Header file
#include "errcode.h"

//======================>>> hexFileBuf::hexFileBuf <<<=======================
hexFileBuf::hexFileBuf(e2AppWinInfo *wininfo)
		: FileBuf(wininfo)
{
	file_type = BIN;
}

//======================>>> hexFileBuf::~hexFileBuf <<<=======================
hexFileBuf::~hexFileBuf()
{
}

#include "crc.h"

//======================>>> hexFileBuf::Load <<<=======================
int hexFileBuf::Load(int bank)
{
	FILE *fh;
	int rval = OK;

	if ( (fh = fopen(GetFileName(), "rb")) == NULL )
		return FILENOTFOUND;
	
	long fsize = GetFileSize(fh);
	rewind(fh);

//In questi formati di file "stupidi" la dimensione
//deve rimanere quella della eeprom attualmente selezionata
	long buf_size = GetBufSize();
	BYTE *ptr = GetBufPtr();
	if (bank)
	{	//caso in cui sia il secondo banco di una splitted
		ptr += GetSplitted();
		buf_size -= GetSplitted();
		if (buf_size < 0)
		{
			fclose(fh);
			return 0;	//Errore, sigh!!
		}
	}
	if (fsize > buf_size)
		fsize = buf_size;
	rval = fread(ptr, fsize, 1, fh);
	fclose(fh);

//	SetStringID("");	//????
	SetComment("");
	SetRollOver(0);		//2 (che significa NO) ??
	SetCRC( mcalc_crc(GetBufPtr(), GetBlockSize() * GetNoOfBlock()) );

//	rval = GetNoOfBlock();

	return rval;
}

//======================>>> hexFileBuf::Save <<<=======================
int hexFileBuf::Save()
{
	FILE *fh;
	int rval;

	if (FileBuf::GetNoOfBlock() <= 0)
		return NOTHINGTOSAVE;

	if ( (fh = fopen(FileBuf::GetFileName(), "wb")) == NULL )
		return CREATEERROR;

	rval = fwrite(FileBuf::GetBufPtr(), FileBuf::GetBlockSize(), FileBuf::GetNoOfBlock(), fh);
	if (rval != FileBuf::GetNoOfBlock())
		rval = WRITEERROR;

	fclose(fh);
	return rval;
}

