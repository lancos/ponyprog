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
#include "picbus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

#include "globals.h"
#include "e2profil.h"
#include "e2cmdw.h"

class e2CmdWindow;

PicBus::PicBus(BusInterface *ptr)
	: BusIO(ptr),
	  DataMask(0xff),
	  ProgMask(0x3fff),
	  ReadProgCode(0x04),			//Read Data from Program Memory
	  LoadProgCode(0x02),			//Load Data for Program Memory
	  ReadDataCode(0x05),			//Read Data from Data Memory
	  LoadDataCode(0x03),			//Load Data for Data Memory
	  LoadConfigCode(0x00),			//Load Configuration
	  IncAddressCode(0x06),			//Increment Address
	  EraseProgMem(0x09),			//Bulk Erase Program Memory
	  EraseDataMem(0x0b),			//Bulk Erase Data Memory
	  BeginEraseProgCode(0x08),		//Begin Erase Programming Cycle
	  BeginProgOnlyCode(0x18)		//Begin Programming Only Cycle
{
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
		n = 6;			//Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << Q_FUNC_INFO << "=" << n;
}

int PicBus::SendDataBit(int b)
{
	//setCLK();				//set SCK high
	//bitDI(b);
	//ShotDelay();
	//clearCLK();				//device latch data bit now!
	//ShotDelay();

	int err = OK;
	clearCLK();
	busI->xferBit(err, b, SPI_MODE_1 | xMODE_WRONLY);

	return err;
}

