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
#include "imbus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

#include "e2cmdw.h"

#ifdef Q_OS_LINUX
#  include <unistd.h>
#else
#  ifdef        __BORLANDC__
#    define     __inline__
#  else // _MICROSOFT_ VC++
#    define     __inline__ __inline
#    define _export
#  endif
#endif

#define IMADDR_ADDR(x)          ( (x) ? 132 : 128 )
#define IMADDR_READ(x)          ( (x) ? 133 : 129 )
#define IMADDR_WRITE(x)         ( (x) ? 135 : 131 )

// Costruttore
IMBus::IMBus(BusInterface *ptr)
	: BusIO(ptr)
{
	qDebug() << Q_FUNC_INFO;

	StatusLocation = 526;   //526 for NVM3060, 14 for MDA2062
	SecondaryAddress = false;
	Data16_mode = true;
	ProgDelay = 30;
}

// Distruttore
IMBus::~IMBus()
{
	qDebug() << Q_FUNC_INFO;
}

void IMBus::SetStatusLocation(int val)
{
	if (val >= 0)
	{
		StatusLocation = val;
	}
}

void IMBus::UseSecondaryAddress(bool val)
{
	SecondaryAddress = val;
}

void IMBus::SetDataMode(int val)
{
	if (val == 16)
	{
		Data16_mode = true;
	}
	else if (val == 8)
	{
		Data16_mode = false;
	}
}

void IMBus::SetProgDelay(int val)
{
	ProgDelay = val;
}

void IMBus::SetDelay()
{
	int val = E2Profile::GetIMBusSpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 1;         // as fast as your PC can
		break;

	case FAST:
		n = 4;
		break;

	case SLOW:
		n = 30;
		break;

	case VERYSLOW:
		n = 100;
		break;

	case ULTRASLOW:
		n = 500;
		break;

	default:
		n = 10;         //Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << Q_FUNC_INFO << "=" << n;
}

int IMBus::SendDataBit(int b)
{
	//clearCLK();             //set clock low
	//bitDI(b);
	//ShotDelay();
	//setCLK();               //device latch data bit now!
	//ShotDelay();

	int err = OK;
	setCLK();
	busI->xferBit(err, b, SPI_MODE_3 | xMODE_WRONLY);

	return err;
}

