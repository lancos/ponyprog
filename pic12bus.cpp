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
#include "pic12bus.h"
#include "errcode.h"

#include "e2app.h"

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

#define	MAX_PROG_PULSES	8

// Constructor
Pic12Bus::Pic12Bus(BusInterface *ptr)
	: BusIO(ptr),
//		DataMask(0xff),
		ProgMask(0x0fff),		//12bit instruction

		ReadProgCode(0x04),			//Read Data from Program Memory *
		LoadProgCode(0x02),			//Load Data for Program Memory *

//		ReadDataCode(0x05),			//Read Data from Data Memory *
//		LoadDataCode(0x03),			//Load Data for Data Memory *
//		LoadConfigCode(0x00),		//Load Configuration *
		IncAddressCode(0x06),		//Increment Address *
//		EraseProgMem(0x09),			//Bulk Erase Program Memory *
//		EraseDataMem(0x0b),			//Bulk Erase Data Memory *
		BeginProgCode(0x08),	//Begin Erase Programming Cycle *
//		BeginProgOnlyCode(0x18)		//Begin Programming Only Cycle
		EndProgCode(0x0e)
{
	UserDebug(Constructor, "Pic12Bus::Pic12Bus()\n");

	OverProgrammingMult = 11;		//Default OverProgramming X value (x11)
	OverProgrammingAdd = 0;			//Default OverProgramming + value (+0)
}

// Desctructor
Pic12Bus::~Pic12Bus()
{
//	Close();
}

void Pic12Bus::SetDelay()
{
	int val = THEAPP->GetPICSpeed();
	int n;

	switch(val)
	{
	case TURBO:
		n = 1;
		break;
	case FAST:
		n = 3;
		break;
	case SLOW:
		n = 20;
		break;
	case VERYSLOW:
		n = 100;
		break;
	case ULTRASLOW:
		n = 1000;
		break;
	default:
		n = 8;         //Default (< 100KHz)
		break;
	}
	BusIO::SetDelay(n);

	UserDebug1(UserApp2, "PIC12Bus::SetDelay() = %d\n", n);
}

int Pic12Bus::SendDataBit(int b)
{
	setCLK();		//set SCK high
	bitDI(b);

	WaitUsec(shot_delay);

	clearCLK();		//device latch data bit now!

	WaitUsec(shot_delay);

	return OK;
}

// returns a negative number in case of error, 0 or 1 otherwise
int Pic12Bus::RecDataBit()
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
int Pic12Bus::SendDataWord(long wo, int wlen)
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

long Pic12Bus::RecDataWord(int wlen)
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

int Pic12Bus::Reset(void)
{
	UserDebug(UserApp2, "Pic12Bus::Reset() IN\n");

	SetDelay();

	SetMCLR();		//First bogus entry to charge capacitors
	WaitMsec(200);		//150

	clearDI();
	ClearMCLR();		//Now reset the micro
	setCLK();		//keep Vdd on
	WaitMsec(20);

	clearCLK();		//Prepare for Program mode entry
	WaitMsec(10);

	SetMCLR();		//Program mode entry
	WaitMsec(10);

	current_address = -1;

	UserDebug(UserApp2, "Pic12Bus::Reset() OUT\n");

	return OK;
}

long Pic12Bus::ReadConfig(UWORD &data)
{
	UserDebug1(UserApp2, "Pic12Bus::ReadConfig(%x) IN\n", data);

//	Reset();

	UBYTE *bp = (UBYTE *)&data;

	//Read Program Code
	SendCmdCode(ReadProgCode);
	UWORD val = RecvProgCode();

	if (val == ProgMask)
		val = 0xffff;

#ifdef	_BIG_ENDIAN_
	*bp++ = (UBYTE)(val >> 8);
	*bp++ = (UBYTE)(val & 0xFF);
#else
	*bp++ = (UBYTE)(val & 0xFF);
	*bp++ = (UBYTE)(val >> 8);
#endif
	IncAddress(1);

	UserDebug1(UserApp2, "Pic12Bus::ReadConfig(%x) OUT\n", data);

	return OK;
}

