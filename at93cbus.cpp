//=========================================================================//
//-------------------------------------------------------------------------//
// at93cbus.cpp -- AT93Cx6 (Microwire eeproms) Bus driver                  //
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
#include "at93cbus.h"
#include "errcode.h"

//Siamo sicuri BIGENDIAN?? Il formato HexIntel e` little-endian
//  e quindi anche le AT90S1200
#define	_BIG_ENDIAN_

#ifdef	_LINUX_
//#  include <asm/io.h>
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
At93cBus::At93cBus(BusInterface *ptr)
	: MicroWireBus(ptr),
		ReadCode(06),
		WriteCode(05),
		WriteEnableCode(04),
		loop_timeout(8000),
		address_len(6)		//9346
{
	UserDebug(Constructor, "At93cBus::At93cBus()\n");
}

//ATTENZIONE!!! Le 93CXX vengono lette e scritte una WORD per volta,
// non a BYTE
long At93cBus::Read(int addr, UBYTE *data, long length)
{
	UserDebug3(UserApp1, "At93cBus::Read(%xh, %xh, %ld)\n", addr, (unsigned int)data, length);

	long len;

	if (addr > 0)
		address_len = addr;

	addr = 0;

	//Dal piu` significativo al meno significativo
	for (len = 0; len < length; len += 2)
	{	//17/08/98 -- now repeat the command every word
		clearCS();
		setCS();

		//Send command opcode
		SendCmdOpcode(ReadCode);
		SendAddress(addr++, address_len);

		UWORD val = RecDataWord();

#ifdef	_BIG_ENDIAN_
		*data++ = (UBYTE)(val >> 8);
		*data++ = (UBYTE)(val & 0xFF);
#else
		*data++ = (UBYTE)(val & 0xFF);
		*data++ = (UBYTE)(val >> 8);
#endif

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	UserDebug1(UserApp1, "At93cBus::Read() = %ld\n", len);

	return len;
}

long At93cBus::Write(int addr, UBYTE const *data, long length)
{
	long curaddr;

	if (addr > 0)
		address_len = addr;

	clearCS();			//17/08/98 -- forse non necessario
	setCS();

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0xFFFF, address_len);

	clearCS();
	setCS();

	length >>= 1;	//contatore da byte a word
	for (curaddr = 0; curaddr < length; curaddr++)
	{
		UWORD val;

#ifdef	_BIG_ENDIAN_
		val  = (UWORD)(*data++) << 8;
		val |= (UWORD)(*data++);
#else
		val  = (UWORD)(*data++);
		val |= (UWORD)(*data++) << 8;
#endif
		//Send command opcode
		SendCmdOpcode(WriteCode);
		SendAddress(curaddr, address_len);
		SendDataWord(val);

#if 1
		if ( WaitReadyAfterWrite(loop_timeout) )
			return 0;		//- 07/08/99 a number >0 but != length mean "User abort"
		//	break;
#else
		WaitMsec(10);
#endif
		clearCS();
		setCS();

		if ( CheckAbort(curaddr * 100 / length) )
			break;
	}

	SendCmdOpcode(WriteEnableCode);
	SendDataWord(0, address_len);

	CheckAbort(100);

	return curaddr << 1;		//word to byte counter
}
