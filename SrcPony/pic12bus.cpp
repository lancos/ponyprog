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
#include "pic12bus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

#include "e2cmdw.h"

#define MAX_PROG_PULSES 8

// Constructor
Pic12Bus::Pic12Bus(BusInterface *ptr)
	: BusIO(ptr),
	  //              DataMask(0xff),
	  ProgMask(0x0fff),               //12bit instruction

	  ReadProgCode(0x04),                     //Read Data from Program Memory *
	  LoadProgCode(0x02),                     //Load Data for Program Memory *

	  //              ReadDataCode(0x05),                     //Read Data from Data Memory *
	  //              LoadDataCode(0x03),                     //Load Data for Data Memory *
	  //              LoadConfigCode(0x00),           //Load Configuration *
	  IncAddressCode(0x06),           //Increment Address *
	  //              EraseProgMem(0x09),                     //Bulk Erase Program Memory *
	  //              EraseDataMem(0x0b),                     //Bulk Erase Data Memory *
	  BeginProgCode(0x08),    //Begin Erase Programming Cycle *
	  //              BeginProgOnlyCode(0x18)         //Begin Programming Only Cycle
	  EndProgCode(0x0e)
{
	qDebug() << Q_FUNC_INFO << "()";

	OverProgrammingMult = 11;               //Default OverProgramming X value (x11)
	OverProgrammingAdd = 0;                 //Default OverProgramming + value (+0)
}

// Desctructor
Pic12Bus::~Pic12Bus()
{
	//      Close();
}

void Pic12Bus::SetDelay()
{
	int val = E2Profile::GetPICSpeed();
	int n;

	switch (val)
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

	busI->SetDelay(n);

	qDebug() << Q_FUNC_INFO << "() = " << n;
}

int Pic12Bus::SendDataBit(int b)
{
	//setCLK();               //set SCK high
	//bitDI(b);
	//ShotDelay();
	//clearCLK();             //device latch data bit now!
	//ShotDelay();

	int err = OK;
	clearCLK();
	busI->xferBit(err, b, SPI_MODE_1 | xMODE_WRONLY);

	return OK;
}

