//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2017   Claudio Lanconelli                           //
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
#include "picbus.h"
#include "errcode.h"

#include <QDebug>

#include "globals.h"
#include "e2profil.h"
#include "e2cmdw.h"

class e2CmdWindow;

#ifdef  __linux__
//#  include <asm/io.h>
#  include <unistd.h>
#else
#  ifdef        __BORLANDC__
#    define     __inline__
#  else // _MICROSOFT_ VC++
#    define     __inline__ __inline
#    define _export
#  endif
#endif

// Constructor
PicBus::PicBus(BusInterface *ptr)
	: BusIO(ptr),
	  DataMask(0xff),
	  ProgMask(0x3fff),
	  ReadProgCode(0x04),                     //Read Data from Program Memory *
	  LoadProgCode(0x02),                     //Load Data for Program Memory *
	  ReadDataCode(0x05),                     //Read Data from Data Memory *
	  LoadDataCode(0x03),                     //Load Data for Data Memory *
	  LoadConfigCode(0x00),           //Load Configuration *
	  IncAddressCode(0x06),           //Increment Address *
	  EraseProgMem(0x09),                     //Bulk Erase Program Memory *
	  EraseDataMem(0x0b),                     //Bulk Erase Data Memory *
	  BeginEraseProgCode(0x08),       //Begin Erase Programming Cycle *
	  BeginProgOnlyCode(0x18)         //Begin Programming Only Cycle
{
}

// Desctructor
PicBus::~PicBus()
{
	//      Close();
}

void PicBus::SetDelay()
{
	int val = E2Profile::GetPICSpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 1;
		break;

	case FAST:
		n = 2;
		break;

	case SLOW:
		n = 20;
		break;

	case VERYSLOW:
		n = 150;
		break;

	case ULTRASLOW:
		n = 1000;
		break;

	default:
		n = 6;         //Default (< 100KHz)
		break;
	}

	BusIO::SetDelay(n);

	qDebug() << "PICBus::SetDelay() = " << n;
}

int PicBus::SendDataBit(int b)
{
	setCLK();               //set SCK high
	bitDI(b);

	WaitUsec(shot_delay);

	clearCLK();             //device latch data bit now!

	WaitUsec(shot_delay);

	return OK;
}

// returns a negative number in case of error, 0 or 1 otherwise
int PicBus::RecDataBit()
{
	register uint8_t b;

	setCLK();               //set SCK high (Pic output data now)

	WaitUsec(shot_delay);

	b = getDO();    // sampling data on falling edge
	clearCLK();

	WaitUsec(shot_delay);

	return b;
}

#define CMD2CMD_DELAY   4000

// OK, ora ci alziamo di un livello: operiamo sul byte
int PicBus::SendDataWord(long wo, int wlen)
{
	int k;

	clearCLK();
	clearDI();

	WaitUsec(busI->GetCmd2CmdDelay());

	//transmit lsb first
	for (k = 0; k < wlen; k++)
	{
		SendDataBit(wo & (1 << k));
	}

	setDI();

	//1 usec from a command to the next
	//      WaitUsec(shot_delay/4+1);

	return OK;
}

long PicBus::RecDataWord(int wlen)
{
	int k;
	long val = 0;

	clearCLK();
	clearDI();

	WaitUsec(busI->GetCmd2CmdDelay());
	setDI();
	WaitUsec(2);

	//receive lsb first
	for (k = 0; k < wlen; k++)
		if (RecDataBit())
		{
			val |= 1 << k;
		}

	//      WaitUsec(shot_delay/4+1);

	return val;
}

int PicBus::SendCmdCode(int opcode)
{
	return SendDataWord(opcode, 6);
}

int PicBus::SendProgCode(uint16_t data)
{
	//the code is 14 bit data with leading and trailing 0's
	data &= ProgMask;
	data <<= 1;
	return SendDataWord(data);
}

uint16_t PicBus::RecvProgCode()
{
	return (uint16_t)(RecDataWord() >> 1) & ProgMask;
}

int PicBus::SendDataCode(uint16_t data)
{
	//the code is 8 bit data with leading and trailing 0's
	data &= DataMask;
	data <<= 1;
	return SendDataWord(data);
}

