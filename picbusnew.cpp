//=========================================================================//
//-------------------------------------------------------------------------//
// picbusnew.cpp -- Source for Pic bus driver class                        //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
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

#include "types.h"
#include "picbusnew.h"
#include "errcode.h"

#include "e2app.h"

#ifdef	_LINUX_
#  include <asm/io.h>
#  include <unistd.h>
#else
#  ifdef	__BORLANDC__
#    define	__inline__
#  else // _MICROSOFT_ VC++
#    define	__inline__ __inline
#    define _export
#  endif
#endif

// Costruttore
PicBusNew::PicBusNew(BusInterface *ptr)
	: PicBus(ptr)
{
}

int PicBusNew::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(7);

	return OK;
}

int PicBusNew::Reset(void)
{
	return PicBus::Reset();
}

long PicBusNew::Write(int addr, UBYTE const *data, long length)
{
	long len;

	if (addr == 0)
		length >>= 1;	//contatore da byte a word

	for (len = 0; len < length; len++)
	{
		UWORD val;

		//Send command opcode
		if (addr)
		{
			//Write Data code
			val  = (UWORD)(*data++);
			if ( CompareSingleWord(val, 0xffff, DataMask) != 0 )
			{
				SendCmdCode(LoadDataCode);
				SendDataCode(val);
				SendCmdCode(BeginProgOnlyCode);

				if ( WaitReadyAfterWrite() )
					break;
			}
			//Verify while programming (10/11/99)
		//	SendCmdCode(ReadDataCode);
		//	if ( CompareSingleWord(val, RecvDataCode(), DataMask) )
		//		break;

			SendCmdCode(IncAddressCode);
		}
		else
		{
#ifdef	_BIG_ENDIAN_
			val  = (UWORD)(*data++) << 8;
			val |= (UWORD)(*data++);
#else
			val  = (UWORD)(*data++);
			val |= (UWORD)(*data++) << 8;
#endif
			if ( CompareSingleWord(val, 0xffff, ProgMask) != 0 )
			{
				//Write Program code
				SendCmdCode(LoadProgCode);
				SendProgCode(val);
				SendCmdCode(BeginProgOnlyCode);

				if ( WaitReadyAfterWrite() )
					break;
			}

			//Verify while programming (10/11/99)
		//	SendCmdCode(ReadProgCode);
		//	if ( CompareSingleWord(val, RecvProgCode(), ProgMask) )
		//		break;

			SendCmdCode(IncAddressCode);
		}

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	if (addr == 0)
		len <<= 1;	//contatore da word a byte

	return len;
}
