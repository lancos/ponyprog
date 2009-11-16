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

#ifndef _FILEBUF_H
#define _FILEBUF_H

// Include standard V files as needed

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "globals.h"

class e2AppWinInfo;

class FileBuf
{
  public:		//---------------------------------------- public

	FileBuf(e2AppWinInfo *wininfo = 0);
	virtual ~FileBuf();

	void SetAWInfo(e2AppWinInfo *wininfo);

	void SetFileName(char const *name);	//ok
	char const *GetFileName() const;	//ok

	virtual int Load(int loadtype = ALL_TYPE, long relocation_offset = 0) = 0;
	virtual int Save(int savetype = ALL_TYPE, long relocation_offset = 0) = 0;
	FileType GetFileType() const
		{ return file_type; }
//	void SetBlockSize(int blksize)
//		{ awi->SetBlockSize(blksize); }
	int GetBlockSize() const;
	int GetNoOfBlock() const;
	void SetNoOfBlock(int no_blk);
	long GetEEpromType() const;
	int GetEEpromPriType() const;
	int GetEEpromSubType() const;
	void SetEEpromType(int pritype, int subtype = 0);
	
	char *GetStringID(char *s = 0);
	void SetStringID(char const *s);
	char *GetComment(char *s = 0);
	void SetComment(char const *s);

	int GetRollOver() const;
	void SetRollOver(int rlv);

	long GetSplitted() const;
	void SetSplitted(long spl);

	DWORD GetLockBits() const;
	void SetLockBits(DWORD bits);
	DWORD GetFuseBits() const;
	void SetFuseBits(DWORD bits);

//	UWORD GetCRC() const
//		{ return awi->GetCRC(); }
	void SetCRC(uint16_t c);

  protected:	//--------------------------------------- protected
	
	uint8_t *GetBufPtr() const;
	long GetBufSize() const;
	e2AppWinInfo *GetAWInfo();
	long GetFileSize(FILE *fh);

	FileType file_type;		//Identificativo del tipo di file (E2P, INTEL, ...)

	// Informazioni addizionali sull'eeprom che vengono salvate nel file

  private:		//--------------------------------------- private

	e2AppWinInfo *awi;	//puntatore alla AppWinInfo che contiene questo FileBuffer
};

#endif
