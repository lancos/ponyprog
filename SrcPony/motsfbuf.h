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

//Read/Write Motorola Srecord file format

#ifndef _MOTOSFBUF_H
#define _MOTOSFBUF_H

#include <QTextStream>

#include "types.h"
#include "filebuf.h"


class MotorolaSFileBuf : public FileBuf
{
  public:               //---------------------------------------- public

	MotorolaSFileBuf(e2AppWinInfo *wininfo = 0);
	virtual ~MotorolaSFileBuf();

	virtual int Load(int loadtype = ALL_TYPE, long relocation_offset = 0);
	virtual int Save(int savetype = ALL_TYPE, long relocation_offset = 0);

  protected:    //--------------------------------------- protected

  private:              //--------------------------------------- private

	int WriteRecord(QTextStream &outs, uint8_t *bptr, long curaddr, long recsize, int fmt);
	int ParseRecord(char *lbufPC, uint8_t *buf_startP, uint8_t *buf_endP, long offset, int nocopy);

	uint8_t *highestPC;
	long highestAddr;
	long lowestAddr;

	int valid_record_count;         //incremented by ParseRecord for each valid record found
};
#endif
