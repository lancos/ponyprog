//=========================================================================//
//-------------------------------------------------------------------------//
// picbus.cpp -- Source for Pic bus driver class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000  Claudio Lanconelli                            //
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
#include "picbus.h"
#include "errcode.h"

#include "e2app.h"

#ifdef	_LINUX_
#  include <asm/io.h>
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
PicBus::PicBus(BusInterface *ptr)
	: BusIO(ptr),
		DataMask(0xff),
		ProgMask(0x3fff),
		ReadProgCode(0x04),
		LoadProgCode(0x02),
		ReadDataCode(0x05),
		LoadDataCode(0x03),
		LoadConfigCode(0x00),
		IncAddressCode(0x06),
		BeginEraseProgCode(0x08),
		BeginProgOnlyCode(0x18),
		EraseProgMem(0x09),
		EraseDataMem(0x0b)
{
}

// Distruttore
PicBus::~PicBus()
{
	Close();
}

void PicBus::SetDelay()
{
    int val = THEAPP->GetPICSpeed();
    int n;

    switch(val)
    {
    case TURBO:
        n = 1;
        break;
    case FAST:
        n = 2;
        break;
    case SLOW:
        n = 15;
        break;
    default:
        n = 6;         //Default (< 100KHz)
        break;
    }
    BusIO::SetDelay(n);

    UserDebug1(UserApp2, "PICBus::SetDelay() = %d\n", n);
}

int PicBus::SendDataBit(int b)
{
	setCLK();		//set SCK high
	bitDI(b);

	WaitUsec(shot_delay);

	clearCLK();		//device latch data bit now!

	WaitUsec(shot_delay);

	return OK;
}

// returns a negative number in case of error, 0 or 1 otherwise
int PicBus::RecDataBit()
{
	register UBYTE b;

	setCLK();		//set SCK high (Pic output data now)

	WaitUsec(shot_delay);

	b = getDO();	// sampling data on falling edge
	clearCLK();

	WaitUsec(shot_delay);

	return b;
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int PicBus::SendDataWord(long wo, int wlen)
{
	int k;

	clearCLK();
	clearDI();

	//transmit lsb first
	for (k = 0; k < wlen; k++)
		SendDataBit(wo & (1<<k));
	setDI();

	//1 usec from a command to the next
	WaitUsec(shot_delay/4+1);

	return OK;
}

long PicBus::RecDataWord(int wlen)
{
	int k;
	long val = 0;

	clearCLK();
	setDI();

	//receive lsb first
	for (k = 0; k < wlen; k++)
		if ( RecDataBit() )
			val |= 1 << k;

	WaitUsec(shot_delay/4+1);

	return val;
}

int PicBus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(10);

	return OK;
}

int PicBus::Reset(void)
{
	SetDelay();

	SetMCLR();
	WaitMsec(150);	//Time to charge the cap with last jdm84 interface

	ClearMCLR();
//	setDI();
//	setCLK();
//	WaitMsec(10);

	clearCLK();
	clearDI();
	WaitMsec(15);

	SetMCLR();
	WaitMsec(5);
/**
	int k;
	UWORD val = 0;
	for (k = 0, val = 0; val == 0 && k < 100; k++)
	{
		SendCmdCode(ReadDataCode);

		val = (UWORD)RecDataWord();
	}
	return (val != 0);
**/
	return OK;
}


void PicBus::DisableCodeProtect()
{
	SendCmdCode(LoadConfigCode);
	SendProgCode(0xffff);

	//go to location 0x2007
	int k;
	for (k = 0; k < 7; k++)
		SendCmdCode(IncAddressCode);

	SendCmdCode(0x01);
	SendCmdCode(0x07);
	SendCmdCode(BeginEraseProgCode);
	WaitReadyAfterWrite();
	SendCmdCode(0x01);
	SendCmdCode(0x07);

	WaitMsec(5);
}

long PicBus::ReadConfig(UWORD *data)
{
	if (data == 0)	//read only configuration word
		return -1;

	SendCmdCode(LoadConfigCode);
	SendProgCode(0xffff);

	int k;
	for (k = 0; k < 8; k++, data++)
	{
		UBYTE *bp = (UBYTE *)data;
		UWORD val;

		SendCmdCode(ReadProgCode);

		val = RecvProgCode();

		if (val == 0x3fff)
			val = 0xffff;

#ifdef	_BIG_ENDIAN_
		*bp++ = (UBYTE)(val >> 8);
		*bp++ = (UBYTE)(val & 0xFF);
#else
		*bp++ = (UBYTE)(val & 0xFF);
		*bp++ = (UBYTE)(val >> 8);
#endif
		SendCmdCode(IncAddressCode);
	}

	return OK;
}

