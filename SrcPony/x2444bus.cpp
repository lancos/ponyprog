//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
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
#include "x2444bus.h"
#include "errcode.h"

#include <QDebug>

#include "e2cmdw.h"

#define _BIG_ENDIAN_

X2444Bus::X2444Bus(BusInterface *ptr)
	: MicroWireBus(ptr),
	  ReadCode(06),
	  WriteCode(03),
	  WriteEnableCode(04),
	  WriteDisableCode(00),
	  RecallCode(05),
	  StoreCode(01),
	  loop_timeout(8000),
	  organization(ORG16)
{
	qDebug() << Q_FUNC_INFO;
}

void X2444Bus::SendCmdAddr(int cmd, int addr)
{
	if (organization == ORG16)
	{
		SendDataWord(0x80 | ((addr & 0x0f) << 3) | (cmd & 7), 8);
	}
	else
	{
		SendDataWord(0x80 | ((addr & 0x07) << 4) | (cmd & 7), 8);
	}
}

long X2444Bus::Read(int addr, quint8 *data, long length, int page_size)
{
	qDebug() << Q_FUNC_INFO << "(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
	ReadStart();

	long len;
	int inc;
	addr = 0;

	if (organization == ORG16)
	{
		inc = 2;
	}
	else
	{
		inc = 1;
	}

	//Prima assicuriamoci una recall
	clearCS();
	ShotDelay();
	setCS();

	SendCmdAddr(RecallCode, 0xff);

	//Dal piu` significativo al meno significativo
	for (len = 0; len < length; len += inc)
	{
		//17/08/98 -- now repeat the command every word
		clearCS();
		ShotDelay();
		setCS();

		//Send command opcode and address
		SendCmdAddr(ReadCode, addr++);

		//The first bit after read command is "shorter"
		// so we have to use a special function
		quint16 val = RecDataWordShort(organization, true);

		if (organization == ORG16)
		{
#ifdef  _BIG_ENDIAN_
			*data++ = (quint8)(val >> 8);
			*data++ = (quint8)(val & 0xFF);
#else
			*data++ = (quint8)(val & 0xFF);
			*data++ = (quint8)(val >> 8);
#endif
		}
		else
		{
			*data++ = (quint8)(val & 0xFF);
		}

		WaitUsec(1);

		if ((len & 1))
		{
			if (ReadProgress(len * 100 / length))
			{
				break;
			}
		}
	}

	clearCS();
	WaitMsec(1);		//Flush

	ReadEnd();
	qDebug() << Q_FUNC_INFO << "=" << len;

	return len;
}

long X2444Bus::Write(int addr, quint8 const *data, long length, int page_size)
{
	long curaddr;

	WriteStart();

	clearCS();                      //17/08/98 -- may be it's not needed
	ShotDelay();
	setCS();

	//Recall eeprom from ram
	SendCmdAddr(RecallCode, 0xff);

	clearCS();
	ShotDelay();
	setCS();

	//Write enable
	SendCmdAddr(WriteEnableCode, 0xff);

	clearCS();
	ShotDelay();
	setCS();

	if (organization == ORG16)
	{
		length >>= 1;        //byte to word  counter
	}

	for (curaddr = 0; curaddr < length; curaddr++)
	{
		quint16 val;

		if (organization == ORG16)
		{
#ifdef  _BIG_ENDIAN_
			val  = (quint16)(*data++) << 8;
			val |= (quint16)(*data++);
#else
			val  = (quint16)(*data++);
			val |= (quint16)(*data++) << 8;
#endif
		}
		else
		{
			val = *data++;
		}

		//Send command opcode
		SendCmdAddr(WriteCode, curaddr);
		SendDataWord(val, organization, true);

		ShotDelay();

		clearCS();
		ShotDelay();
		setCS();

		if ((curaddr & 1))
		{
			if (WriteProgress(curaddr * 100 / length))
			{
				break;
			}
		}
	}

	SendCmdAddr(StoreCode, 0xff);
	clearCS();
	WaitMsec(10);		//Flush

	WriteEnd();

	if (organization == ORG16)
	{
		curaddr <<= 1;        //word to byte counter
	}

	return curaddr;
}
