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
#include "at90sbus.h"
#include "errcode.h"
#include "eeptypes.h"

#include "e2app.h"

//Pay attention that Intel Hex format is Little Endian
#undef	_BIG_ENDIAN_

// Constructor
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
		ReadLock0(0x58), ReadLock1(0),
		WriteLock0(0xAC), WriteLock1(0xE0),
		ReadFuse0(0x50), ReadFuse1(0),
		WriteFuse0(0xAC), WriteFuse1a(0xBF), WriteFuse1b(0xA0),
		ReadFuseHigh0(0x58), ReadFuseHigh1(0x08),
		WriteFuseHigh0(0xAC), WriteFuseHigh1(0xA8),
		ReadFuseExt0(0x50), ReadFuseExt1(0x08),
		WriteFuseExt0(0xAC), WriteFuseExt1(0xA4),
		ReadCalib0(0x38), ReadCalib1(0),
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

		WaitMsec( THEAPP->GetAVRDelayAfterReset() );	// At least 20msec (AVR datasheets)

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

int At90sBus::WriteLockBits(DWORD param, long model)
{
	int val1, val2, val3, val4;
	int val5, val6, val7, val8;

	val1 = -1;
	val2 = val3 = val4 = 0;

	val5 = -1;
	val6 = val7 = val8 = 0;

	switch (model)
	{
	case AT90S1200:
	case AT90S2313:
	case AT90S4414:
	case AT90S8515:
	case AT90S2323:
	case AT90S2343:
	case AT90S2333:
	case AT90S4433:
	case AT90S4434:
	case AT90S8535:
	case ATmega603:
	case ATmega103:
	case ATtiny12:
	case ATtiny15:
	case ATtiny22:
		val1 = WriteLock0;
		val2 = ~(param & 0x06);
		break;

	case ATtiny2313:
	case ATtiny26:
	case ATtiny13:
	case ATtiny25: case ATtiny45: case ATtiny85:
	case ATtiny261: case ATtiny461: case ATtiny861:
		val1 = WriteLock0;
		val2 = WriteLock1;
		//
		val4 = ~(param & 0x03);
		break;

	case ATmega8:
	case ATmega16:
	case ATmega161:
	case ATmega163:
	case ATmega323:
	case ATmega128:
	case ATmega64:
	case ATmega32:
	case ATmega162:
	case ATmega169:
	case ATmega8515: case ATmega8535:
	case ATmega48: case ATmega88: case ATmega168:
	case ATmega164: case ATmega324: case ATmega644:
	case ATmega640: case ATmega1280: case ATmega1281:
	case ATmega2560: case ATmega2561:
	case AT90CAN32: case AT90CAN64: case AT90CAN128:
		val1 = WriteLock0;
		val2 = WriteLock1;
		//
		val4 = ~(param & 0x3F);
		break;

	//Only Parallel programming
	case AT90S8534:
	case ATtiny28:

	//Only HV programming
	case ATtiny11:
	case ATtiny10:
		//No Locks
		break;

	default:
		val1 = WriteLock0;
		val2 = ~(param & 0x06);
		break;
	}

	if (val1 != -1)
	{
		SendDataByte(val1);
		SendDataByte(val2);
		SendDataByte(val3);
		SendDataByte(val4);

		WaitMsec(twd_prog * 10);
	}

	if (val5 != -1)
	{
		SendDataByte(val5);
		SendDataByte(val6);
		SendDataByte(val7);
		SendDataByte(val8);

		WaitMsec(twd_prog * 10);
	}

	return OK;
}