long PicBus::WriteConfig(UWORD *data)
{
	if (data == 0)
		return -1;

	SendCmdCode(LoadConfigCode);
//	SendProgCode(0xffff);
	SendProgCode(data[7]);		//Other PIC programmers do this

	int k;
	for (k = 0; k < 8; k++, data++)
	{
		if (*data != 0xffff)
		{
			UBYTE *bp = (UBYTE *)data;
			UWORD val;

			//Write Data code
			SendCmdCode(LoadProgCode);

#ifdef	_BIG_ENDIAN_
			val  = (UWORD)(*bp++) << 8;
			val |= (UWORD)(*bp++);
#else
			val  = (UWORD)(*bp++);
			val |= (UWORD)(*bp++) << 8;
#endif
			SendProgCode(val);
			SendCmdCode(BeginEraseProgCode);

			if ( WaitReadyAfterWrite() )
				break;

			//Verify while programming (10/11/99)
			SendCmdCode(ReadProgCode);
			if ( CompareSingleWord(val, RecvProgCode(), ProgMask) )
				return -3;
		}
		SendCmdCode(IncAddressCode);
	}

	return OK;
}

//11/09/99
int PicBus::Erase()
{
	DisableCodeProtect();

	SendCmdCode(LoadProgCode);
	SendProgCode(0xffff);
	SendCmdCode(EraseProgMem);
	SendCmdCode(BeginEraseProgCode);

	WaitMsec(20);

	SendCmdCode(LoadDataCode);
	SendDataCode(0xffff);
	SendCmdCode(EraseDataMem);
	SendCmdCode(BeginEraseProgCode);

	WaitMsec(20);

	Reset();

	return OK;
}

long PicBus::Read(int addr, UBYTE *data, long length)
{
	long len;

	//Data is 8 bit, while Program is 14 bit (i.e. 16 bit)
	if (addr == 0)
		length >>= 1;	//contatore da byte a word

	for (len = 0; len < length; len++)
	{	//
		//Send command opcode
		if (addr)
		{
			//Read DataCode
			SendCmdCode(ReadDataCode);
			*data++ = (UBYTE)RecvDataCode();

			SendCmdCode(IncAddressCode);
		}
		else
		{
			//Read Program Code
			SendCmdCode(ReadProgCode);
			UWORD val = RecvProgCode();

			if (val == 0x3fff)
				val = 0xffff;

#ifdef	_BIG_ENDIAN_
			*data++ = (UBYTE)(val >> 8);
			*data++ = (UBYTE)(val & 0xFF);
#else
			*data++ = (UBYTE)(val & 0xFF);
			*data++ = (UBYTE)(val >> 8);
#endif
			SendCmdCode(IncAddressCode);
		}

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	if (addr == 0)
		len <<= 1;	//contatore da word a byte

	return len;
}

long PicBus::Write(int addr, UBYTE const *data, long length)
{
	long len;

	if (addr == 0)
		length >>= 1;	//contatore da byte a word

//	while (length--)
	for (len = 0; len < length; len++)
	{
		UWORD val;

		//Send command opcode
		if (addr)
		{
			//Write Data code
			SendCmdCode(LoadDataCode);
			val  = (UWORD)(*data++);
			SendDataCode(val);
			SendCmdCode(BeginEraseProgCode);

			if ( WaitReadyAfterWrite() )
				break;

			//Verify while programming (10/11/99)
			SendCmdCode(ReadDataCode);
			if ( CompareSingleWord(val, RecvDataCode(), DataMask) )
				break;

			SendCmdCode(IncAddressCode);
		}
		else
		{
			//Write Program code
			SendCmdCode(LoadProgCode);

#ifdef	_BIG_ENDIAN_
			val  = (UWORD)(*data++) << 8;
			val |= (UWORD)(*data++);
#else
			val  = (UWORD)(*data++);
			val |= (UWORD)(*data++) << 8;
#endif
			SendProgCode(val);
			SendCmdCode(BeginEraseProgCode);

			if ( WaitReadyAfterWrite() )
				break;

			//Verify while programming (10/11/99)
			SendCmdCode(ReadProgCode);
			if ( CompareSingleWord(val, RecvProgCode(), ProgMask) )
				break;

			SendCmdCode(IncAddressCode);
		}

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	if (addr == 0)
		len <<= 1;	//contatore da word a byte

	return len;
}

int PicBus::CompareSingleWord(UWORD data1, UWORD data2, UWORD mask)
{
	return (data1 & mask) != (data2 & mask);
}

int PicBus::CompareMultiWord(UBYTE *data1, UBYTE *data2, ULONG length, int prog)
{
	int retval = 0;

	if ( data1 == 0 || data2 == 0 || (length & 1) != 0 )
		return -2;

	if (prog)
	{
		ULONG k;
		for (k = 0; k < length; k += 2)
		{
			UWORD val1, val2;

#ifdef	_BIG_ENDIAN_
			val1  = (UWORD)(*data1++) << 8;
			val1 |= (UWORD)(*data1++);

			val2  = (UWORD)(*data2++) << 8;
			val2 |= (UWORD)(*data2++);
#else
			val1  = (UWORD)(*data1++);
			val1 |= (UWORD)(*data1++) << 8;

			val2  = (UWORD)(*data2++);
			val2 |= (UWORD)(*data2++) << 8;
#endif
			if ( (retval = CompareSingleWord(val1, val2, ProgMask)) )
				break;		//Stop if a difference
		}
	}
	else
	{
		retval = memcmp(data1, data2, length);
	}

	return retval;
}
