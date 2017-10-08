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

#include "types.h"
#include "at250bus2.h"
#include "errcode.h"

#include <QDebug>

#include "e2cmdw.h"

#ifndef __linux__
#  ifdef        __BORLANDC__
#    define     __inline__
#  else // _MICROSOFT_ VC++
#    define     __inline__ __inline
#    define _export
#  endif
#endif

// Costruttore
At250BigBus::At250BigBus(BusInterface *ptr)
	: At250Bus(ptr)
{
	qDebug() << "At250BigBus::At250BigBus(" << (hex) << ptr << (dec) <<  ")";
}

long At250BigBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << "At250BigBus::Read(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
	ReadStart();

	long len;

	SendDataByte(ReadData);
	SendDataByte((addr >> 8) & 0xFF);       //MSB
	SendDataByte(addr       & 0xFF);        //LSB

	WaitUsec(shot_delay);

	for (len = 0; len < length; len++)
	{
		*data++ = RecDataByte();

		if ((len % 10) == 0)
		{
			if (ReadProgress(len * 100 / length))
			{
				break;
			}
		}
	}

	EndCycle();

	ReadEnd();
	qDebug() << "At250BigBus::Read() = " << len;

	return len;
}


long At250BigBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long len;

	WriteStart();

	int writepage_size = E2Profile::GetSPIPageWrite();
//	E2Profile::SetSPIPageWrite(writepage_size);

	WriteEEPStatus(0);

	// 07/08/99 *** bug fix suggested by Atmel Product engineer
	if (!WaitEndOfWrite())
	{
		return 0;
	}

	long count = 0;
	for (len = 0; len < length; len += writepage_size, addr += writepage_size)
	{
		SendDataByte(WriteEnable);
		EndCycle();

		SendDataByte(WriteData);
		SendDataByte((addr >> 8) & 0xFF);       //MSB
		SendDataByte(addr       & 0xFF);        //LSB

		int j;

		for (j = 0; j < writepage_size; j++)
		{
			SendDataByte(*data++);
		}

		EndCycle();

		if (!WaitEndOfWrite())
		{
			return 0;        //Must return 0, because > 0 (and != length) means "Abort by user"
		}

		if ((++count & 1))
		{
			if (WriteProgress(len * 100 / length))
			{
				break;
			}
		}
	}

	WriteEnd();

	return len;
}