int At90sBus::WriteFuseBits(DWORD param, long model)
{
	int val1, val2, val3, val4;
	int val5, val6, val7, val8;
	int val9, valA, valB, valC;

	val1 = -1;
	val2 = val3 = val4 = 0;

	val5 = -1;
	val6 = val7 = val8 = 0;

	val9 = -1;
	valA = valB = valC = 0;

	switch (model)
	{
	case AT90S2323:
	case AT90S2343:
	case AT90S4434:
	case AT90S8535:
		val1 = WriteFuse0;
		val2 = WriteFuse1a & ~(param & 1);
		break;

	case ATmega603:
	case ATmega103:
		val1 = WriteFuse0;
		val2 = WriteFuse1a & ~(param & 0x0B);
		break;

	case AT90S2333:
	case AT90S4433:
		val1 = WriteFuse0;
		val2 = WriteFuse1a & ~(param & 0x1F);
		break;

	//One byte fuse
	case ATtiny12:
	case ATtiny15:
	case ATmega161:
		val1 = WriteFuse0;
		val2 = WriteFuse1b;
		//
		val4 = ~(param & 0xFF);
		break;

	//Two byte fuse
	case ATtiny13:
	case ATtiny26:
	case ATmega163:
	case ATmega323:
	case ATmega8:
	case ATmega16:
	case ATmega32:
	case ATmega8515:
	case ATmega8535:
		val1 = WriteFuse0;
		val2 = WriteFuse1b;
		//
		val4 = ~(param & 0xFF);

		val5 = WriteFuseHigh0;
		val6 = WriteFuseHigh1;
		//
		val8 = ~((param >> 8) & 0xFF);
		break;

	//Three byte fuse
	case ATtiny2313:
	case ATtiny25: case ATtiny45: case ATtiny85:
	case ATtiny261: case ATtiny461: case ATtiny861:
	case ATmega128:
	case ATmega64:
	case ATmega162:
	case ATmega169:
	case ATmega48: case ATmega88: case ATmega168:
	case ATmega164: case ATmega324: case ATmega644:
	case AT90CAN32: case AT90CAN64: case AT90CAN128:
	case ATmega640: case ATmega1280: case ATmega1281:
//	case ATmega2560: case ATmega2561:
		val1 = WriteFuse0;
		val2 = WriteFuse1b;
		//
		val4 = ~(param & 0xFF);

		val5 = WriteFuseHigh0;
		val6 = WriteFuseHigh1;
		//
		val8 = ~((param >> 8) & 0xFF);

		val9 = WriteFuseExt0;
		valA = WriteFuseExt1;
		//
		valC = ~((param >> 16) & 0xFF);
		break;

	//No fuses
	case AT90S1200:
	case AT90S2313:
	case AT90S4414:
	case AT90S8515:
	case ATtiny22:
	case ATtiny11:
	case ATtiny10:
	case ATtiny28:
	case AT90S8534:
	default:
		//No Fuses
		break;
	}

	if (val1 != -1)
	{
		SendDataByte(val1);
		SendDataByte(val2);
		SendDataByte(val3);
		SendDataByte(val4);

		WaitMsec(twd_prog * 10);
	}

	if (val5 != -1)
	{
		SendDataByte(val5);
		SendDataByte(val6);
		SendDataByte(val7);
		SendDataByte(val8);

		WaitMsec(twd_prog * 10);
	}

	if (val9 != -1)
	{
		SendDataByte(val9);
		SendDataByte(valA);
		SendDataByte(valB);
		SendDataByte(valC);

		WaitMsec(twd_prog * 10);
	}

	return OK;
}

