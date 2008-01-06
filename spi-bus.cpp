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
#include "spi-bus.h"
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

// Costruttore
SPIBus::SPIBus(BusInterface *ptr)
	: BusIO(ptr)
{
}

// Distruttore
SPIBus::~SPIBus()
{
//	Close();
}

void SPIBus::SetDelay()
{
	int val = THEAPP->GetSPISpeed();
	int n;

	switch(val)
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

	UserDebug1(UserApp2, "SPIBus::SetDelay() = %d\n", n);
}


int SPIBus::SendDataBit(int b)
{
	clearSCK();		//si assicura che SCK low
	bitMOSI(b);
	WaitUsec(shot_delay);
	setSCK();		//AT90s latch data bit now!
	WaitUsec(shot_delay);
	clearSCK();

	return OK;
}

// ritorna un numero negativo se c'e` un errore, altrimenti 0 o 1
int SPIBus::RecDataBit()
{
	register UBYTE b;

	clearSCK();		//si assicura SCK low
	WaitUsec(shot_delay);
	setSCK();
	WaitUsec(shot_delay);
	b = getMISO();
	clearSCK();

	return b;
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int SPIBus::SendDataByte(int by)
{
	int k;

	clearSCK();
	//Dal piu` significativo al meno significativo (7 to 0)
	for (k = 7; k >= 0; k--)
		SendDataBit(by & (1<<k));
	setMOSI();

	return OK;
}

int SPIBus::RecDataByte()
{
	int k, val = 0;

	clearSCK();
	for (k = 7; k >= 0; k--)
		if ( RecDataBit() )
			val |= 1 << k;
	setMOSI();

	return val;
}


int SPIBus::Reset(void)
{
	UserDebug(UserApp2, "SPIBus::Reset() I\n");

	SetDelay();

	clearSCK();		// Dal datasheet AVR
	setMOSI();		// the datasheet don't specify, but we need to provide power from at least one line (MOSI while SCK and RESET are low)

	WaitMsec(20);
	ClearReset();	//One pulse on the reset (datasheet AVR)
	WaitMsec( THEAPP->GetSPIResetPulse() );	//AppNote AVR910 suggest >100 msec
	SetReset();

	WaitMsec( THEAPP->GetSPIDelayAfterReset() );

	UserDebug(UserApp2, "SPIBus::Reset() O\n");

	return OK;
}
