//=========================================================================//
//-------------------------------------------------------------------------//
// at90sbus.cpp -- AT90Sxxxx (AVR micro) Bus Driver                        //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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
#include "at90sbus.h"
#include "errcode.h"

#include "e2app.h"

/*Attenzione!! il format Intel Hex e` Little Endian */
#undef	_BIG_ENDIAN_

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

#define	PAGE_SIZE	256

// Costruttore
At90sBus::At90sBus(BusInterface *ptr)
	: SPIBus(ptr),
		EnableProg0(0xAC), EnableProg1(0x53),
		ChipErase0(0xAC), ChipErase1(0x80),
		ReadProgMemH0(0x28), ReadProgMemH1(0),
		ReadProgMemL0(0x20), ReadProgMemL1(0),
		WriteProgMemH0(0x48), WriteProgMemH1(0),
		WriteProgMemL0(0x40), WriteProgMemL1(0),
		WriteProgPageMem(0x4C),
		ReadEEPMem0(0xA0), ReadEEPMem1(0),
		WriteEEPMem0(0xC0), WriteEEPMem1(0),
		ReadDevCode0(0x30), ReadDevCode1(0),
		WriteLock0(0xAC), WriteLock1(0xFF),
		WriteFuse0(0xAC), WriteFuse1(0xBF),
		ReadLock0(0x58), ReadLock1(0),
		ReadFuse0(0x50), ReadFuse1(0),
		p1_a(0x80), p2_a(0x7F), pflash_a(0x7F),
		p1_b(0x00), p2_b(0xFF), pflash_b(0xFF)
{
	UserDebug(Constructor, "At90sBus::At90sBus()\n");

	//With this values the AVR can be programmed even at low voltage (3.2V)
	twd_erase = 30;
	twd_prog = 10;
}

//limit EEPROM size to 64K max
int At90sBus::ReadEEPByte(long addr)
{
	SendDataByte(ReadEEPMem0);
	SendDataByte(ReadEEPMem1 | ((addr & 0xFFFF) >> 8) );	//19/01/1999 -- the bug is due to an error in the original Atmel datasheet
	SendDataByte(addr);

	return RecDataByte();
}

void At90sBus::WriteEEPByte(long addr, int data)
{
	SendDataByte(WriteEEPMem0);
	SendDataByte(WriteEEPMem1 | ((addr & 0xFFFF) >> 8) );		//19/01/1999
	SendDataByte(addr);
	SendDataByte(data);
}


int At90sBus::ReadProgByte(long addr)
{
	int lsb = addr & 1;
	addr >>= 1;		//L'indirizzo e` di Word, non di byte

	//Se fosse little-endian sarebbe l'inverso
#ifdef	_BIG_ENDIAN_
	if (!lsb)
#else
	if (lsb)
#endif
	{
		SendDataByte(ReadProgMemH0);
		SendDataByte(ReadProgMemH1 | (addr >> 8));
	}
	else
	{
		SendDataByte(ReadProgMemL0);
		SendDataByte(ReadProgMemL1 | (addr >> 8));
	}
	SendDataByte(addr);

	return RecDataByte();
}

void At90sBus::WriteProgByte(long addr, int data)
{
	THEAPP->SetLastProgrammedAddress(addr);

	int lsb = addr & 1;
	addr >>= 1;		//L'indirizzo e` di Word, non di byte

	//Se fosse little-endian sarebbe l'inverso
#ifdef	_BIG_ENDIAN_
	if (!lsb)
#else
	if (lsb)
#endif
	{
		SendDataByte(WriteProgMemH0);
		SendDataByte(WriteProgMemH1 | (addr >> 8));
	}
	else
	{
		SendDataByte(WriteProgMemL0);
		SendDataByte(WriteProgMemL1 | (addr >> 8));
	}
	SendDataByte(addr);
	SendDataByte(data);
}

int At90sBus::Reset()
{
	UserDebug(UserApp2, "At90sBus::Reset() I\n");

	int success_flag = 0;

	RefreshParameters();

	int j;
	for (j = 0; j < 4 && !success_flag; j++)
	{
		int val = 0;

		SPIBus::Reset();

		WaitMsec(50);	// At least 20msec (AVR datasheets)

		int k;
		for (k = 0; k < 32 && !success_flag; k++)
		{
			UserDebug(UserApp2, "At90sBus::Reset() ** SendEnableProg\n");

			SendDataByte(EnableProg0);
			SendDataByte(EnableProg1);
			val = RecDataByte();
			SendDataByte(0);

			if (val != EnableProg1)		//Echo expected
			{
				RecDataBit();		//Give a pulse on SCK (as AVR datasheets suggest)
			}
			else
			{
				success_flag = 1;
			}
		}
	}

	return success_flag;
}

