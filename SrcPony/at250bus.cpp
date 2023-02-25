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
#include "at250bus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

At250Bus::At250Bus(BusInterface *ptr)
	: SPIBus(ptr),
	  WriteEnable(0x06),
	  WriteDisable(0x04),
	  ReadStatus(0x05),
	  WriteStatus(0x01),
	  ReadData(0x03),
	  WriteData(0x02),
	  NotReadyFlag(1),
	  WenFlag(2),
	  BPFlags(8 + 4),
	  loop_timeout(5000)
{
}

void At250Bus::EndCycle()
{
	//WaitUsec(1);
	setNCS();
	ShotDelay(2);
	clearNCS();
	ShotDelay();
}

int At250Bus::ReadEEPByte(int addr)
{
	int rv;

	SendDataByte(ReadData | (((addr >> 8) & 1) << 3));
	SendDataByte(addr);

	ShotDelay();

	rv = RecDataByte();

	EndCycle();

	return rv;
}

void At250Bus::WriteEEPByte(int addr, int data)
{
	SendDataByte(WriteData | (((addr >> 8) & 1) << 3));
	SendDataByte(addr);
	SendDataByte(data);

	EndCycle();
}

int At250Bus::ReadEEPStatus()
{
	int rv;

	SendDataByte(ReadStatus);
	rv = RecDataByte();

	EndCycle();

	return rv;
}

int At250Bus::WriteEEPStatus(int data)
{
	SendDataByte(WriteEnable);
	EndCycle();

	SendDataByte(WriteStatus);
	SendDataByte(data);

	EndCycle();

	return 0;
}

int At250Bus::WaitEndOfWrite(int timeout)               // 07/08/99
{
	if (timeout <= 0)
	{
		timeout = loop_timeout;
	}

	int k;

	for (k = timeout; k > 0 && (ReadEEPStatus() & NotReadyFlag); k--)
	{
		WaitUsec(50);
	}

	return (k != 0);
}

int At250Bus::Reset(void)
{
	qDebug() << Q_FUNC_INFO;

	SPIBus::Reset();
	ShotDelay();

	//SendDataByte(WriteEnable);
	//EndCycle();

	return OK;
}

long At250Bus::Read(int addr, quint8 *data, long length, int page_size)
{
	qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << addr << ", " << data << ", " << (Qt::dec) << length << ")";

	long len;

	ReadStart();

	for (len = 0; len < length; len++)
	{
		*data++ = (quint8)ReadEEPByte(addr++);

		if ((len % 10) == 0)
		{
			if (ReadProgress(len * 100 / length))
			{
				break;
			}
		}
	}
	WaitMsec(1);		//Flush

	ReadEnd();

	qDebug() << Q_FUNC_INFO << "=" << len;

	return len;
}

long At250Bus::Write(int addr, quint8 const *data, long length, int page_size)
{
	long len;

	WriteStart();
	WriteEEPStatus(0);

	// 07/08/99 *** bug fix suggested by Atmel Product engineer
	if (!WaitEndOfWrite())
	{
		return 0;
	}

	for (len = 0; len < length; len++)
	{
		SendDataByte(WriteEnable);
		EndCycle();

		WriteEEPByte(addr++, *data++);

		if (!WaitEndOfWrite())
		{
			return 0;		//Must return 0, because > 0 (and != length) means "Abort by user"
		}

		if ((len & 1))
		{
			if (WriteProgress(len * 100 / length))
			{
				break;
			}
		}
	}
	WaitMsec(1);		//Flush

	WriteEnd();

	return len;
}