long Pic12Bus::WriteConfig(UWORD data)
{
	UserDebug1(UserApp2, "Pic12Bus::WriteConfig(%x) IN\n", data);

//  	Reset();

	UBYTE *bp = (UBYTE *)&data;
	UWORD val;

	//Write Program code
#ifdef	_BIG_ENDIAN_
	val  = (UWORD)(*bp++) << 8;
	val |= (UWORD)(*bp++);
#else
	val  = (UWORD)(*bp++);
	val |= (UWORD)(*bp++) << 8;
#endif
	int k;
	for (k = 100; k > 0; k--)
		ProgramPulse(val, 0);

	IncAddress(1);

	UserDebug1(UserApp2, "Pic12Bus::WriteConfig(%x) OUT\n", data);

	return OK;
}

long Pic12Bus::BlankCheck(long length)
{
	length >>= 1;	//contatore da byte a word

	//Point to first location
//	SendCmdCode(IncAddressCode);

	long len;
	for (len = 0; len < length-1; len++)	//Skip last location (RC calibration)
	{
		//Read Program Code
		SendCmdCode(ReadProgCode);
		if ( CompareSingleWord(0xffff, RecvProgCode(), ProgMask) )
			break;

		if ( CheckAbort(len * 100 / length) )
			break;

		IncAddress(1);
	}
	CheckAbort(100);

	return (len == length);
}

