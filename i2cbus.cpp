//=========================================================================//
//-------------------------------------------------------------------------//
// i2cbus.cpp -- Source for I2C Bus driver class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include "types.h"
#include "i2cbus.h"
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

#define	SCLTIMEOUT	200	// enable SCL check and timing (for slaves that hold down the SCL line to slow the transfer)

#define BUSYDELAY	100
#define SDATIMEOUT	200

// Constructor
I2CBus::I2CBus(BusInterface *ptr)
	: BusIO(ptr)
{
	shot_delay = 0;
}

// Destructor
I2CBus::~I2CBus()
{
	Close();
}

int I2CBus::CheckBusy()
{
	register int count;

	for (count = BUSYDELAY; count > 0 && isSCLSDAuno(); count--)
		WaitUsec(1);
	return (count ? IICERR_BUSBUSY : 0);
}

int I2CBus::SendStart()
{
	register int err;

	setSCLSDA();
#ifdef	SCLTIMEOUT
	if (getSCL() == 0)
	{
		int k;

		for(k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
			WaitUsec(2);
		if (k == 0)
			return IICERR_SCLCONFLICT;
	}
#else
 	WaitUsec(shot_delay/2);
#endif
	if ( (err = CheckBusy()) )
		return err;
	clearSDA();
	WaitUsec(shot_delay);	// tHD;STA = 4 usec
	clearSCL();
	WaitUsec(shot_delay/2);

	return 0;
}

int I2CBus::SendStop()
{
	clearSCL();
	WaitUsec(shot_delay+1);
	clearSDA();
	WaitUsec(shot_delay+1);
	setSCL();
#ifdef	SCLTIMEOUT
	if (getSCL() == 0)
	{
		int k;

		for(k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
			WaitUsec(2);
		if (k == 0)
		{
		//	UserDebug(UserApp2, "I2CBus::SendStop() *** SCL error\n");
			return IICERR_SCLCONFLICT;
		}
	}
#else
	WaitUsec(shot_delay/2);
#endif
	WaitUsec(shot_delay+1);	// tSU;STOP = 4.7 usec
	setSDA();
	WaitUsec(shot_delay/2+1);
	if (getSDA() == 0)
	{
	//	UserDebug(UserApp2, "I2CBus::SendStop() *** SDA error\n");
		return IICERR_SDACONFLICT;
	}

	//tBUF = 4.7 usec
	WaitUsec(shot_delay);

	return 0;
}

int I2CBus::SendBitMast(int b)
{
	bitSDA(b);
	WaitUsec(shot_delay/2+1);	// tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
	setSCL();
	/* Se SCL e` ancora 0 significa che uno Slave sta` rallentando
	 * il trasferimento, attendi che SCL si rialzi */
	if (getSCL() == 0)
	{
#ifdef	SCLTIMEOUT
		int k;

		for(k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
			WaitUsec(2);
		if (k == 0)
			return IICERR_SCLCONFLICT;
#endif
	}
	WaitUsec(shot_delay/2);	// tHIGH / 2 = 2 usec
	if (!getSDA() != !b)
		return IICERR_SDACONFLICT;
	WaitUsec(shot_delay/2);	// tHIGH / 2 = 2 usec
	clearSCL();
	WaitUsec(shot_delay/2);	// tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)

	return 0;
}

// returns a negative number if there's an error, 0 or 1 otherwise
int I2CBus::RecBitMast()
{
	register UBYTE b;

	setSDA();		// to receive data SDA must be high
	WaitUsec(shot_delay/2+1);	// tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
	setSCL();
	/* Se SCL e` ancora 0 significa che uno Slave sta` rallentando
	 * il trasferimento, attendi che SCL si rialzi */
	if (getSCL() == 0)
	{
#ifdef	SCLTIMEOUT
		int k;

		for(k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
			WaitUsec(2);
		if (k == 0)
			return IICERR_SCLCONFLICT;
#endif
	}
	WaitUsec(shot_delay/2);	// tHIGH / 2 = 2 usec
	b = getSDA();
	WaitUsec(shot_delay/2);	// tHIGH / 2 = 2 usec
	clearSCL();
	WaitUsec(shot_delay/2);	// tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)

	return b;
}

/* OK, ora ci alziamo di un livello: operiamo sul byte */
int I2CBus::SendByteMast(int by)
{
	int lrb,k;

	for (k = 7; k >= 0; k--)
		if ( (lrb = SendBitMast(by & (1<<k))) )
			return lrb;

	lrb = RecBitMast();	// acknowledge bit
	if (lrb < 0)		// < 0 means that an error occured
		return lrb;
	if (lrb)
		return IICERR_NOTACK;

	return 0;
}

int I2CBus::RecByteMast(int ack)
{
	int k,lrb,val = 0;

	for (k = 7; k >= 0; k--)
	{
		lrb = RecBitMast();
		if (lrb < 0)
			return lrb;
		if (lrb)
			val |= 1 << k;
	}
	if ( (k = SendBitMast(ack)) )	// send the ack
		return k;
	setSDA();	// release SDA line to the slave trasmitter

	return val;
}

void I2CBus::SetDelay()
{
	int val = THEAPP->GetI2CSpeed();
	int n;

	switch(val)
	{
	case TURBO:
		n = 0;		// as fast as your PC can
		break;
	case FAST:
		n = 2;		// > 100 Khz, < 400 Khz
		break;
	case SLOW:
		n = 20;		// (< 25 Khz)
		break;
//	case VERYSLOW:
//		n = 100;
//		break;
	default:
		n = 5;		//Default (< 100KHz)
		break;
	}
	BusIO::SetDelay(n);

	UserDebug1(UserApp2, "I2CBus::SetDelay() = %d\n", n);
}

/** Obsolete calibration routine
//Calibra il ritardo per la temporizzazione I2C
int I2CBus::Calibration(int slave)
{
	UBYTE ch;
	int step = 2;

	delay_loop = 10000;		//ritardo con cui deve funzionare anche un PentiumII 300MHz e successivi...
	if ( StartWrite(slave, &ch, 1) == 1 )
	{
		delay_loop = 1;
		int k;
		for (k = 0; Read(slave, &ch, 1) != 1 && k < 100; k++)
			if ( errno == IICERR_NOADDRACK )
			{
				delay_loop += step;
				if ( delay_loop > 80 )
					step = 20;
				else
				if ( delay_loop > 40 )
					step = 10;
				else
				if ( delay_loop > 20 )
					step = 5;
			}
	}

	if (errno)
		delay_loop = DELAYLOOP;
	else
		delay_loop++;

	return errno;
}
**/

long I2CBus::Read(int slave, UBYTE *data, long length)
{
	long len;

	UserDebug3(UserApp2, "I2CBus::Read(%d, %x, %ld) - IN\n", slave, data, length);
	len = StartRead(slave, data, length);
	if (len == length)
		if ( errno || Stop() )
			len = 0;

	UserDebug2(UserApp2, "I2CBus::Read() = %d, errno = %d - OUT\n", len, errno);

	return len;
}

long I2CBus::Write(int slave, UBYTE const *data, long length)
{
	long len;

	UserDebug3(UserApp2, "I2CBus::Write(%d, %x, %ld) - IN\n", slave, data, length);

	len = StartWrite(slave, data, length);
	if (len == length)
		if ( errno || Stop())
			len = 0;

	UserDebug2(UserApp2, "I2CBus::Write() = %d, errno = %d - OUT\n", len, errno);

	return len;
}

int I2CBus::ReadByte(int ack)
{
	return RecByteMast(ack);
}


int I2CBus::Start(UBYTE slave)
{
	int temp;

	// send Start
	if ( (temp = SendStart()) )
	{
		errno = temp;
		return errno;
	}

	if ( (temp = SendByteMast(slave)) != 0 )
	{
		errno = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
		last_addr = slave;
		return errno;
	}

	return 0;
}

/* se length e` zero, viene ricevuto un byte senza ack per cercare di
 * liberare il bus da uno slave che per qualche motivo e` ancora in
 * trasmissione. In questo caso data deve puntare ad un buffer di (almeno)
 * un byte.
 */
ULONG I2CBus::StartRead(UBYTE slave, UBYTE *data, ULONG length)
{
	int temp;
	ULONG len = length;

	UserDebug3(UserApp2, "I2CBus::StartRead(%d, %x, %ld) - IN\n", slave, data, length);

	if (len > 0)
	{
		// send Start
		if ( (temp = SendStart()) )
		{
			errno = temp;
			return 0;
		}

		if ( (temp = SendByteMast(slave | 1)) != 0 )
		{
			errno = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
			last_addr = slave | 1;
			return 0;
		}

		while (len > 1)
		{
			if ((temp = RecByteMast(0)) < 0)
			{
				errno = temp;
				goto fineR;
			}
			*data++ = (UBYTE)temp;
			len--;
		}
	}
	// last byte received without acknowledge
	if ((temp = RecByteMast(1)) < 0)
	{
		errno = temp;
		goto fineR;
	}
	len--;

	*data = (UBYTE)temp;
	errno = 0;

fineR:
	UserDebug2(UserApp2, "I2CBus::StartRead() = %d, errno = %d - OUT\n", length-len, errno);

	return length-len;
}

ULONG I2CBus::StartWrite(UBYTE slave, UBYTE const *data, ULONG length)
{
	int error;
	ULONG len = length;

	UserDebug3(UserApp2, "I2CBus::StartWrite(%d, %x, %ld) - IN\n", slave, data, length);

	if (len == 0)
		return 0;

	if ( (error = SendStart()) )
	{
		errno = error;
		return 0;
	}
	if ( (error = SendByteMast(slave & 0xFE)) )
	{
		errno = (error == IICERR_NOTACK) ? IICERR_NOADDRACK : error;
		last_addr = slave & 0xFE;
		return 0;
	}

	while (len > 0)
	{
		if ( (error = SendByteMast(*data++)) != 0 )
		{
			errno = error;
			goto fineW;
		}
		len--;
	}

fineW:
	UserDebug2(UserApp2, "I2CBus::StartWrite() = %d, errno = %d - OUT\n", length-len, errno);

	return length-len;
}

int I2CBus::Stop(void)
{
	UserDebug(UserApp3, "I2CBus::Stop() - IN\n");

	errno = SendStop() ? IICERR_STOP : 0;

	UserDebug1(UserApp3, "I2CBus::Stop() = %d - OUT\n", errno);

	return errno;
}

int I2CBus::Reset(void)
{
	UserDebug(UserApp3, "I2CBus::Reset() - IN\n");

	SetDelay();

	BYTE c;
	Read(0x00, &c, 0);
	setSCLSDA();
	WaitMsec(100);		//tolto il commento il 25/01/1999 e raddoppiato per permettere il funzionamento della 2402 non Cmos

	UserDebug(UserApp3, "I2CBus::Reset() - OUT\n");

	return OK;
}

void I2CBus::Close(void)
{
	UserDebug1(UserApp1, "I2CBus::Close() busI=%xh\n", busI);

	setSCLSDA();
	BusIO::Close();
}

int I2CBus::TestPort(int port)
{
	int ret_val = busI->TestSave(port);

	if (ret_val == OK)
	{
		// Send general call address 0x00
		int err = Start(0x00 | 1);
		if ( err != OK  &&  err != IICERR_NOADDRACK )
			ret_val = err;
	}
	busI->TestRestore();

	return ret_val;
}
