//=========================================================================//
//-------------------------------------------------------------------------//
// at250bus2.cpp -- AT25xxx (Big SPI eeproms) Bus Driver                   //
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
#include "at250bus2.h"
#include "errcode.h"

#include "e2app.h"

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	__inline__
#  else // _MICROSOFT_ VC++
#    define	__inline__ __inline
#    define _export
#  endif
#endif

// Costruttore
At250BigBus::At250BigBus(BusInterface *ptr)
	: At250Bus(ptr)
{
	UserDebug1(Constructor, "At250BigBus::At250BigBus(%xh)\n", (unsigned int)ptr);
}

long At250BigBus::Read(int addr, UBYTE *data, long length)
{
	UserDebug3(UserApp2, "At250BigBus::Read(%xh, %xh, %ld)\n", addr, (unsigned int)data, length);

	long len;

	SendDataByte( ReadData );
	SendDataByte( (addr >> 8) & 0xFF );	//MSB
	SendDataByte(  addr       & 0xFF );	//LSB

	WaitUsec(shot_delay);

	for (len = 0; len < length; len++)
	{
		*data++ = RecDataByte();

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	EndCycle();

	CheckAbort(100);

	UserDebug1(UserApp2, "At250BigBus::Read() = %ld\n", len);

	return len;
}


long At250BigBus::Write(int addr, UBYTE const *data, long length)
{
	long len;

	int writepage_size = THEAPP->GetSPIPageWrite();
	THEAPP->SetSPIPageWrite(writepage_size);

	WriteEEPStatus(0);

	// 07/08/99 *** bug fix suggested by Atmel Product engineer
	if (!WaitEndOfWrite())
		return 0;

	for (len = 0; len < length; len += writepage_size, addr += writepage_size)
	{
		SendDataByte(WriteEnable);
		EndCycle();

		SendDataByte( WriteData );
		SendDataByte( (addr >> 8) & 0xFF );	//MSB
		SendDataByte(  addr       & 0xFF );	//LSB

		int j;
		for (j = 0; j < writepage_size; j++)
			SendDataByte(*data++);

		EndCycle();

		if (!WaitEndOfWrite())
			return 0;		//Must return 0, because > 0 (and != length) means "Abort by user"

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	return len;
}
