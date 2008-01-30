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
#include "sdebus.h"
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
Sde2506Bus::Sde2506Bus(BusInterface *ptr)
	: BusIO(ptr)
{
	UserDebug(Constructor, "Sde2506Bus::Sde2506Bus() constructor\n");
}

// Distruttore
Sde2506Bus::~Sde2506Bus()
{
	UserDebug(Destructor, "Sde2506Bus::~Sde2506Bus() destructor\n");

//	Close();
}

void Sde2506Bus::SetDelay()
{
    int val = THEAPP->GetSDESpeed();
    int n;

    switch(val)
    {
    case TURBO:
        n = 1;         // as fast as your PC can
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
    BusIO::SetDelay(n);

    UserDebug1(UserApp2, "Sde2506Bus::SetDelay() = %d\n", n);
}

int Sde2506Bus::SendDataBit(int b)
{
	clearCLK();		//si assicura che SCK low
	WaitUsec(shot_delay);
	setCLK();
	bitDI(b);
	WaitUsec(shot_delay);
	clearCLK();		//device latch data bit now!

	return OK;
}

// ritorna un numero negativo se c'e` un errore, altrimenti 0 o 1
int Sde2506Bus::RecDataBit()
{
	register UBYTE b;

	clearCLK();		//the eeprom set data now
	WaitUsec(shot_delay);
	setCLK();
	b = getDO();
	WaitUsec(shot_delay);	//hold time
	clearCLK();

	return b;
}

//* OK, ora ci alziamo di un livello: operiamo sul byte
int Sde2506Bus::SendDataWord(int wo, int wlen)
{
	int k;

	clearCLK();
	//From LSB to MSB
	for (k = 0; k < wlen; k++)
		SendDataBit(wo & (1<<k));
	WaitUsec(shot_delay);
	setDI();

	return OK;
}

int Sde2506Bus::RecDataWord(int wlen)
{
	int k, val = 0;

	clearCLK();
	setDI();
	for (k = 0; k < wlen; k++)
		if ( RecDataBit() )
			val |= 1 << k;

	return val;
}

int Sde2506Bus::WaitReadyAfterWrite(long timeout)
{
	WaitMsec(15);

	return OK;
}

int Sde2506Bus::Reset(void)
{
	UserDebug(UserApp2, "Sde2506Bus::Reset()\n");

	SetDelay();

	clearCLK();		//clock = 0
	setDI();		//data = 1
	setCE();		//CE = 1

	return OK;
}

long Sde2506Bus::Read(int addr, UBYTE *data, long length, int page_size)
{
	UserDebug3(UserApp2, "Sde2506Bus::Read(%Xh, %Xh, %ld)\n", addr, (unsigned int)data, length);

	long len;

	for (len = 0; len < length; len++)
	{
		setCE();

		//Send command opcode
		SendAddress(addr++);
		SendControlBit(0);		//SB = 0 --> Read op
		clearCE();

		SendDataBit(1);
		*data++ = RecDataWord();

		WaitUsec(shot_delay+1);
		setCE();

		if ( (len % 4) == 0 )
			if ( CheckAbort(len * 100 / length) )
				break;
	}
	CheckAbort(100);

	UserDebug1(UserApp2, "Sde2506Bus::Read() = %ld\n", len);

	return len;
}

long Sde2506Bus::Write(int addr, UBYTE const *data, long length, int page_size)
{
	long curaddr;

	for (curaddr = addr; curaddr < length; curaddr++)
	{
		setCE();

		//Send command opcode
		SendDataWord(*data++);
		SendAddress(curaddr);

		SendControlBit(1);			//SB = 1 --> Write/Erase op
		clearCE();

		SendDataBit(1);				//Start erase
		setDI();
		WaitReadyAfterWrite();
		setCE();					//End erase

		WaitUsec(shot_delay/2+1);	//perform write
		clearDI();
		WaitUsec(shot_delay);
		clearCE();

		SendDataBit(0);			//Start write
		setDI();
		WaitReadyAfterWrite();
		setCE();				//End write

		if ( (curaddr & 1) )
			if ( CheckAbort(curaddr * 100 / length) )
				break;
	}
	CheckAbort(100);

	return curaddr;
}
