//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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
#include "sxbus.h"
#include "errcode.h"

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

#define	SHOT_DELAY()	WaitUsec(5)		//inb(busI->GetFirstPort())

// Costruttore
SxBus::SxBus(BusInterface *ptr)
	: BusIO(ptr)
{
}

// Distruttore
SxBus::~SxBus()
{
//	Close();
}

/**
int SxBus::Open(int port)
{
	if ( BusIO::Open(port) == OK )
	{
	}

	return GetErrNo();
}

void SxBus::Close(void)
{
	BusIO::Close();
}
**/

int SxBus::SendDataBit(int b)
{
	setCLK();		//set SCK high
	bitDI(b);
	SHOT_DELAY();
	SHOT_DELAY();
	SHOT_DELAY();
	clearCLK();		//device latch data bit now!
	SHOT_DELAY();
	SHOT_DELAY();

	return OK;
}

// returns a negative number in case of error, 0 or 1 otherwise
int SxBus::RecDataBit()
{
	register uint8_t b;

	setCLK();		//set SCK high (Pic output data now)
	SHOT_DELAY();
	SHOT_DELAY();
	SHOT_DELAY();
	b = getDO();	// sampling data on falling edge
	clearCLK();
	SHOT_DELAY();
	SHOT_DELAY();

	return b;
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int SxBus::SendDataWord(int wo, int wlen)
{
	int k;

	clearCLK();
	clearDI();

	//transmit lsb first
	for (k = 0; k < wlen; k++)
		SendDataBit(wo & (1<<k));
	setDI();

	//1 usec from a command to the next
	SHOT_DELAY();
	SHOT_DELAY();
	SHOT_DELAY();
	SHOT_DELAY();

	return OK;
}

int SxBus::RecDataWord(int wlen)
{
	int k, val = 0;

	clearCLK();
	setDI();

	//receive lsb first
	for (k = 0; k < wlen; k++)
		if ( RecDataBit() )
			val |= 1 << k;

	return val;
}

int SxBus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(10);

	return OK;
}

int SxBus::Reset(void)
{
//	ClearMCLR();

//	clearCLK();
//	clearDI();
//	WaitMsec(5);

//	SetMCLR();

//	WaitMsec(1);

	return OK;
}


long SxBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	long len = length;

	length >>= 1;	//contatore da byte a word
	while (length--)
	{	//
		//Send command opcode
//		SendCmdCode(addr ? ReadDataCode : ReadProgCode);

//		uint16_t val = RecvDataCode();
		uint16_t val = 0;

#ifdef	_BIG_ENDIAN_
		*data++ = (uint8_t)(val >> 8);
		*data++ = (uint8_t)(val & 0xFF);
#else
		*data++ = (uint8_t)(val & 0xFF);
		*data++ = (uint8_t)(val >> 8);
#endif
//		SendCmdCode(IncAddressCode);
	}

	return len;
}

long SxBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long curaddr;

	length >>= 1;	//contatore da byte a word
	for (curaddr = 0; curaddr < length; curaddr++)
	{
		uint16_t val;

		//Send command opcode
//		SendCmdCode(addr ? LoadDataCode : LoadProgCode);

#ifdef	_BIG_ENDIAN_
		val  = (uint16_t)(*data++) << 8;
		val |= (uint16_t)(*data++);
#else
		val  = (uint16_t)(*data++);
		val |= (uint16_t)(*data++) << 8;
#endif
//		SendDataCode(val);
//		SendCmdCode(BeginProgCode);
#if 0
		if ( WaitReadyAfterWrite(loop_timeout) )
			break;
#else
		WaitMsec(10);
#endif
//		SendCmdCode(IncAddressCode);
	}

	return curaddr;
}
