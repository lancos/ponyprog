//=========================================================================//
//-------------------------------------------------------------------------//
// at89sbus.cpp -- AT90Sxxxx (AVR micro) Bus Driver                        //
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
#include "at89sbus.h"
#include "errcode.h"

#include "e2app.h"

/*Attenzione!! il format Intel Hex e` Little Endian */
#undef	_BIG_ENDIAN_

#ifndef	_LINUX_
#  ifdef	__BORLANDC__
#    define	__inline__
#  else // _MICROSOFT_ VC++
#    define	__inline__ __inline
#    define _export
#  endif
#endif

// Costruttore
At89sBus::At89sBus(BusInterface *ptr)
	: At90sBus(ptr),
		EnableProg0(0xAC), EnableProg1(0x53),
		ChipErase0(0xAC), ChipErase1(0x04),
		ReadProgMem(0x01),
		WriteProgMem(0x02),
		ReadEEPMem(0x05),
		WriteEEPMem(0x06),
		WriteLock0(0xAC), WriteLock1(0x07)
{
}

int At89sBus::ReadEEPByte(long addr)
{
	SendDataByte(ReadEEPMem | ((addr >> 5) & 0xF8));
	SendDataByte(addr & 0xFF);

	return RecDataByte();
}

void At89sBus::SetDelay()
{
	int val = THEAPP->GetSPISpeed();
	int n;

	switch(val)
	{
	case TURBO:
		n = 1;
		break;
	case FAST:
		n = 5;
		break;
	case SLOW:
		n = 30;
		break;
	default:
		n = 15;
		break;
	}
	BusIO::SetDelay(n);

	UserDebug1(UserApp2, "At89sBus::SetDelay() = %d\n", n);
}

void At89sBus::WriteEEPByte(long addr, int data)
{
	SendDataByte(WriteEEPMem | ((addr >> 5) & 0xF8));
	SendDataByte(addr & 0xFF);
	SendDataByte(data);
}


int At89sBus::ReadProgByte(long addr)
{
	SendDataByte(ReadProgMem | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04) );
	SendDataByte(addr & 0xFF);

	return RecDataByte();
}

void At89sBus::WriteProgByte(long addr, int data)
{
	SendDataByte(WriteProgMem | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04));
	SendDataByte(addr & 0xFF);
	SendDataByte(data);
}

int At89sBus::Reset(void)
{
	SPIBus::Reset();

	SetDelay();

	WaitMsec(50);	// At least 20msec (AVR datasheet)

	SendDataByte(EnableProg0);
	SendDataByte(EnableProg1);
	SendDataByte(0);
//	int val = RecDataByte();
	return OK;
}

int At89sBus::WriteLockBits(int byte)
{
	int mask = 0xff & ~byte;

	SendDataByte(WriteLock0);
	SendDataByte(mask);	//Atmel datasheets don't specify what bits are LB1, LB2, LB3!!
	SendDataByte(0);

	WaitMsec(20);		//4msec dai datasheet Atmel

	return OK;
}

int At89sBus::WriteFuseBits(int byte)
{
	return OK;
}

int At89sBus::ReadLockBits()
{
	return OK;
}

int At89sBus::ReadFuseBits()
{
	return OK;
}

int At89sBus::ReadDeviceCode(int addr)
{
	return OK;
}

int At89sBus::Erase(int type)
{
	//Erase command
	SendDataByte(ChipErase0);
	SendDataByte(ChipErase1);
	SendDataByte(0);

	WaitMsec(twd_erase);
	Reset();

	return OK;
}

long At89sBus::Read(int addr, UBYTE *data, long length)
{
	long len;
//	int code[3];
//
//	code[0] = ReadDeviceCode(0);
//	code[1] = ReadDeviceCode(1);
//	code[2] = ReadDeviceCode(2);

	if (addr)
	{	//EEprom
		addr = 0;
		for (len = 0; len < length; len++)
		{
			*data++ = (UBYTE)ReadEEPByte(addr++);

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}
	else
	{	//Flash Eprom
		addr = 0;
		for (len = 0; len < length; len++)
		{
			*data++ = (UBYTE)ReadProgByte(addr++);

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}

	return len;
}

int At89sBus::WaitReadyAfterWrite(int type, long addr, int data, long timeout)
{
	int rval;

#if 0
	rval = OK;
	WaitMsec(5);		//4msec dai datasheet Atmel
#else
	rval = -1;

	int k;
	for (k = 0; k < timeout; k++)
	{
		int val;
		if (type)
			val = ReadEEPByte(addr);
		else
			val = ReadProgByte(addr);

		if (val == data)
		{
			rval = OK;
			break;
		}
	}
#endif

	return rval;
}

long At89sBus::Write(int addr, UBYTE const *data, long length)
{
	long len;

	if (addr)
	{	//EEprom
		for (addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			int val = ReadEEPByte(addr);

			if (val != *data)
			{
				WriteEEPByte(addr, *data);

				//Interrupt the writing and exit (device missing?)
				if ( WaitReadyAfterWrite(1, addr, *data) != OK )
					return 0;
			}

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}
	else
	{	//Flash Eprom
		for (addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			int val = ReadProgByte(addr);

			if (val != *data)
			{
				WriteProgByte(addr, *data);

				//Interrupt the writing and exit (device missing?)
				if ( WaitReadyAfterWrite(0, addr, *data) != OK )
					return 0;
			}

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}

	return len;
}
