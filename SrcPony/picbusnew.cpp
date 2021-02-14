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

#include "types.h"
#include "picbusnew.h"
#include "errcode.h"

#include "globals.h"

#include <QtCore>

PicBusNew::PicBusNew(BusInterface *ptr)
	: PicBus(ptr)
{
}

int PicBusNew::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(7);

	return OK;
}

long PicBusNew::Write(int addr, quint8 const *data, long length, int page_size)
{
	long len;

	WriteStart();

	if (addr == 0)
	{
		length >>= 1;			//convert counter from byte to word
	}

	for (len = 0; len < length; len++)
	{
		quint16 val;

		//Send command opcode
		if (addr)
		{
			//Write Data code
			val  = (quint16)(*data++);

			if (CompareSingleWord(val, 0xffff, DataMask) != 0)
			{
				SendCmdCode(LoadDataCode);
				SendDataCode(val);
				SendCmdCode(BeginProgOnlyCode);

				if (WaitReadyAfterWrite())
				{
					break;
				}
			}

			//Verify while programming (10/11/99)
			//      SendCmdCode(ReadDataCode);
			//      if ( CompareSingleWord(val, RecvDataCode(), DataMask) )
			//              break;

			SendCmdCode(IncAddressCode);
		}
		else
		{
#ifdef  _BIG_ENDIAN_
			val  = (quint16)(*data++) << 8;
			val |= (quint16)(*data++);
#else
			val  = (quint16)(*data++);
			val |= (quint16)(*data++) << 8;
#endif

			if (CompareSingleWord(val, 0xffff, ProgMask) != 0)
			{
				//Write Program code
				SendCmdCode(LoadProgCode);
				SendProgCode(val);
				SendCmdCode(BeginProgOnlyCode);

				if (WaitReadyAfterWrite())
				{
					break;
				}
			}

			//Verify while programming (10/11/99)
			//      SendCmdCode(ReadProgCode);
			//      if ( CompareSingleWord(val, RecvProgCode(), ProgMask) )
			//              break;

			SendCmdCode(IncAddressCode);
		}

		if (WriteProgress(len * 100 / length))
		{
			break;
		}
	}
	WaitMsec(1);		//Flush

	WriteEnd();

	if (addr == 0)
	{
		len <<= 1;		//convert counter from word to byte
	}

	return len;
}
