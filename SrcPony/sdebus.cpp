//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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
#include "sdebus.h"
#include "errcode.h"

#include <QDebug>
#include <QtCore>

#include "e2cmdw.h"

Sde2506Bus::Sde2506Bus(BusInterface *ptr)
	: BusIO(ptr)
{
	qDebug() << __PRETTY_FUNCTION__;
}

Sde2506Bus::~Sde2506Bus()
{
	qDebug() << __PRETTY_FUNCTION__;
}

void Sde2506Bus::SetDelay()
{
	int val = E2Profile::GetSDESpeed();
	int n;

	switch (val)
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

	case VERYSLOW:
		n = 100;
		break;

	case ULTRASLOW:
		n = 1000;
		break;

	default:
		n = 10;         //Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << __PRETTY_FUNCTION__ << "=" << n;
}

int Sde2506Bus::SendDataBit(int b)
{
	//clearCLK();             //si assicura che SCK low
	//ShotDelay();
	//setCLK();
	//bitDI(b);
	//ShotDelay();
	//clearCLK();             //device latch data bit now!

	int err = OK;
	busI->xferBit(err, b, SPI_MODE_1 | xMODE_WRONLY);

	return err;
}

int Sde2506Bus::RecDataBit()
{
	//clearCLK();             //the eeprom set data now
	//ShotDelay();
	//setCLK();
	//b = getDO();
	//ShotDelay();   //hold time
	//clearCLK();

	int err = OK;
	int rv = busI->xferBit(err, 1, SPI_MODE_0 | xMODE_RDONLY);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int Sde2506Bus::SendDataWord(int wo, int wlen)
{
	int err = OK;

	clearCLK();
	ShotDelay();
	busI->xferWord(err, wo, SPI_MODE_1 | xMODE_WRONLY, wlen, true);
	setDI();

	return err;
}

int Sde2506Bus::RecDataWord(int wlen)
{
	int err = OK;
	clearCLK();
	int rv = busI->xferWord(err, 0xffff, SPI_MODE_0 | xMODE_RDONLY, wlen, true);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

int Sde2506Bus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(15);

	return OK;
}

int Sde2506Bus::Reset(void)
{
	qDebug() << __PRETTY_FUNCTION__;

	SetDelay();

	clearCLK();			//clock = 0
	setDI();			//data = 1
	setCE();			//CE = 1

	return OK;
}

long Sde2506Bus::Read(int addr, uint8_t *data, long length, int page_size)
{
	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << addr << ", " << data << ", " << (dec) << length << ")";
	ReadStart();

	long len;

	for (len = 0; len < length; len++)
	{
		setCE();

		//Send command opcode
		SendAddress(addr++);
		SendControlBit(0);			//SB = 0 --> Read op
		WaitUsec(5);
		clearCE();
		ShotDelay();

		SendDataBit(1);
		*data++ = RecDataWord();

		ShotDelay(2);
		setCE();

		if ((len % 4) == 0)
		{
			if (ReadProgress(len * 100 / length))
			{
				break;
			}
		}
	}
	WaitMsec(1);		//Flush

	ReadEnd();
	qDebug() << __PRETTY_FUNCTION__ << "=" << len;

	return len;
}

long Sde2506Bus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long curaddr;

	WriteStart();

	for (curaddr = addr; curaddr < length; curaddr++)
	{
		setCE();

		//Send command opcode
		SendDataWord(*data++);
		SendAddress(curaddr);

		SendControlBit(1);			//SB = 1 --> Write/Erase op
		ShotDelay(2);				//WaitUsec(5);
		clearCE();

		SendDataBit(1);				//Start erase
		setDI();
		WaitReadyAfterWrite();
		setCE();					//End erase

		ShotDelay();				//WaitUsec(GetDelay() / 2 + 1);   //perform write
		clearDI();
		ShotDelay();
		clearCE();

		SendDataBit(0);				//Start write
		setDI();
		WaitReadyAfterWrite();
		setCE();					//End write

		if ((curaddr & 1))
		{
			if (WriteProgress(curaddr * 100 / length))
			{
				break;
			}
		}
	}
	WaitMsec(1);		//Flush

	WriteEnd();

	return curaddr;
}

