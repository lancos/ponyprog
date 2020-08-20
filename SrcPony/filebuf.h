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

#ifndef _FILEBUF_H
#define _FILEBUF_H

#include <QString>
#include <QFile>

// #include <stdio.h>
#include "types.h"
#include "globals.h"


class e2AppWinInfo;

class FileBuf
{
  public:

	FileBuf(e2AppWinInfo *wininfo = 0);
	virtual ~FileBuf();

	void SetAWInfo(e2AppWinInfo *wininfo);

	void SetFileName(const QString &name);     //ok
	QString GetFileName();        //ok

	virtual int Load(int loadtype = ALL_TYPE, long relocation_offset = 0) = 0;
	virtual int Save(int savetype = ALL_TYPE, long relocation_offset = 0) = 0;
	FileType GetFileType() const
	{
		return file_type;
	}
	//      void SetBlockSize(int blksize)
	//              { awi->SetBlockSize(blksize); }
	int GetBlockSize() const;
	int GetNoOfBlock() const;
	void SetNoOfBlock(int no_blk);
	quint32 GetEEpromType() const;
	quint32 GetEEpromPriType() const;
	quint32 GetEEpromSubType() const;
	void SetEEpromType(quint32 pritype, quint32 subtype);
	void SetEEpromType(quint32 id);

	QString GetStringID();
	void SetStringID(const QString &s);
	QString GetComment();
	void SetComment(const QString &s);

	int GetRollOver() const;
	void SetRollOver(int rlv);

	long GetSplitted() const;
	void SetSplitted(long spl);

	quint32 GetLockBits() const;
	void SetLockBits(quint32 bits);
	quint32 GetFuseBits() const;
	void SetFuseBits(quint32 bits);

	//      uint16_t GetCRC() const
	//              { return awi->GetCRC(); }
	void SetCRC(uint16_t c);

  protected:

	uint8_t *GetBufPtr() const;
	long GetBufSize() const;
	e2AppWinInfo *GetAWInfo();
	long GetFileSize(QFile &fh);

	FileType file_type;             //Identificativo del tipo di file (E2P, INTEL, ...)

	// Informazioni addizionali sull'eeprom che vengono salvate nel file

  private:

	e2AppWinInfo *awi;      //puntatore alla AppWinInfo che contiene questo FileBuffer
};

#endif