long Pic12Bus::Read(int addr, UBYTE *data, long length, int page_size)
{
	long len;

	UserDebug3(UserApp2, "Pic12Bus::Read(%d, %ph, %ld) IN\n", addr, data, length);

	length >>= 1;	//contatore da byte a word

	//Point to first location
//	SendCmdCode(IncAddressCode);

	for (len = 0; len < length; len++)
	{
		//Read Program Code
		SendCmdCode(ReadProgCode);
		UWORD val = RecvProgCode();

		if (val == ProgMask)
			val = 0xffff;

#ifdef	_BIG_ENDIAN_
		*data++ = (UBYTE)(val >> 8);
		*data++ = (UBYTE)(val & 0xFF);
#else
		*data++ = (UBYTE)(val & 0xFF);
		*data++ = (UBYTE)(val >> 8);
#endif
		IncAddress(1);

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	len <<= 1;	//contatore da word a byte

	UserDebug1(UserApp2, "Pic12Bus::Read() = %ld OUT\n", len);

	return len;
}

long Pic12Bus::Write(int addr, UBYTE const *data, long length, int page_size)
{
	long len;
	int rv = OK;

	UserDebug3(UserApp2, "Pic12Bus::Write(%d, %ph, %ld) IN\n", addr, data, length);

	length >>= 1;	//contatore da byte a word

	//The address pointer should already point to first address
	//location (via a ConfigRead or IncAddress)

	//Program cycle
	for (len = 0; len < length; len++)
	{
		UWORD val;

		//Write Program code
#ifdef	_BIG_ENDIAN_
		val  = (UWORD)(*data++) << 8;
		val |= (UWORD)(*data++);
#else
		val  = (UWORD)(*data++);
		val |= (UWORD)(*data++) << 8;
#endif
		rv = WriteProgWord(val, length-1);

		if ( rv != OK )
		{
			len = rv;
			break;
		}

		if ( CheckAbort(len * 100 / length) )
			break;
	}
	CheckAbort(100);

	if (len > 0)
		len <<= 1;	//contatore da word a byte

	UserDebug2(UserApp2, "Pic12Bus::Write() = %ld ** %ld OUT\n", len, GetLastProgrammedAddress());

	return len;
}

int Pic12Bus::WriteProgWord(UWORD val, long rc_addr)
{
	int k;
	int rval = OK;

	UserDebug2(UserApp2, "Pic12Bus::WriteProgWord(%x, %ld) IN\n", val, current_address);

	//Check for RC calibration location
	if (current_address == rc_addr)
	{
		//Check for blank (erased cells)
		//Programma la RC calibration solamente se la locazione e` cancellata
		// e il valore da pgrogrammare corrisponde ad una MOVLW xx (0x0Cxx)
		SendCmdCode(ReadProgCode);
		if ( CompareSingleWord(RecvProgCode(), 0xffff, ProgMask) == 0 &&
			 CompareSingleWord(val, 0x0C00, (ProgMask & 0xff00)) == 0 )
		{
			SetLastProgrammedAddress(current_address << 1);

			for (k = 1; k <= MAX_PROG_PULSES; k++)
			{
				if ( ProgramPulse(val, 1) == OK )
					break;
			}
			if (k > MAX_PROG_PULSES)
			{
				rval = E2ERR_WRITEFAILED;	//Write error
			}
			else
			{
				UserDebug1(UserApp2, "Pic12Bus::WriteProgWord(): Npulses = %d\n", k);

				k *= OverProgrammingMult;
				k += OverProgrammingAdd;
				while (k--)
					ProgramPulse(val, 0);	//Program pulse without test
			}
		}

		IncAddress(1);
	}
	else
	{
		//Check for blank (erased cells)
		SendCmdCode(ReadProgCode);
		if ( CompareSingleWord(RecvProgCode(), 0xffff, ProgMask) )
		{
			rval = E2ERR_BLANKCHECKFAILED;
		}
		else
		{
			//Skip FFF words
			if ( CompareSingleWord(val, 0xffff, ProgMask) != 0 )
			{
				SetLastProgrammedAddress(current_address << 1);

				for (k = 1; k <= MAX_PROG_PULSES; k++)
				{
					if ( ProgramPulse(val, 1) == OK )
						break;
				}
				if (k > MAX_PROG_PULSES)
				{
					rval = E2ERR_WRITEFAILED;	//Write error
				}
				else
				{
					UserDebug1(UserApp2, "Pic12Bus::WriteProgWord(): Npulses = %d\n", k);

					k *= OverProgrammingMult;
					k += OverProgrammingAdd;
					while (k--)
						ProgramPulse(val, 0);	//Program pulse without test
				}
			}

			IncAddress(1);
		}
	}

	UserDebug1(UserApp2, "Pic12Bus::WriteProgWord() = %d OUT\n", rval);

	return rval;
}

void Pic12Bus::IncAddress(int n)
{
	UserDebug1(UserApp3, "Pic12Bus::IncAddress(%d) IN\n", n);

	while (n--)
	{
		SendCmdCode(IncAddressCode);
		current_address++;
	}

	UserDebug1(UserApp3, "Pic12Bus::IncAddress() OUT ** cur_addr = %ld\n", current_address);
}

int Pic12Bus::ProgramPulse(UWORD val, int verify, int width)
{
	int rval = OK;

	UserDebug3(UserApp3, "Pic12Bus::ProgramPulse(%x, %d, %d) IN\n", val, verify, width);

	SendCmdCode(LoadProgCode);
	SendProgCode(val);

	SendCmdCode(BeginProgCode);	//Start programming pulse
	WaitUsec(width);
	SendCmdCode(EndProgCode);	//Stop programming pulse

	WaitUsec(1000);		//wait between pulses

	//Verify programmed location
	if (verify)
	{
		SendCmdCode(ReadProgCode);
		rval = CompareSingleWord(val, RecvProgCode(), ProgMask);
	}

	UserDebug1(UserApp3, "Pic12Bus::ProgramPulse() = %d OUT\n", rval);

	return rval;
}

int Pic12Bus::CompareSingleWord(UWORD data1, UWORD data2, UWORD mask)
{
	return (data1 & mask) != (data2 & mask);
}

int Pic12Bus::CompareMultiWord(UBYTE *data1, UBYTE *data2, ULONG length, int split)
{
	int retval = 0;

	if ( data1 == 0 || data2 == 0 || (length & 1) != 0 )
		return BADPARAM;

	if (!split)
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