int At90sBus::WriteLockBits(int byte)
{
	int mask = WriteLock1 & ~(byte & 0x1F);

	SendDataByte(WriteLock0);
	SendDataByte(mask);
	SendDataByte(0);
	SendDataByte(0);

	WaitMsec(twd_prog);

	return OK;
}

int At90sBus::WriteFuseBits(int byte)
{
	int mask1 = WriteFuse1 & ~(byte & 0x1F);
	int mask2 = (~byte) & 0xFF;		//Tiny12 Fuses

	SendDataByte(WriteFuse0);
	SendDataByte(mask1);
	SendDataByte(0);
	SendDataByte(mask2);

	WaitMsec(twd_prog);

	return OK;
}

int At90sBus::ReadFuseBits()
{
	int retval;

	SendDataByte(ReadFuse0);
	SendDataByte(ReadFuse1);
	SendDataByte(0);
	retval = RecDataByte();

	return retval;
}

int At90sBus::ReadLockBits()
{
	int retval;

	SendDataByte(ReadLock0);
	SendDataByte(ReadLock1);
	SendDataByte(0);
	retval = RecDataByte();

	return retval;
}

int At90sBus::ReadDeviceCode(int addr)
{
	SendDataByte(ReadDevCode0);
	SendDataByte(ReadDevCode1);
	SendDataByte(addr & 3);

	return RecDataByte();
}

long At90sBus::Read(int addr, UBYTE *data, long length)
{
	long len;
	int code[3];

	code[0] = ReadDeviceCode(0);
	code[1] = ReadDeviceCode(1);
	code[2] = ReadDeviceCode(2);

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

int At90sBus::WaitReadyAfterWrite(int type, long addr, int data, long timeout)
{
	int rval;

	if (type)
	{	//EEprom
		if (data == p1_a || data == p2_a ||
			data == p1_b || data == p2_b)
		{
			rval = OK;
			WaitMsec(twd_prog);
		}
		else
		{
			rval = -1;

			int k;
			for (k = 0; k < timeout; k++)
			{
				int val = ReadEEPByte(addr);

				if (val == data)
				{
					rval = OK;
					break;
				}
			/**
				else
				if (val != p1_a && val != p2_a &&
					val != p1_b && val != p2_b)
				{
				//	rval = OK;
				//	if (val != data)
				//		WaitMsec(twd_prog);
					break;
				}
			**/
			}
		}

	}
	else
	{	//Flash
		if (data == pflash_a ||
			data == pflash_b)
		{
			rval = OK;
			WaitMsec(twd_prog);
		}
		else
		{
			rval = -1;

			int k;
			for (k = 0; k < timeout; k++)
			{
				int val = ReadProgByte(addr);

				if (val == data)
				{
					rval = OK;
					break;
				}
			/**
				else
				if (val != pflash_a &&
					val != pflash_b)
				{
				//	rval = OK;
				//	if (val != data)
				//		WaitMsec(twd_prog);
					break;
				}
			**/
			}
		}
	}

	return rval;
}

//11/09/99
int At90sBus::Erase()
{
	//Erase command
	SendDataByte(ChipErase0);
	SendDataByte(ChipErase1);
	SendDataByte(0);
	SendDataByte(0);

	WaitMsec(twd_erase);
	Reset();

	return OK;
}

long At90sBus::Write(int addr, UBYTE const *data, long length)
{
	long len;

	if (addr)
	{	//EEprom
		for (addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			//09/10/98 -- program only locations that really need to be programmed
			int val = ReadEEPByte(addr);

			if (val != *data)
			{
				WriteEEPByte(addr, *data);

				//Interrupt the writing and exit (device missing?)
				if ( WaitReadyAfterWrite(1, addr, *data) != OK )
					return E2ERR_WRITEFAILED;
			}

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}
	else
	{	//Flash Eprom

	/**	11/09/99
		//Erase command
		SendDataByte(ChipErase0);
		SendDataByte(ChipErase1);
		SendDataByte(0);
		SendDataByte(0);

		WaitMsec(twd_erase);
		Reset();
	**/

		//Write
		for(addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			//09/10/98 -- program only locations not equal to FF (erase set all FF)
			if (*data != 0xFF)
			{
				WriteProgByte(addr, *data);

				if ( WaitReadyAfterWrite(0, addr, *data) != OK )
					return E2ERR_WRITEFAILED;
			}

			if ( CheckAbort(len * 100 / length) )
				break;
		}
		CheckAbort(100);
	}

	return len;
}

void At90sBus::RefreshParameters()
{
	twd_prog = THEAPP->GetAVRProgDelay();
	twd_erase = THEAPP->GetAVREraseDelay();
}
