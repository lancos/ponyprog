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
#include "globals.h"

#include "spi-bus.h"
#include "errcode.h"

#include "e2cmdw.h"

#include <QDebug>
#include <QtCore>

SPIBus::SPIBus(BusInterface *ptr, int bpw, bool cpha, bool cpol)
	: BusIO(ptr),
	  m_bits_per_word(bpw),
	  m_cpol(cpol),
	  m_cpha(cpha)
{
}

SPIBus::~SPIBus()
{
}

void SPIBus::SetDelay()
{
	int val = E2Profile::GetSPISpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 0;         // as fast as your PC can
		break;

	case FAST:
		n = 1;
		break;

	case SLOW:
		n = 10;
		break;

	case VERYSLOW:
		n = 80;
		break;

	case ULTRASLOW:
		n = 1000;
		break;

	default:
		n = 5;         //Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << __PRETTY_FUNCTION__ << "() = " << n;
}


int SPIBus::SendDataBit(int b)
{
	int err = OK;
	busI->SPI_xferBit(err, b, GetMode() | SPIMODE_WRONLY);

	return err;
}

int SPIBus::RecDataBit()
{
	int err = OK;
	int rv = busI->SPI_xferBit(err, 1, GetMode() | SPIMODE_RDONLY);
	if (err == OK)
		return rv;
	else
		return err;
}

int SPIBus::SendDataByte(int by)
{
	int err = OK;
	busI->SPI_xferWord(err, by, GetMode() | SPIMODE_WRONLY);

	return err;
}

int SPIBus::RecDataByte()
{
	int err = OK;
	int rv = busI->SPI_xferWord(err, 0xff, GetMode() | SPIMODE_RDONLY);
	if (err == OK)
		return rv;
	else
		return err;
}

int SPIBus::Reset(void)
{
	qDebug() << __PRETTY_FUNCTION__ << "() I";

	SetDelay();

	clearSCK();             // Dal datasheet AVR
	setMOSI();              // the datasheet don't specify, but we need to provide power from at least one line (MOSI while SCK and RESET are low)

	WaitMsec(20);
	ClearReset();   //One pulse on the reset (datasheet AVR)
	WaitMsec(E2Profile::GetSPIResetPulse());   //AppNote AVR910 suggest >100 msec
	SetReset();

	WaitMsec(E2Profile::GetSPIDelayAfterReset());

	qDebug() << __PRETTY_FUNCTION__ << "() O";

	return OK;
}
