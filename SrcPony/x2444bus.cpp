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

// Costruttore
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
	qDebug() << "X2444Bus::X2444Bus()";
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

long X2444Bus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << "X2444Bus::Read(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
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
	WaitUsec(shot_delay);
	setCS();

	SendCmdAddr(RecallCode, 0xff);

	//Dal piu` significativo al meno significativo
	for (len = 0; len < length; len += inc)
	{
		//17/08/98 -- now repeat the command every word
		clearCS();
		WaitUsec(shot_delay);
		setCS();

		//Send command opcode and address
		SendCmdAddr(ReadCode, addr++);

		//Il primo bit dopo il comando di lettura e` piu` "corto"
		// cosi` non possiamo utilizzare la normale routine di lettura word
		uint16_t val = RecDataWordShort(organization, 1);

		if (organization == ORG16)
		{
#ifdef  _BIG_ENDIAN_
			*data++ = (uint8_t)(val >> 8);
			*data++ = (uint8_t)(val & 0xFF);
#else
			*data++ = (uint8_t)(val & 0xFF);
			*data++ = (uint8_t)(val >> 8);
#endif
		}
		else
		{
			*data++ = (uint8_t)(val & 0xFF);
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

	ReadEnd();
	qDebug() << "X2444Bus::Read() = " << len;

	return len;
}

long X2444Bus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long curaddr;

	WriteStart();

	clearCS();                      //17/08/98 -- may be it's not needed
	WaitUsec(shot_delay);
	setCS();

	//Recall eeprom from ram
	SendCmdAddr(RecallCode, 0xff);

	clearCS();
	WaitUsec(shot_delay);
	setCS();

	//Write enable
	SendCmdAddr(WriteEnableCode, 0xff);

	clearCS();
	WaitUsec(shot_delay);
	setCS();

	if (organization == ORG16)
	{
		length >>= 1;        //byte to word  counter
	}

	for (curaddr = 0; curaddr < length; curaddr++)
	{
		uint16_t val;

		if (organization == ORG16)
		{
#ifdef  _BIG_ENDIAN_
			val  = (uint16_t)(*data++) << 8;
			val |= (uint16_t)(*data++);
#else
			val  = (uint16_t)(*data++);
			val |= (uint16_t)(*data++) << 8;
#endif
		}
		else
		{
			val = *data++;
		}

		//Send command opcode
		SendCmdAddr(WriteCode, curaddr);
		SendDataWord(val, organization, 1);

		WaitUsec(shot_delay);

		clearCS();
		WaitUsec(shot_delay);
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
	WaitMsec(10);

	WriteEnd();

	if (organization == ORG16)
	{
		curaddr <<= 1;        //word to byte counter
	}

	return curaddr;
}
