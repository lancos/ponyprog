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
#include <string.h>

#include "types.h"
#include "filebuf.h"		// Header file
#include "e2awinfo.h"
#include "errcode.h"

//======================>>> FileBuf::FileBuf <<<=======================
FileBuf::FileBuf(e2AppWinInfo *wininfo)
{
	SetAWInfo(wininfo);
}

//======================>>> FileBuf::~FileBuf <<<=======================
FileBuf::~FileBuf()
{
}

void FileBuf::SetAWInfo(e2AppWinInfo *wininfo)
{
	if (wininfo)
		awi = wininfo;
}

e2AppWinInfo *FileBuf::GetAWInfo()
{
	return awi;
}

uint8_t *FileBuf::GetBufPtr() const
{
	return (uint8_t *)awi->GetBufPtr();
}

long FileBuf::GetBufSize() const
{
	return awi->GetBufSize();
}

int FileBuf::GetNoOfBlock() const
{
	return awi->GetNoOfBlock();
}

void FileBuf::SetNoOfBlock(int no_blk)
{
	awi->SetNoOfBlock(no_blk);
}

int FileBuf::GetBlockSize() const
{
	return awi->GetBlockSize();
}

//======================>>> FileBuf::SetNewFile <<<=======================
void FileBuf::SetFileName(char const *name)
{
	awi->SetFileName(name);
}

//======================>>> FileBuf::GetFileName <<<=======================
char const *FileBuf::GetFileName() const
{
	return awi->GetFileName();
}

char *FileBuf::GetStringID(char *s)
{
	return awi->GetStringID(s);
}

void FileBuf::SetStringID(char const *s)
{
	awi->SetStringID(s);
}

char *FileBuf::GetComment(char *s)
{
	return awi->GetComment(s);
}

void FileBuf::SetComment(char const *s)
{
	awi->SetComment(s);
}

int FileBuf::GetRollOver() const
{
	return awi->GetBankRollOver();
}

DWORD FileBuf::GetLockBits() const
{
	return awi->GetLockBits();
}

DWORD FileBuf::GetFuseBits() const
{
	return awi->GetFuseBits();
}

void FileBuf::SetRollOver(int rlv)
{
	awi->SetBankRollOver(rlv);
}

long FileBuf::GetSplitted() const
{
	return awi->GetSplittedInfo();
}

void FileBuf::SetSplitted(long spl)
{
	awi->SetSplittedInfo(spl);
}

void FileBuf::SetLockBits(DWORD bits)
{
	awi->SetLockBits(bits);
}

void FileBuf::SetFuseBits(DWORD bits)
{
	awi->SetFuseBits(bits);
}

void FileBuf::SetCRC(UWORD c)
{
	awi->SetCRC(c);
}

//======================>>> FileBuf::GetEEpromType <<<=======================
long FileBuf::GetEEpromType() const
{
	return ((long)awi->GetEEPPriType() << 16) | (awi->GetEEPSubType() & 0x7FFF);
}

//======================>>> FileBuf::GetEEpromPriType <<<=======================
int FileBuf::GetEEpromPriType() const
{
	return awi->GetEEPPriType();
}

//======================>>> FileBuf::GetEEpromSubType <<<=======================
int FileBuf::GetEEpromSubType() const
{
	return awi->GetEEPSubType();
}

//======================>>> FileBuf::SetEEpromType <<<=======================
void FileBuf::SetEEpromType(int pritype, int subtype)
{
	awi->SetEEProm(pritype, subtype);
}

//======================>>> FileBuf::GetFileSize <<<=======================
long FileBuf::GetFileSize(FILE *fh)
{
    long fsize = 0, old_pos;

    if (fh)
    {
        old_pos = ftell(fh);            // salva la posizione attuale
        fseek(fh, 0, SEEK_END);         // si posizione in fondo al file
        fsize = ftell(fh);              // ne ricava la lunghezza
        fseek(fh, old_pos, SEEK_SET);   // si riposizione dove era prima
    }

    return fsize;
}

