//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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
#include <QFile>
#include <QFileInfo>

#include "types.h"
#include "filebuf.h"            // Header file
#include "e2awinfo.h"
#include "errcode.h"

FileBuf::FileBuf(e2AppWinInfo *wininfo)
{
	SetAWInfo(wininfo);
}

FileBuf::~FileBuf()
{
}

void FileBuf::SetAWInfo(e2AppWinInfo *wininfo)
{
	if (wininfo)
	{
		awi = wininfo;
	}
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

void FileBuf::SetFileName(const QString &name)
{
	awi->SetFileName(name);
}

QString FileBuf::GetFileName()
{
	return awi->GetFileName();
}

QString FileBuf::GetStringID()
{
	return awi->GetStringID();
}

void FileBuf::SetStringID(const QString &s)
{
	awi->SetStringID(s);
}

QString FileBuf::GetComment()
{
	return awi->GetComment();
}

void FileBuf::SetComment(const QString &s)
{
	awi->SetComment(s);
}

int FileBuf::GetRollOver() const
{
	return awi->GetBankRollOver();
}

quint32 FileBuf::GetLockBits() const
{
	return awi->GetLockBits();
}

quint32 FileBuf::GetFuseBits() const
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

void FileBuf::SetLockBits(quint32 bits)
{
	awi->SetLockBits(bits);
}

void FileBuf::SetFuseBits(quint32 bits)
{
	awi->SetFuseBits(bits);
}

void FileBuf::SetCRC(uint16_t c)
{
	awi->SetCRC(c);
}

quint32 FileBuf::GetEEpromType() const
{
	return awi->GetEEPId();
}

quint32 FileBuf::GetEEpromPriType() const
{
	return awi->GetE2PPriType(awi->GetEEPId());
}

quint32 FileBuf::GetEEpromSubType() const
{
	return awi->GetE2PSubType(awi->GetEEPId());
}

void FileBuf::SetEEpromType(quint32 pritype, quint32 subtype)
{
// 	extern long BuildE2PType(int pritype, int subtype);

	awi->SetEEProm(awi->BuildE2PType(pritype, subtype));
}

void FileBuf::SetEEpromType(quint32 id)
{
	awi->SetEEProm(id);
}


long FileBuf::GetFileSize(QFile &fh)
{
	if (fh.exists())
	{
		return fh.size();
	}
	else
	{
		return -1;
	}
}

