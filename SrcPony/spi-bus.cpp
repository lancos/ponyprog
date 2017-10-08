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
#include "globals.h"

#include "spi-bus.h"
#include "errcode.h"


#include "e2cmdw.h"

#include <QDebug>


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

// Costruttore
SPIBus::SPIBus(BusInterface *ptr, bool cpha)
	: BusIO(ptr),
	  fall_edge_sample(cpha)
{
}

// Distruttore
SPIBus::~SPIBus()
{
	//      Close();
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

	BusIO::SetDelay(n);

	qDebug() << "SPIBus::SetDelay() = " << n;
}


int SPIBus::SendDataBit(int b)
{
	if (fall_edge_sample)
	{
		setSCK();               //be sure the SCK line is high
		bitMOSI(b);
		WaitUsec(shot_delay);
		clearSCK();             //slave latches data bit now!
		WaitUsec(shot_delay);
	}
	else
	{
		clearSCK();             //be sure the SCK line is low
		bitMOSI(b);
		WaitUsec(shot_delay);
		setSCK();               //slave latches data bit now!
		WaitUsec(shot_delay);
		clearSCK();
	}

	return OK;
}

// ritorna un numero negativo se c'e` un errore, altrimenti 0 o 1
int SPIBus::RecDataBit()
{
	register uint8_t b;

	if (fall_edge_sample)
	{
		setSCK();               //be sure the SCK line is high
		WaitUsec(shot_delay);
		clearSCK();
		b = getMISO();
		WaitUsec(shot_delay);
	}
	else
	{
		clearSCK();             //be sure the SCK line is low
		WaitUsec(shot_delay);
		setSCK();
		b = getMISO();
		WaitUsec(shot_delay);
		clearSCK();
	}

	return b;
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int SPIBus::SendDataByte(int by)
{
	int k;

	clearSCK();

	//MSbit (7) sent first
	for (k = 7; k >= 0; k--)
	{
		SendDataBit(by & (1 << k));
	}

	setMOSI();

	return OK;
}

int SPIBus::RecDataByte()
{
	int k, val = 0;

	setMOSI();
	clearSCK();

	for (k = 7; k >= 0; k--)
		if (RecDataBit())
		{
			val |= 1 << k;
		}

	return val;
}


int SPIBus::Reset(void)
{
	qDebug() << "SPIBus::Reset() I";

	SetDelay();

	clearSCK();             // Dal datasheet AVR
	setMOSI();              // the datasheet don't specify, but we need to provide power from at least one line (MOSI while SCK and RESET are low)

	WaitMsec(20);
	ClearReset();   //One pulse on the reset (datasheet AVR)
	WaitMsec(E2Profile::GetSPIResetPulse());   //AppNote AVR910 suggest >100 msec
	SetReset();

	WaitMsec(E2Profile::GetSPIDelayAfterReset());

	qDebug() << "SPIBus::Reset() O";

	return OK;
}
