//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id: binfbuf.cpp,v 1.3 2009/11/16 22:29:18 lancos Exp $
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
#include "binfbuf.h"            // Header file
#include "errcode.h"


//======================>>> binFileBuf::binFileBuf <<<=======================
binFileBuf::binFileBuf(e2AppWinInfo *wininfo)
	: FileBuf(wininfo)
{
	file_type = BIN;
}

//======================>>> binFileBuf::~binFileBuf <<<=======================
binFileBuf::~binFileBuf()
{
}

#include "crc.h"

//======================>>> binFileBuf::Load <<<=======================
int binFileBuf::Load(int loadtype, long relocation_offset)
{
	FILE *fh;
	int rval = OK;

	if ( (fh = fopen(GetFileName().toLatin1(), "rb")) == NULL )
	{
		return FILENOTFOUND;
	}

	long fsize = GetFileSize(fh);
	rewind(fh);

	//In questi formati di file "stupidi" la dimensione
	//deve rimanere quella della eeprom attualmente selezionata
	long buf_size = GetBufSize();
	uint8_t *ptr = GetBufPtr();

	if (loadtype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < buf_size)
		{
			ptr += GetSplitted();
			buf_size -= GetSplitted();
		}
		else
		{
			fclose(fh);
			return 0;
		}
	}
	else if (loadtype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= buf_size)
		{
			buf_size = GetSplitted();
		}
		else
		{
			fclose(fh);
			return 0;
		}
	}

	if (relocation_offset >= buf_size)
	{
		return BADPARAM;
	}
	else
	{
		ptr += relocation_offset;
		buf_size -= relocation_offset;
	}

	if (fsize > buf_size)
	{
		fsize = buf_size;
	}

	rval = fread(ptr, fsize, 1, fh);
	fclose(fh);

	//      SetStringID("");        //????
	SetComment("");
	SetRollOver(0);         //2 (che significa NO) ??
	SetCRC( mcalc_crc(GetBufPtr(), GetBlockSize() * GetNoOfBlock()) );

	//      rval = GetNoOfBlock();

	return rval;
}

//======================>>> binFileBuf::Save <<<=======================
int binFileBuf::Save(int savetype, long relocation_offset)
{
	FILE *fh;
	int rval;

	if (FileBuf::GetNoOfBlock() <= 0)
	{
		return NOTHINGTOSAVE;
	}

	if ( (fh = fopen(FileBuf::GetFileName().toLatin1(), "wb")) == NULL )
	{
		return CREATEERROR;
	}

	long buf_size = FileBuf::GetBlockSize() * FileBuf::GetNoOfBlock();
	uint8_t *ptr = FileBuf::GetBufPtr();

	if (savetype == PROG_TYPE)
	{
		if (GetSplitted() > 0 && GetSplitted() <= buf_size)
		{
			buf_size = GetSplitted();
		}
		else
		{
			fclose(fh);
			return 0;
		}
	}
	else if (savetype == DATA_TYPE)
	{
		if (GetSplitted() >= 0 && GetSplitted() < buf_size)
		{
			ptr += GetSplitted();
			buf_size -= GetSplitted();
		}
		else
		{
			fclose(fh);
			return 0;
		}
	}

	rval = fwrite(ptr, buf_size, 1, fh);

	if (rval == 0)
	{
		rval = WRITEERROR;
	}

	fclose(fh);
	return rval;
}
