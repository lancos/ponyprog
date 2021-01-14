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
#include "at250bus2.h"
#include "errcode.h"

#include <QtCore>
#include <QDebug>

#include "e2cmdw.h"

At250BigBus::At250BigBus(BusInterface *ptr)
	: At250Bus(ptr)
{
	qDebug() << Q_FUNC_INFO << "(" << (hex) << ptr << (dec) <<  ")";
}

long At250BigBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << Q_FUNC_INFO << "(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
	ReadStart();

	long len;

	SendDataByte(ReadData);
	SendDataByte((addr >> 8) & 0xFF);		//MSB
	SendDataByte(addr & 0xFF);				//LSB

	ShotDelay();

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
	WaitMsec(1);		//Flush

	EndCycle();

	ReadEnd();
	qDebug() << Q_FUNC_INFO << "=" << len;

	return len;
}

long At250BigBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long len;

	WriteStart();

	int writepage_size = E2Profile::GetSPIPageWrite();

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
		SendDataByte((addr >> 8) & 0xFF);		//MSB
		SendDataByte(addr & 0xFF);				//LSB

		int j;

		for (j = 0; j < writepage_size; j++)
		{
			SendDataByte(*data++);
		}

		EndCycle();

		if (!WaitEndOfWrite())
		{
			return 0;			//Must return 0, because > 0 (and != length) means "Abort by user"
		}

		if ((++count & 1))
		{
			if (WriteProgress(len * 100 / length))
			{
				break;
			}
		}
	}
	WaitMsec(1);			//Flush

	WriteEnd();

	return len;
}