// returns a negative number in case of error, 0 or 1 otherwise
int Pic12Bus::RecDataBit()
{
	//setCLK();               //set SCK high (Pic output data now)
	//ShotDelay();
	//b = getDO();    // sampling data on falling edge
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

int Pic12Bus::SendDataWord(long wo, int wlen)
{
	int err = OK;

	clearCLK();
	clearDI();

	//transmit lsb first
	//for (int k = 0; k < wlen; k++)
	//	SendDataBit(wo & (1 << k));
	busI->xferWord(err, wo, SPI_MODE_1 | xMODE_WRONLY, wlen, true);

	setDI();

	//1 usec from a command to the next
	//WaitUsec(GetDelay() / 4 + 1);
	ShotDelay(2);

	return OK;
}

long Pic12Bus::RecDataWord(int wlen)
{
	int err = OK;

	clearCLK();
	setDI();

	//receive lsb first
	//long val = 0;
	//for (int k = 0; k < wlen; k++)
	//	if (RecDataBit())
	//		val |= 1 << k;
	ShotDelay();
	int rv = busI->xferWord(err, 0xffff, SPI_MODE_1 | xMODE_RDONLY, wlen, true);

	//WaitUsec(GetDelay() / 4 + 1);
	ShotDelay(2);

	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int Pic12Bus::Reset(void)
{
	qDebug() << Q_FUNC_INFO << " IN";

	SetDelay();

	SetMCLR();              //First bogus entry to charge capacitors
	WaitMsec(200);          //150

	clearDI();
	ClearMCLR();            //Now reset the micro
	setCLK();               //keep Vdd on
	WaitMsec(20);

	clearCLK();             //Prepare for Program mode entry
	WaitMsec(10);

	SetMCLR();              //Program mode entry
	WaitMsec(10);

	current_address = -1;

	qDebug() << Q_FUNC_INFO << " OUT";

	return OK;
}

long Pic12Bus::ReadConfig(quint16 &data)
{
	//      Reset();

	quint8 *bp = (quint8 *)&data;

	//Read Program Code
	SendCmdCode(ReadProgCode);
	quint16 val = RecvProgCode();

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
	IncAddress(1);

	qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << val << ") OUT";

	return OK;
}


long Pic12Bus::WriteConfig(quint16 data)
{
	qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << data  << ") IN";

	//      Reset();

	quint8 *bp = (quint8 *)&data;
	quint16 val;

	//Write Program code
#ifdef  _BIG_ENDIAN_
	val  = (quint16)(*bp++) << 8;
	val |= (quint16)(*bp++);
#else
	val  = (quint16)(*bp++);
	val |= (quint16)(*bp++) << 8;
#endif
	int k;

	for (k = 100; k > 0; k--)
	{
		ProgramPulse(val, 0);
	}

	IncAddress(1);

	return OK;
}

long Pic12Bus::BlankCheck(long length)
{
	length >>= 1;   //contatore da byte a word

	ReadStart();
	//Point to first location
	//      SendCmdCode(IncAddressCode);

	long len;

	for (len = 0; len < length - 1; len++)   //Skip last location (RC calibration)
	{
		//Read Program Code
		SendCmdCode(ReadProgCode);

		if (CompareSingleWord(0xffff, RecvProgCode(), ProgMask))
		{
			break;
		}

		if (ReadProgress(len * 100 / length))
		{
			break;
		}

		IncAddress(1);
	}

	ReadEnd();

	return (len == length);
}

long Pic12Bus::Read(int addr, quint8 *data, long length, int page_size)
{
	long len;

	qDebug() << Q_FUNC_INFO << "(" << addr << ", " << (Qt::hex) << data << ", " << (Qt::dec) <<  length << ") IN";

	ReadStart();
	length >>= 1;   //contatore da byte a word

	//Point to first location
	//      SendCmdCode(IncAddressCode);

	for (len = 0; len < length; len++)
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
		IncAddress(1);

		if (ReadProgress(len * 100 / length))
		{
			break;
		}
	}

	ReadEnd();

	len <<= 1;      //contatore da word a byte

	qDebug() << Q_FUNC_INFO << "() = " << len << " OUT";

	return len;
}

long Pic12Bus::Write(int addr, quint8 const *data, long length, int page_size)
{
	long len;

	qDebug() << Q_FUNC_INFO << "(" << addr << ", " << (Qt::hex) << data << ", " << (Qt::dec) << length << ") IN";

	WriteStart();
	length >>= 1;   //contatore da byte a word

	//The address pointer should already point to first address
	//location (via a ConfigRead or IncAddress)

	//Program cycle
	for (len = 0; len < length; len++)
	{
		quint16 val;

		//Write Program code
#ifdef  _BIG_ENDIAN_
		val  = (quint16)(*data++) << 8;
		val |= (quint16)(*data++);
#else
		val  = (quint16)(*data++);
		val |= (quint16)(*data++) << 8;
#endif
		int rv = WriteProgWord(val, length - 1);

		if (rv != OK)
		{
			len = rv;
			break;
		}

		if (WriteProgress(len * 100 / length))
		{
			break;
		}
	}

	WriteEnd();

	if (len > 0)
	{
		len <<= 1;        //contatore da word a byte
	}

	qDebug() << Q_FUNC_INFO << "() = " << len << " ** " <<  GetLastProgrammedAddress() << " OUT";

	return len;
}

int Pic12Bus::WriteProgWord(quint16 val, long rc_addr)
{
	int k;
	int rval = OK;

	qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << val << ", " << (Qt::dec) <<  current_address << ") IN";

	//Check for RC calibration location
	if (current_address == rc_addr)
	{
		//Check for blank (erased cells)
		//Programma la RC calibration solamente se la locazione e` cancellata
		// e il valore da pgrogrammare corrisponde ad una MOVLW xx (0x0Cxx)
		SendCmdCode(ReadProgCode);

		if (CompareSingleWord(RecvProgCode(), 0xffff, ProgMask) == 0 &&
				CompareSingleWord(val, 0x0C00, (ProgMask & 0xff00)) == 0)
		{
			SetLastProgrammedAddress(current_address << 1);

			for (k = 1; k <= MAX_PROG_PULSES; k++)
			{
				if (ProgramPulse(val, 1) == OK)
				{
					break;
				}
			}

			if (k > MAX_PROG_PULSES)
			{
				rval = E2ERR_WRITEFAILED;       //Write error
			}
			else
			{
				qDebug() << "Pic12Bus::WriteProgWord(): Npulses = " << k;

				k *= OverProgrammingMult;
				k += OverProgrammingAdd;

				while (k--)
				{
					ProgramPulse(val, 0);        //Program pulse without test
				}
			}
		}

		IncAddress(1);
	}
	else
	{
		//Check for blank (erased cells)
		SendCmdCode(ReadProgCode);

		if (CompareSingleWord(RecvProgCode(), 0xffff, ProgMask))
		{
			rval = E2ERR_BLANKCHECKFAILED;
		}
		else
		{
			//Skip FFF words
			if (CompareSingleWord(val, 0xffff, ProgMask) != 0)
			{
				SetLastProgrammedAddress(current_address << 1);

				for (k = 1; k <= MAX_PROG_PULSES; k++)
				{
					if (ProgramPulse(val, 1) == OK)
					{
						break;
					}
				}

				if (k > MAX_PROG_PULSES)
				{
					rval = E2ERR_WRITEFAILED;       //Write error
				}
				else
				{
					qDebug() << "Pic12Bus::WriteProgWord(): Npulses = " << k;

					k *= OverProgrammingMult;
					k += OverProgrammingAdd;

					while (k--)
					{
						ProgramPulse(val, 0);        //Program pulse without test
					}
				}
			}

			IncAddress(1);
		}
	}

	qDebug() << Q_FUNC_INFO << "() = " << rval << " OUT";

	return rval;
}

void Pic12Bus::IncAddress(int n)
{
	qDebug() << Q_FUNC_INFO << "(" << n << ") IN";

	while (n--)
	{
		SendCmdCode(IncAddressCode);
		current_address++;
	}

	qDebug() << Q_FUNC_INFO << "() OUT ** cur_addr = " << current_address;
}

int Pic12Bus::ProgramPulse(quint16 val, int verify, int width)
{
	int rval = OK;

	qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << val << ", " << (Qt::dec) <<  verify << ", " << width << ") IN";

	SendCmdCode(LoadProgCode);
	SendProgCode(val);

	SendCmdCode(BeginProgCode);     //Start programming pulse
	WaitUsec(width);
	SendCmdCode(EndProgCode);       //Stop programming pulse

	WaitUsec(1000);         //wait between pulses

	//Verify programmed location
	if (verify)
	{
		SendCmdCode(ReadProgCode);
		rval = CompareSingleWord(val, RecvProgCode(), ProgMask);
	}

	qDebug() << Q_FUNC_INFO << "() = " << rval << " OUT";

	return rval;
}

int Pic12Bus::CompareSingleWord(quint16 data1, quint16 data2, quint16 mask)
{
	return (data1 & mask) != (data2 & mask);
}

int Pic12Bus::CompareMultiWord(quint8 *data1, quint8 *data2, long length, int split)
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
