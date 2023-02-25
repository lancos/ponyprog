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
#include "at93cbus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

#include "e2cmdw.h"

//Siamo sicuri BIGENDIAN?? Il formato HexIntel e` little-endian
//  e quindi anche le AT90S1200
#define _BIG_ENDIAN_

// Costruttore
At93cBus::At93cBus(BusInterface *ptr)
	: MicroWireBus(ptr),
	  ReadCode(06),
	  WriteCode(05),
	  WriteEnableCode(04),
	  EraseAllCode(04),
	  PrClearCode(07),
	  loop_timeout(8000),
	  address_len(6),			//9346
	  organization(ORG16)
{
	qDebug() << Q_FUNC_INFO;
}

int At93cBus::Erase(int type)
{
	(void)type;

	clearCS();
	setCS();

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0xFFFF, address_len);

	clearCS();
	setCS();

	SendCmdOpcode(EraseAllCode);
	SendDataWord((2 << (address_len - 2)), address_len);

	clearCS();
	setCS();

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0, address_len);

	return 1;
}


/**** Protect disable sequence:
        clearCS();
        PRE = 0;
        setCS();

        SendCmdOpcode(WriteEnableCode);         //WEN
        SendDataWord(0xFFFF, address_len);

        clearCS();
        PRE = 1;
        setCS();

        SendCmdOpcode(WriteEnableCode);         //PREN
        SendDataWord(0xFFFF, address_len);

        clearCS();
        PRE = 1;
        setCS();

        SendCmdOpcode(PrClearCode);                     //PRCLEAR
        SendDataWord(0xFFFF, address_len);

        clearCS();
        PRE = 0;
        setCS();

        SendCmdOpcode(WriteEnableCode);         //WDS
        SendDataWord(0, address_len);
****/


//ATTENTION!!! 93CXX are read and written a WORD at a time (not BYTE)
long At93cBus::Read(int addr, quint8 *data, long length, int page_size)
{
	(void)page_size;

	qDebug() << "At93cBus::Read(" << (Qt::hex) << addr << ", " << data << ", " << (Qt::dec) << length;

	ReadStart();

	long len;

	if (addr > 0)
	{
		address_len = addr;
	}

	addr = 0;

	int inc;

	if (organization == ORG16)
	{
		inc = 2;
	}
	else
	{
		inc = 1;
	}

	//Dal piu` significativo al meno significativo
	for (len = 0; len < length; len += inc)
	{
		//17/08/98 -- now repeat the command every word
		clearCS();
		ShotDelay();
		setCS();
		ShotDelay();

		//Send command opcode
		SendCmdOpcode(ReadCode);
		SendAddress(addr++, address_len);

		quint16 val = RecDataWord(organization);

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

		if ((len % 4) == 0)
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

long At93cBus::Write(int addr, quint8 const *data, long length, int page_size)
{
	long curaddr;

	WriteStart();

	if (addr > 0)
	{
		address_len = addr;
	}

	clearCS();                      //17/08/98 -- may be it's not needed
	setCS();

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0xFFFF, address_len);

	clearCS();
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
		SendCmdOpcode(WriteCode);
		SendAddress(curaddr, address_len);
		SendDataWord(val, organization);

#if 1

		if (WaitReadyAfterWrite(loop_timeout))
		{
			return 0;        //- 07/08/99 a number >0 but != length mean "User abort"
		}

#else
		WaitMsec(10);
#endif
		clearCS();
		setCS();

		if ((curaddr & 1))
			if (WriteProgress(curaddr * 100 / length))
			{
				break;
			}
	}

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0, address_len);
	WaitMsec(1);		//Flush

	WriteEnd();

	if (organization == ORG16)
	{
		curaddr <<= 1;        //word to byte counter
	}

	return curaddr;
}