// returns a negative number in case of error, 0 or 1 otherwise
int PicBus::RecDataBit()
{
	//setCLK();				//set SCK high (Pic output data now)
	//ShotDelay();
	//b = getDO();			// sampling data on falling edge
	//clearCLK();
	//ShotDelay();

	int err = OK;
	clearCLK();
	int rv = busI->xferBit(err, 1, SPI_MODE_1 | xMODE_RDONLY);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int PicBus::SendDataWord(long wo, int wlen)
{
	int err = OK;

	clearCLK();
	clearDI();
	WaitUsec(busI->GetCmd2CmdDelay());
	busI->xferWord(err, wo, SPI_MODE_1 | xMODE_WRONLY, wlen, true);
	setDI();

	//1 usec from a command to the next
	//WaitUsec(shot_delay/4+1);

	return err;
}

long PicBus::RecDataWord(int wlen)
{
	int err = OK;

	clearCLK();
	clearDI();
	WaitUsec(busI->GetCmd2CmdDelay());
	setDI();
	ShotDelay();
	int rv = busI->xferWord(err, 0xffff, SPI_MODE_1 | xMODE_RDONLY, wlen, true);

	//WaitUsec(shot_delay/4+1);

	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int PicBus::SendCmdCode(int opcode)
{
	return SendDataWord(opcode, 6);
}

int PicBus::SendProgCode(quint16 data)
{
	//the code is 14 bit data with leading and trailing 0's
	data &= ProgMask;
	data <<= 1;
	return SendDataWord(data);
}

quint16 PicBus::RecvProgCode()
{
	return (quint16)(RecDataWord() >> 1) & ProgMask;
}

int PicBus::SendDataCode(quint16 data)
{
	//the code is 8 bit data with leading and trailing 0's
	data &= DataMask;
	data <<= 1;
	return SendDataWord(data);
}

quint16 PicBus::RecvDataCode()
{
	return (quint16)(RecDataWord() >> 1) & DataMask;
}

int PicBus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(10);

	return OK;
}

int PicBus::Reset(void)
{
	SetDelay();

	SetMCLR();				//First bogus entry to charge capacitors
	WaitMsec(150);

	clearDI();
	ClearMCLR();			//Now reset the micro
	setCLK();				//keep Vdd on
	WaitUsec(1000);

	clearCLK();				//Prepare for Program mode entry
	WaitUsec(1000);

	SetMCLR();				//Program mode entry
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

long PicBus::ReadConfig(quint16 *data)
{
	Q_CHECK_PTR(data);

	if (data == 0)   //read only configuration word
	{
		return BADPARAM;
	}

	SendCmdCode(LoadConfigCode);
	SendProgCode(0xffff);

	int k;

	for (k = 0; k < 8; k++, data++)
	{
		quint8 *bp = (quint8 *)data;
		quint16 val;

		SendCmdCode(ReadProgCode);

		val = RecvProgCode();

		if (val == ProgMask)
		{
			val = 0xffff;
		}

#ifdef  _BIG_ENDIAN_
		*bp++ = (quint8)(val >> 8);
		*bp++ = (quint8)(val & 0xFF);
#else
		*bp++ = (quint8)(val & 0xFF);
		*bp++ = (quint8)(val >> 8);
#endif
		SendCmdCode(IncAddressCode);
	}

	Reset();

	return OK;
}

long PicBus::WriteConfig(quint16 *data)
{
	Q_CHECK_PTR(data);

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
			quint8 *bp = (quint8 *)data;
			quint16 val;

			//Write Data code
			SendCmdCode(LoadProgCode);

#ifdef  _BIG_ENDIAN_
			val  = (quint16)(*bp++) << 8;
			val |= (quint16)(*bp++);
#else
			val  = (quint16)(*bp++);
			val |= (quint16)(*bp++) << 8;
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


long PicBus::Read(int addr, quint8 *data, long length, int page_size)
{
	long len;

	ReadStart();

	//Data is 8 bit, while Program is 14 bit (i.e. 16 bit)
	if (addr == 0)
	{
		length >>= 1;
	}

	for (len = 0; len < length; len++)
	{
		//Send command opcode
		if (addr)
		{
			//Read DataCode
			SendCmdCode(ReadDataCode);
			*data++ = (quint8)RecvDataCode();

			SendCmdCode(IncAddressCode);
		}
		else
		{
			//Read Program Code
			SendCmdCode(ReadProgCode);
			quint16 val = RecvProgCode();

			if (val == ProgMask)
			{
				val = 0xffff;
			}

#ifdef  _BIG_ENDIAN_
			*data++ = (quint8)(val >> 8);
			*data++ = (quint8)(val & 0xFF);
#else
			*data++ = (quint8)(val & 0xFF);
			*data++ = (quint8)(val >> 8);
#endif
			SendCmdCode(IncAddressCode);
		}

		if (ReadProgress(len * 100 / length))
		{
			break;
		}
	}

	WaitMsec(1);		//Flush
	ReadEnd();

	if (addr == 0)
	{
		len <<= 1;
	}

	return len;
}

long PicBus::Write(int addr, quint8 const *data, long length, int page_size)
{
	long len;

	WriteStart();

	if (addr == 0)
	{
		length >>= 1;
	}

	for (len = 0; len < length; len++)
	{
		quint16 val;

		//Send command opcode
		if (addr)
		{
			//Write Data code
			SendCmdCode(LoadDataCode);
			val  = (quint16)(*data++);
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
			val  = (quint16)(*data++) << 8;
			val |= (quint16)(*data++);
#else
			val  = (quint16)(*data++);
			val |= (quint16)(*data++) << 8;
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

	WaitMsec(1);			//Flush
	WriteEnd();

	if (addr == 0)
	{
		len <<= 1;
	}

	return len;
}

int PicBus::CompareSingleWord(quint16 data1, quint16 data2, quint16 mask)
{
	return (data1 & mask) != (data2 & mask);
}

int PicBus::CompareMultiWord(quint8 *data1, quint8 *data2, long length, int split)
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
			quint16 val1, val2;

#ifdef  _BIG_ENDIAN_
			val1  = (quint16)(*data1++) << 8;
			val1 |= (quint16)(*data1++);

			val2  = (quint16)(*data2++) << 8;
			val2 |= (quint16)(*data2++);
#else
			val1  = (quint16)(*data1++);
			val1 |= (quint16)(*data1++) << 8;

			val2  = (quint16)(*data2++);
			val2 |= (quint16)(*data2++) << 8;
#endif

			if ((retval = CompareSingleWord(val1, val2, ProgMask)))
			{
				break;		//Stop if a difference is detected
			}
		}
	}
	else
	{
		retval = memcmp(data1, data2, length);
	}

	return retval;
}