// return negative number in case of error, otherwise received bit (0 or 1)
int IMBus::RecDataBit()
{
	//clearCLK();                             //the eeprom set data now
	//ShotDelay();
	//setCLK();
	//b = getDO();
	//ShotDelay();

	int err = OK;
	setCLK();
	int rv = busI->xferBit(err, 1, SPI_MODE_3 | xMODE_RDONLY);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int IMBus::IdentPulse()
{
	clearIdent();
	ShotDelay(6);
	setIdent();
	ShotDelay(2);

	return OK;
}

//byte operations
int IMBus::SendAddrWord(int wo)
{
	int err = OK;

	setCLK();
	clearIdent();

	//From LSB to MSB
	//for (int k = 0; k < 8; k++)
	//	SendDataBit(wo & (1 << k));
	busI->xferByte(err, wo, SPI_MODE_3 | xMODE_WRONLY, 8, true);

	setCLK();
	setDI();
	setIdent();
	ShotDelay(2);	//1.5 usec

	return err;
}

int IMBus::SendDataWord(long wo, int wlen)
{
	int err = OK;

	setCLK();
	setIdent();

	//From LSB to MSB
	//for (int k = 0; k < wlen; k++)
	//	SendDataBit(wo & (1 << k));
	busI->xferWord(err, wo, SPI_MODE_3 | xMODE_WRONLY, wlen, true);

	setCLK();
	setDI();

	return err;
}

int IMBus::RecDataWord(int wlen)
{
	int err = OK;

	setCLK();
	setDI();
	setIdent();

	//int val = 0;
	//for (int k = 0; k < wlen; k++)
	//	if (RecDataBit())
	//		val |= 1 << k;
	int rv = busI->xferWord(err, 0xffff, SPI_MODE_3 | xMODE_RDONLY, wlen, true);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int IMBus::WaitReadyAfterWrite(int addr, int delay, long timeout)
{
	int rval = OK;

	if (delay > 0)
	{
		WaitMsec(delay);
	}
	else
	{
		uint8_t val = 0xff;

		while ((val & (1 << 1)) && timeout--)
		{
			//Read status location at address 526

			//Send address code
			SendAddrWord(IMADDR_ADDR(SecondaryAddress));
			SendDataWord(StatusLocation, Data16_mode ? 16 : 8);
			IdentPulse();

			//Receive data
			SendAddrWord(IMADDR_READ(SecondaryAddress));
			val = (uint8_t)RecDataWord(Data16_mode ? 16 : 8);
			IdentPulse();
		}

		if (timeout == 0)
		{
			rval = E2P_TIMEOUT;
		}
	}

	return rval;
}

int IMBus::Reset(void)
{
	qDebug() << "IMBus::Reset()";

	SetDelay();

	setCLK();               //clock = 1
	setDI();                //data = 1
	setIdent();             //Ident = 1

	WaitMsec(50);

	return OK;
}

long IMBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << "IMBus::Read(" << (hex) << addr << ", " << data << ", " << (dec) <<  length << ")";

	ReadStart();
	long len;

	for (len = 0; len < length; len++, addr++)
	{
		//Send address code
		SendAddrWord(IMADDR_ADDR(SecondaryAddress));
		SendDataWord(addr, Data16_mode ? 16 : 8);
		IdentPulse();

		//Receive data
		SendAddrWord(IMADDR_READ(SecondaryAddress));
		*data++ = (uint8_t)RecDataWord(Data16_mode ? 16 : 8);
		IdentPulse();

		if ((len % 4) == 0)
		{
			if (ReadProgress(len * 100 / length))
			{
				break;
			}
		}
	}

	ReadEnd();

	qDebug() << "IMBus::Read() = " << len;

	return len;
}

long IMBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long len;
	uint8_t bval;
	int loop_timeout;

	WriteStart();

	for (len = 0; len < length; len++, addr++, data++)
	{
		bval = ~ *data;         //be sure are different to write at least once

		for (loop_timeout = 10; bval != *data && loop_timeout > 0; loop_timeout--)
		{
			//Erase command first
			//Send address code
			SendAddrWord(IMADDR_ADDR(SecondaryAddress));
			SendDataWord(addr, Data16_mode ? 16 : 8);
			IdentPulse();

			//Erase location
			SendAddrWord(IMADDR_WRITE(SecondaryAddress));
			SendDataWord(0xffff, Data16_mode ? 16 : 8);
			IdentPulse();

			if (WaitReadyAfterWrite(addr, ProgDelay) != OK)
			{
				return E2P_TIMEOUT;
			}

			//Write command
			if (ProgDelay <= 0)             //Ripristina l'indirizzo corretto (solo se ` attivo il polling)
			{
				//Send address code
				SendAddrWord(IMADDR_ADDR(SecondaryAddress));
				SendDataWord(addr, Data16_mode ? 16 : 8);
				IdentPulse();
			}

			//Program location
			SendAddrWord(IMADDR_WRITE(SecondaryAddress));
			SendDataWord(*data, Data16_mode ? 16 : 8);
			IdentPulse();

			if (WaitReadyAfterWrite(addr, ProgDelay) != OK)
			{
				return E2P_TIMEOUT;
			}

			//Verify while write
			//Send address code
			SendAddrWord(IMADDR_ADDR(SecondaryAddress));
			SendDataWord(addr, Data16_mode ? 16 : 8);
			IdentPulse();

			//Receive data
			SendAddrWord(IMADDR_READ(SecondaryAddress));
			bval = (uint8_t)RecDataWord(Data16_mode ? 16 : 8);
			IdentPulse();
		} //for

		if (loop_timeout == 0)
		{
			return E2ERR_WRITEFAILED;
		}

		if (WriteProgress(len * 100 / length))
		{
			break;
		}
	}

	WriteEnd();

	return len;
}
