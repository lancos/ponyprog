//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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
	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << ptr << (dec) <<  ")";
}


int At250BigBus::ReadEEPByte(int addr)
{
	int rv;

	if (isUSBInstalled())
	{
		uint8_t buf[8] = {0};
		buf[0] = (ReadData | (((addr >> 8) & 1) << 3));
		buf[1] = ((addr >> 8) & 0xFF);
		buf[2] = (addr & 0xFF);

		int ret = StreamSPI(0, 4, buf, NULL);

		rv = buf[0];
	}
	else
	{
		SendDataByte(ReadData);
		SendDataByte((addr >> 8) & 0xFF);		//MSB
		SendDataByte(addr & 0xFF);				//LSB

		ShotDelay();

		rv = RecDataByte();

		EndCycle();
	}

	return rv;
}

void At250BigBus::WriteEEPByte(int addr, int data)
{
	if (isUSBInstalled())
	{
		uint8_t buf[8] = {0};
		buf[0] = WriteData;
		buf[1] = ((addr >> 8) & 0xFF);
		buf[2] = (addr & 0xFF);

		int ret = StreamSPI(0, 4, buf, NULL);
	}
	else
	{
		SendDataByte(WriteData);
		SendDataByte((addr >> 8) & 0xFF);		//MSB
		SendDataByte(addr & 0xFF);				//LSB

		EndCycle();
	}
}

long At250BigBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
	ReadStart();

	long len;

	if (page_size > 1)
	{
		uint8_t *buf = new uint8_t[page_size + 4];
		for (len = 0; len < length; len += page_size, addr += page_size)
		{
			int cnt = 0;
			buf[cnt++] = ReadData;
			buf[cnt++] = ((addr >> 8) & 0xFF);		//MSB
			buf[cnt++] = (addr & 0xFF);				//LSB

			int ret = StreamSPI(0, cnt, buf, NULL);

			// TODO memcpy
			for (len = 0; len < length; len++)
			{
				*data++ = buf[len];
			}
			ReadProgress(len * 100 / length);

			WaitMsec(1);		//Flush
		}

		EndCycle();

		ReadEnd();

		delete[] buf;
	}
	else
	{
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
	}
	qDebug() << __PRETTY_FUNCTION__ << "=" << len;

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

	SendDataByte(WriteEnable);

	if (page_size > 1)
	{
		uint8_t *buf = new uint8_t[page_size + 4];
		for (len = 0; len < length; len += writepage_size, addr += writepage_size)
		{
			int cnt = 0;
			buf[cnt++] = WriteData;
			buf[cnt++] = ((addr >> 8) & 0xFF);		//MSB
			buf[cnt++] = (addr & 0xFF);				//LSB

			for (int j = 0; j < writepage_size; j++)
			{
				buf[cnt++] = *data++;
			}

			int ret = StreamSPI(0, cnt, buf, NULL);

			WaitMsec(1);		//Flush

			WriteProgress(len * 100 / length);
		}

		delete[] buf;
	}
	else
	{
		int count = 0;
		for (len = 0; len < length; len += writepage_size, addr += writepage_size)
		{
			EndCycle();

			SendDataByte(WriteData);
			SendDataByte((addr >> 8) & 0xFF);		//MSB
			SendDataByte(addr & 0xFF);				//LSB

			for (int j = 0; j < writepage_size; j++)
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
	}

	SendDataByte(WriteDisable);

	WaitMsec(1);			//Flush

	WriteEnd();

	return len;
}