uint16_t PicBus::RecvDataCode()
{
	return (uint16_t)(RecDataWord() >> 1) & DataMask;
}

int PicBus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(10);

	return OK;
}

int PicBus::Reset(void)
{
	SetDelay();

	SetMCLR();              //First bogus entry to charge capacitors
	WaitMsec(150);

	clearDI();
	ClearMCLR();    //Now reset the micro
	setCLK();               //keep Vdd on
	WaitUsec(1000);

	clearCLK();             //Prepare for Program mode entry
	WaitUsec(1000);

	SetMCLR();              //Program mode entry
	WaitMsec(10);

	return OK;
}


void PicBus::DisableCodeProtect()
{
	SendCmdCode(LoadConfigCode);
	SendProgCode(0xffff);

	//go to location 0x2007
	int k;

	for (k = 0; k < 7; k++)
	{
		SendCmdCode(IncAddressCode);
	}

	SendCmdCode(0x01);
	SendCmdCode(0x07);
	SendCmdCode(BeginEraseProgCode);
	WaitMsec(20);
	SendCmdCode(0x01);
	SendCmdCode(0x07);

	WaitMsec(10);

	Reset();
}

long PicBus::ReadConfig(uint16_t *data)
{
	if (data == 0)   //read only configuration word
	{
		return BADPARAM;
	}

	SendCmdCode(LoadConfigCode);
	SendProgCode(0xffff);

	int k;

	for (k = 0; k < 8; k++, data++)
	{
		uint8_t *bp = (uint8_t *)data;
		uint16_t val;

		SendCmdCode(ReadProgCode);

		val = RecvProgCode();

		if (val == ProgMask)
		{
			val = 0xffff;
		}

#ifdef  _BIG_ENDIAN_
		*bp++ = (uint8_t)(val >> 8);
		*bp++ = (uint8_t)(val & 0xFF);
#else
		*bp++ = (uint8_t)(val & 0xFF);
		*bp++ = (uint8_t)(val >> 8);
#endif
		SendCmdCode(IncAddressCode);
	}

	Reset();

	return OK;
}

long PicBus::WriteConfig(uint16_t *data)
{
	if (data == 0)
	{
		return BADPARAM;
	}

	SendCmdCode(LoadConfigCode);
	SendProgCode(data[7]);          //Other PIC programmers do this

	int k;

	for (k = 0; k < 8; k++, data++)
	{
		if (*data != 0xffff)
		{
			uint8_t *bp = (uint8_t *)data;
			uint16_t val;

			//Write Data code
			SendCmdCode(LoadProgCode);

#ifdef  _BIG_ENDIAN_
			val  = (uint16_t)(*bp++) << 8;
			val |= (uint16_t)(*bp++);
#else
			val  = (uint16_t)(*bp++);
			val |= (uint16_t)(*bp++) << 8;
#endif
			SendProgCode(val);
			SendCmdCode(BeginEraseProgCode);

			if (WaitReadyAfterWrite())
			{
				break;
			}

			//Verify while programming (10/11/99)
			SendCmdCode(ReadProgCode);

			if (CompareSingleWord(val, RecvProgCode(), ProgMask))
			{
				return E2ERR_WRITEFAILED;
			}
		}

		SendCmdCode(IncAddressCode);
	}

	WaitMsec(20);
	Reset();

	return OK;
}

int PicBus::Erase(int type)
{
	if (type == ALL_TYPE)
	{
		DisableCodeProtect();
	}

	if (type & PROG_TYPE)
	{
		SendCmdCode(LoadProgCode);
		SendProgCode(0xffff);
		SendCmdCode(EraseProgMem);
		SendCmdCode(BeginEraseProgCode);

		WaitMsec(40);
		Reset();
	}

	if (type & DATA_TYPE)
	{
		SendCmdCode(LoadDataCode);
		SendDataCode(0xffff);
		SendCmdCode(EraseDataMem);
		SendCmdCode(BeginEraseProgCode);

		WaitMsec(40);
		Reset();
	}

	return OK;
}


long PicBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	long len;

	ReadStart();

	//Data is 8 bit, while Program is 14 bit (i.e. 16 bit)
	if (addr == 0)
	{
		length >>= 1;        //contatore da byte a word
	}

	for (len = 0; len < length; len++)
	{
		//
		//Send command opcode
		if (addr)
		{
			//Read DataCode
			SendCmdCode(ReadDataCode);
			*data++ = (uint8_t)RecvDataCode();

			SendCmdCode(IncAddressCode);
		}
		else
		{
			//Read Program Code
			SendCmdCode(ReadProgCode);
			uint16_t val = RecvProgCode();

			if (val == ProgMask)
			{
				val = 0xffff;
			}

#ifdef  _BIG_ENDIAN_
			*data++ = (uint8_t)(val >> 8);
			*data++ = (uint8_t)(val & 0xFF);
#else
			*data++ = (uint8_t)(val & 0xFF);
			*data++ = (uint8_t)(val >> 8);
#endif
			SendCmdCode(IncAddressCode);
		}

		if (ReadProgress(len * 100 / length))
		{
			break;
		}
	}

	ReadEnd();

	if (addr == 0)
	{
		len <<= 1;        //contatore da word a byte
	}

	return len;
}

long PicBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long len;

	WriteStart();

	if (addr == 0)
	{
		length >>= 1;        //contatore da byte a word
	}

	for (len = 0; len < length; len++)
	{
		uint16_t val;

		//Send command opcode
		if (addr)
		{
			//Write Data code
			SendCmdCode(LoadDataCode);
			val  = (uint16_t)(*data++);
			SendDataCode(val);
			SendCmdCode(BeginEraseProgCode);

			if (WaitReadyAfterWrite())
			{
				break;
			}

			//Verify while programming (10/11/99)
			//      SendCmdCode(ReadDataCode);
			//      if ( CompareSingleWord(val, RecvDataCode(), DataMask) )
			//              break;

			SendCmdCode(IncAddressCode);
		}
		else
		{
			//Write Program code
			SendCmdCode(LoadProgCode);

#ifdef  _BIG_ENDIAN_
			val  = (uint16_t)(*data++) << 8;
			val |= (uint16_t)(*data++);
#else
			val  = (uint16_t)(*data++);
			val |= (uint16_t)(*data++) << 8;
#endif
			SendProgCode(val);
			SendCmdCode(BeginEraseProgCode);

			if (WaitReadyAfterWrite())
			{
				break;
			}

			//Verify while programming (10/11/99)
			//      SendCmdCode(ReadProgCode);
			//      if ( CompareSingleWord(val, RecvProgCode(), ProgMask) )
			//              break;

			SendCmdCode(IncAddressCode);
		}

		if (WriteProgress(len * 100 / length))
		{
			break;
		}
	}

	WriteEnd();

	if (addr == 0)
	{
		len <<= 1;        //contatore da word a byte
	}

	return len;
}

int PicBus::CompareSingleWord(uint16_t data1, uint16_t data2, uint16_t mask)
{
	return (data1 & mask) != (data2 & mask);
}

int PicBus::CompareMultiWord(uint8_t *data1, uint8_t *data2, long length, int split)
{
	int retval = 0;

	if (data1 == 0 || data2 == 0 || (length & 1) != 0)
	{
		return BADPARAM;
	}

	if (!split)
	{
		long k;

		for (k = 0; k < length; k += 2)
		{
			uint16_t val1, val2;

#ifdef  _BIG_ENDIAN_
			val1  = (uint16_t)(*data1++) << 8;
			val1 |= (uint16_t)(*data1++);

			val2  = (uint16_t)(*data2++) << 8;
			val2 |= (uint16_t)(*data2++);
#else
			val1  = (uint16_t)(*data1++);
			val1 |= (uint16_t)(*data1++) << 8;

			val2  = (uint16_t)(*data2++);
			val2 |= (uint16_t)(*data2++) << 8;
#endif

			if ((retval = CompareSingleWord(val1, val2, ProgMask)))
			{
				break;        //Stop if a difference
			}
		}
	}
	else
	{
		retval = memcmp(data1, data2, length);
	}

	return retval;
}
