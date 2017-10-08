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


#include "e2profil.h"
#include "microbus.h"
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
MicroWireBus::MicroWireBus(BusInterface *ptr)
	: BusIO(ptr)
{
	qDebug() << "MicroWireBus::MicroWireBus()";
}

// Distruttore
MicroWireBus::~MicroWireBus()
{
	qDebug() << "MicroWireBus::~MicroWireBus()";

	//      Close();
}

void MicroWireBus::SetDelay()
{
	int val = E2Profile::GetMicroWireSpeed();
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
		n = 500;
		break;

	default:
		n = 5;         //Default (< 100KHz)
		break;
	}

	BusIO::SetDelay(n);

	qDebug() << "MicroWire::SetDelay() = " << n;
}


int MicroWireBus::SendDataBit(int b)
{
	clearCLK();             //si assicura che SCK low
	bitDI(b);

	WaitUsec(shot_delay);

	setCLK();               //device latch data bit now!

	WaitUsec(shot_delay);

	clearCLK();

	return OK;
}

// ritorna un numero negativo se c'e` un errore, altrimenti 0 o 1
int MicroWireBus::RecDataBit()
{
	register uint8_t b;

	clearCLK();             //si assicura che SCK low

	WaitUsec(shot_delay);

	setCLK();

	WaitUsec(shot_delay);

	b = getDO();
	clearCLK();

	return b;
}

int MicroWireBus::RecDataBitShort()
{
	clearCLK();             //si assicura che SCK low
	WaitUsec(shot_delay);
	return getDO();
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int MicroWireBus::SendDataWord(int wo, int wlen, int lsb)
{
	int k;

	clearCLK();

	if (lsb)
	{
		//Dal meno significativo al piu` significativo
		for (k = 0; k < wlen; k++)
		{
			SendDataBit(wo & (1 << k));
		}
	}
	else     //Dal piu` significativo al meno significativo
	{
		for (k = wlen - 1; k >= 0; k--)
		{
			SendDataBit(wo & (1 << k));
		}
	}

	clearDI();

	return OK;
}

//Standard Receive data word
int MicroWireBus::RecDataWord(int wlen, int lsb)
{
	int k, val = 0;

	clearCLK();

	if (lsb)
	{
		for (k = 0; k < wlen; k++)
			if (RecDataBit())
			{
				val |= (1 << k);
			}
	}
	else
	{
		for (k = wlen - 1; k >= 0; k--)
			if (RecDataBit())
			{
				val |= (1 << k);
			}
	}

	return val;
}

//Receive Data word with the first clock pulse shortened.
//  In case of the device doesn't leave a clock pulse to switch
//  from DataOut to DataIn after the command
int MicroWireBus::RecDataWordShort(int wlen, int lsb)
{
	int k, val = 0;

	clearCLK();

	if (wlen > 0)
	{
		if (lsb)
		{
			val = RecDataBitShort() ? 1 : 0;

			for (k = 1; k < wlen; k++)
				if (RecDataBit())
				{
					val |= (1 << k);
				}
		}
		else
		{
			val = RecDataBitShort() ? 1 << (wlen - 1) : 0;

			for (k = wlen - 2; k >= 0; k--)
				if (RecDataBit())
				{
					val |= (1 << k);
				}
		}
	}

	return val;
}

int MicroWireBus::WaitReadyAfterWrite(long timeout)
{
	clearCLK();
	ClearReset();   //27/05/98

	WaitUsec(shot_delay);

	SetReset();             //27/05/98

	WaitUsec(shot_delay);

	clearCLK();

	long k;

	for (k = timeout; k > 0  &&  !getDO(); k--)
	{
		WaitUsec(1);        //07/08/99 ** try to fix temporization (so to need only one global calibration)
	}

	qDebug() << "MicroWireBus::WaitReadyAfterWrite() = " << k;

	return k ? OK : E2P_TIMEOUT;
}

int MicroWireBus::Reset(void)
{
	qDebug() << "MicroWireBus::Reset()";

	SetDelay();

	clearCLK();
	//clearDI();    //27/05/98
	setDI();

	ClearReset();   //Da un impulso sul reset
	WaitMsec(1);    //27/05/98
	SetReset();

	WaitMsec(50);

	return OK;
}


int MicroWireBus::CalcAddressSize(int mem_size, int org) const
{
	if (mem_size-- <= 0)
	{
		return -1;
	}

	int k;

	for (k = 15; k > 0; k--)
		if (mem_size & (1 << k))
		{
			break;
		}

	return k + 1;
}