DWORD At90sBus::ReadFuseBits(long model)
{
	DWORD retval = 0;
	DWORD rv1, rv2, rv3;

	switch (model)
	{
	case ATtiny22:
		SendDataByte(ReadLock0);	//NB Read LOCK!!
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0x20;
		break;

	case AT90S2323:
	case AT90S2343:
	case AT90S4434:
	case AT90S8535:
		SendDataByte(ReadLock0);	//NB Read LOCK!!
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0x21;
		break;

	//One byte fuse
	case AT90S2333:
	case AT90S4433:
	case ATmega603:
	case ATmega103:
	case ATtiny12:
	case ATtiny15:
	case ATmega161:
		SendDataByte(ReadFuse0);
		SendDataByte(ReadFuse1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0xFF;
		break;

	//Two byte fuse
	case ATtiny13:
	case ATtiny26:
	case ATmega163:
	case ATmega323:
	case ATmega8:
	case ATmega16:
	case ATmega32:
	case ATmega8515:
	case ATmega8535:
		SendDataByte(ReadFuse0);
		SendDataByte(ReadFuse1);
		SendDataByte(0);
		rv1 = RecDataByte();
		rv1 = ~rv1 & 0xFF;

		SendDataByte(ReadFuseHigh0);
		SendDataByte(ReadFuseHigh1);
		SendDataByte(0);
		rv2 = RecDataByte();
		rv2 = ~rv2 & 0xFF;

		retval = (rv2 << 8) | rv1;
		break;

	//Three byte fuse
	case ATtiny2313:
	case ATtiny25: case ATtiny45: case ATtiny85:
	case ATtiny261: case ATtiny461: case ATtiny861:
	case ATmega128:
	case ATmega64:
	case ATmega162:
	case ATmega169:
	case ATmega48: case ATmega88: case ATmega168:
	case ATmega164: case ATmega324: case ATmega644:
	case AT90CAN32: case AT90CAN64: case AT90CAN128:
	case ATmega640: case ATmega1280: case ATmega1281:
//	case ATmega2560: case ATmega2561:
		SendDataByte(ReadFuse0);
		SendDataByte(ReadFuse1);
		SendDataByte(0);
		rv1 = RecDataByte();
		rv1 = ~rv1 & 0xFF;

		SendDataByte(ReadFuseHigh0);
		SendDataByte(ReadFuseHigh1);
		SendDataByte(0);
		rv2 = RecDataByte();
		rv2 = ~rv2 & 0xFF;

		SendDataByte(ReadFuseExt0);
		SendDataByte(ReadFuseExt1);
		SendDataByte(0);
		rv3 = RecDataByte();
		rv3 = ~rv3 & 0xFF;

		retval = (rv3 << 16) | (rv2 << 8) | rv1;
		break;

	//No fuses
	case AT90S1200:
	case AT90S2313:
	case AT90S4414:
	case AT90S8515:
	default:
		//No Fuses
		break;
	}

	return retval;
}

DWORD At90sBus::ReadLockBits(long model)
{
	DWORD retval = 0;
	int rv1, rv2;

	switch (model)
	{
	case AT90S1200:
	case AT90S2313:
	case AT90S4414:
	case AT90S8515:
		//No read locks command
		int code[3];
		code[0] = ReadDeviceCode(0);
		code[1] = ReadDeviceCode(1);
		code[2] = ReadDeviceCode(2);
		if (code[0] == 0x00 && code[1] == 0x01 && code[2] == 0x02)
			retval = 0x06;	//both lock bits programmed
		break;

	case ATtiny22:
	case AT90S2323:
	case AT90S2343:
	case AT90S4434:
	case AT90S8535:
		SendDataByte(ReadLock0);
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv2 = rv1 = RecDataByte();
		rv1 = ~rv1 & 0x80;
		rv2 = ~rv2 & 0x40;
		retval = (rv1 >> 6) | (rv2 >> 4);
		break;

	case ATtiny12:
	case ATtiny15:
	case AT90S2333:
	case AT90S4433:
	case ATmega603:
	case ATmega103:
		SendDataByte(ReadLock0);
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0x06;
		break;

	case ATtiny13:
	case ATtiny26:
	case ATtiny2313:
	case ATtiny25: case ATtiny45: case ATtiny85:
	case ATtiny261: case ATtiny461: case ATtiny861:
		SendDataByte(ReadLock0);
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0x03;
		break;

	case ATmega8:
	case ATmega16:
	case ATmega161:
	case ATmega163:
	case ATmega323:
	case ATmega128:
	case ATmega64:
	case ATmega32:
	case ATmega162:
	case ATmega169:
	case ATmega8515: case ATmega8535:
	case ATmega48: case ATmega88: case ATmega168:
	case ATmega164: case ATmega324: case ATmega644:
	case AT90CAN32: case AT90CAN64: case AT90CAN128:
	case ATmega640: case ATmega1280: case ATmega1281:
//	case ATmega2560: case ATmega2561:
		SendDataByte(ReadLock0);
		SendDataByte(ReadLock1);
		SendDataByte(0);
		rv1 = RecDataByte();
		retval = ~rv1 & 0x3F;
		break;

	default:
		//No locks
		break;
	}

	return retval;
}

int At90sBus::ReadDeviceCode(int addr)
{
	SendDataByte(ReadDevCode0);
	SendDataByte(ReadDevCode1);
	SendDataByte(addr & 3);

	return RecDataByte();
}

long At90sBus::ReadCalibration(int addr)
{
	SendDataByte(ReadCalib0);
	SendDataByte(ReadCalib1);
	SendDataByte(addr & 3);

	return RecDataByte();
}

long At90sBus::Read(int addr, UBYTE *data, long length)
{
	long len;

//	int code[3];

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
			rval = E2P_TIMEOUT;

			int k;
			for (k = 0; k < timeout; k++)
			{
				int val = ReadEEPByte(addr);

				if (val == data)
				{
					rval = OK;
					break;
				}
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
			rval = E2P_TIMEOUT;

			int k;
			for (k = 0; k < timeout; k++)
			{
				int val = ReadProgByte(addr);

				if (val == data)
				{
					rval = OK;
					break;
				}
			}
		}
	}

	return rval;
}

//11/09/99
int At90sBus::Erase(int type)
{
	//Erase command
	SendDataByte(ChipErase0);
	SendDataByte(ChipErase1);
	SendDataByte(0);
	SendDataByte(0);

	WaitMsec(twd_erase);
	Reset();

	/** Needed by ATtiny12 **/
	WriteProgByte(0, 0xFF);
	WaitMsec(twd_prog);

	//Erase command
	SendDataByte(ChipErase0);
	SendDataByte(ChipErase1);
	SendDataByte(0);
	SendDataByte(0);

	WaitMsec(twd_erase);
	Reset();
	/****/

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

				WaitUsec(100);

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
		for(addr = 0, len = 0; len < length; addr++, data++, len++)
		{
			//09/10/98 -- program only locations not equal to FF (erase set all FF)
			if (*data != 0xFF)
			{
				WriteProgByte(addr, *data);

				WaitUsec(100);

				if ( WaitReadyAfterWrite(0, addr, *data, 2000) != OK )
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
