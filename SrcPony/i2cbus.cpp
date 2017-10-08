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


#include "i2cbus.h"
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

#define SCLTIMEOUT      900     // enable SCL check and timing (for slaves that hold down the SCL line to slow the transfer)

#define BUSYDELAY       100
#define SDATIMEOUT      200

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
	{
		WaitUsec(1);
	}

	return (count ? IICERR_BUSBUSY : 0);
}

int I2CBus::SendStart()
{
	register int err;

	setSCLSDA();
#ifdef  SCLTIMEOUT

	if (getSCL() == 0)
	{
		int k;

		for (k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
		{
			WaitUsec(1);
		}

		if (k == 0)
		{
			return IICERR_SCLCONFLICT;
		}
	}

#else
	WaitUsec(shot_delay / 2);
#endif

	if ((err = CheckBusy()))
	{
		return err;
	}

	clearSDA();
	WaitUsec(shot_delay);   // tHD;STA = 4 usec
	clearSCL();
	WaitUsec(shot_delay / 2);

	return 0;
}

int I2CBus::SendStop()
{
	clearSCL();
	WaitUsec(shot_delay + 1);
	clearSDA();
	WaitUsec(shot_delay + 1);
	setSCL();
#ifdef  SCLTIMEOUT

	if (getSCL() == 0)
	{
		int k;

		for (k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
		{
			WaitUsec(1);
		}

		if (k == 0)
		{
			//      qDebug() << "I2CBus::SendStop() *** SCL error";
			return IICERR_SCLCONFLICT;
		}
	}

#else
	WaitUsec(shot_delay / 2);
#endif
	WaitUsec(shot_delay + 1); // tSU;STOP = 4.7 usec
	setSDA();
	WaitUsec(shot_delay / 2 + 1);

	if (getSDA() == 0)
	{
		//      qDebug() << "I2CBus::SendStop() *** SDA error";
		return IICERR_SDACONFLICT;
	}

	//tBUF = 4.7 usec
	WaitUsec(shot_delay);

	return 0;
}

int I2CBus::SendBitMast(int b)
{
	bitSDA(b);
	WaitUsec(shot_delay / 2 + 1);   // tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
	setSCL();

	/* Se SCL e` ancora 0 significa che uno Slave sta` rallentando
	 * il trasferimento, attendi che SCL si rialzi */
	if (getSCL() == 0)
	{
#ifdef  SCLTIMEOUT
		int k;

		for (k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
		{
			WaitUsec(1);
		}

		if (k == 0)
		{
			return IICERR_SCLCONFLICT;
		}

#endif
	}

	WaitUsec(shot_delay / 2); // tHIGH / 2 = 2 usec

	if (!getSDA() != !b)
	{
		return IICERR_SDACONFLICT;
	}

	WaitUsec(shot_delay / 2); // tHIGH / 2 = 2 usec
	clearSCL();
	WaitUsec(shot_delay / 2); // tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)

	return 0;
}

// returns a negative number if there's an error, 0 or 1 otherwise
int I2CBus::RecBitMast()
{
	register uint8_t b;

	setSDA();               // to receive data SDA must be high
	WaitUsec(shot_delay / 2 + 1);   // tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
	setSCL();

	/* Se SCL e` ancora 0 significa che uno Slave sta` rallentando
	 * il trasferimento, attendi che SCL si rialzi */
	if (getSCL() == 0)
	{
#ifdef  SCLTIMEOUT
		int k;

		for (k = SCLTIMEOUT; getSCL() == 0 && k > 0; k--)
		{
			WaitUsec(1);
		}

		if (k == 0)
		{
			return IICERR_SCLCONFLICT;
		}

#endif
	}

	WaitUsec(shot_delay / 2); // tHIGH / 2 = 2 usec
	b = getSDA();
	WaitUsec(shot_delay / 2); // tHIGH / 2 = 2 usec
	clearSCL();
	WaitUsec(shot_delay / 2); // tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)

	return b;
}

// OK, ora ci alziamo di un livello: operiamo sul byte
int I2CBus::SendByteMast(int by)
{
	int lrb, k;

	for (k = 7; k >= 0; k--)
		if ((lrb = SendBitMast(by & (1 << k))))
		{
			return lrb;
		}

	lrb = RecBitMast();     // acknowledge bit

	if (lrb < 0)            // < 0 means that an error occured
	{
		return lrb;
	}

	if (lrb)
	{
		return IICERR_NOTACK;
	}

	return 0;
}

int I2CBus::SendByteMastLSB(int by)
{
	int lrb, k;

	for (k = 0; k < 8; k++)
		if ((lrb = SendBitMast(by & (1 << k))))
		{
			return lrb;
		}

	lrb = RecBitMast();     // acknowledge bit

	if (lrb < 0)            // < 0 means that an error occured
	{
		return lrb;
	}

	if (lrb)
	{
		return IICERR_NOTACK;
	}

	return 0;
}

int I2CBus::RecByteMast(int ack)
{
	int k, lrb, val = 0;

	for (k = 7; k >= 0; k--)
	{
		lrb = RecBitMast();

		if (lrb < 0)
		{
			return lrb;
		}

		if (lrb)
		{
			val |= 1 << k;
		}
	}

	if ((k = SendBitMast(ack)))     // send the ack
	{
		return k;
	}

	setSDA();       // release SDA line to the slave trasmitter

	return val;
}

int I2CBus::RecByteMastLSB(int ack)
{
	int k, lrb, val = 0;

	for (k = 0; k < 8; k++)
	{
		lrb = RecBitMast();

		if (lrb < 0)
		{
			return lrb;
		}

		if (lrb)
		{
			val |= 1 << k;
		}
	}

	if ((k = SendBitMast(ack)))     // send the ack
	{
		return k;
	}

	setSDA();       // release SDA line to the slave trasmitter

	return val;
}

void I2CBus::SetDelay()
{
	int val = E2Profile::GetI2CSpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 0;          // as fast as your PC can
		break;

	case FAST:
		n = 2;          // > 100 Khz, < 400 Khz
		break;

	case SLOW:
		n = 20;         // (< 25 Khz)
		break;

	case VERYSLOW:
		n = 100;
		break;

	case ULTRASLOW:
		n = 1000;
		break;

	default:
		n = 5;          //Default (< 100KHz)
		break;
	}

	BusIO::SetDelay(n);

	qDebug() << "I2CBus::SetDelay() = " << n;
}

long I2CBus::Read(int slave, uint8_t *data, long length, int page_size)
{
	long len;

	qDebug() << "I2CBus::Read(" << slave << ", " << (void *) data << ", " << length << ") - IN";
	len = StartRead(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	qDebug() << "I2CBus::Read() = " << len << ", err_no = " << err_no << " - OUT";

	return len;
}

long I2CBus::Write(int slave, uint8_t const *data, long length, int page_size)
{
	long len;

	qDebug() << "I2CBus::Write(" << slave << ", " << (hex) << data << ", " << (dec) << length << ") - IN";

	len = StartWrite(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	qDebug() << "I2CBus::Write() = " << len << ", err_no = " << err_no << " - OUT";

	return len;
}

int I2CBus::ReadByte(int ack, int lsb)
{
	if (lsb)
	{
		return RecByteMastLSB(ack);
	}
	else
	{
		return RecByteMast(ack);
	}
}

int I2CBus::WriteByte(int by, int lsb)
{
	if (lsb)
	{
		return SendByteMastLSB(by);
	}
	else
	{
		return SendByteMast(by);
	}
}

int I2CBus::Start(uint8_t slave)
{
	int temp;

	// send Start
	if ((temp = SendStart()))
	{
		err_no = temp;
		return err_no;
	}

	if ((temp = SendByteMast(slave)) != 0)
	{
		err_no = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
		last_addr = slave;
		return err_no;
	}

	return 0;
}

/* se length e` zero, viene ricevuto un byte senza ack per cercare di
 * liberare il bus da uno slave che per qualche motivo e` ancora in
 * trasmissione. In questo caso data deve puntare ad un buffer di (almeno)
 * un byte.
 */
long I2CBus::StartRead(uint8_t slave, uint8_t *data, long length)
{
	int temp;
	long len = length;

	qDebug() << "I2CBus::StartRead(" << slave << ", " << (hex) << data << ", " << (dec) << length << ") - IN";

	if (len > 0)
	{
		// send Start
		if ((temp = SendStart()))
		{
			err_no = temp;
			return 0;
		}

		if ((temp = SendByteMast(slave | 1)) != 0)
		{
			err_no = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
			last_addr = slave | 1;
			return 0;
		}

		while (len > 1)
		{
			if ((temp = RecByteMast(0)) < 0)
			{
				err_no = temp;
				goto fineR;
			}

			*data++ = (uint8_t)temp;
			len--;
		}
	}

	// last byte received without acknowledge
	if ((temp = RecByteMast(1)) < 0)
	{
		err_no = temp;
		goto fineR;
	}

	len--;

	*data = (uint8_t)temp;
	err_no = 0;

fineR:
	qDebug() << "I2CBus::StartRead() = " << (long)(length - len) << ", err_no = " << err_no << " - OUT";

	return length - len;
}

long I2CBus::StartWrite(uint8_t slave, uint8_t const *data, long length)
{
	int error;
	long len = length;

	qDebug() << "I2CBus::StartWrite(" << slave << ", " << (hex) << data << ", " << (dec) <<  length << ") - IN";

	if (len == 0)
	{
		return 0;
	}

	if ((error = SendStart()))
	{
		err_no = error;
		return 0;
	}

	if ((error = SendByteMast(slave & 0xFE)))
	{
		err_no = (error == IICERR_NOTACK) ? IICERR_NOADDRACK : error;
		last_addr = slave & 0xFE;
		return 0;
	}

	while (len > 0)
	{
		if ((error = SendByteMast(*data++)) != 0)
		{
			err_no = error;
			goto fineW;
		}

		len--;
	}

fineW:
	qDebug() << "I2CBus::StartWrite() = " << (long)(length - len) << ", err_no = " << err_no << " - OUT";

	return length - len;
}

int I2CBus::Stop(void)
{
	qDebug() << "I2CBus::Stop() - IN";

	err_no = SendStop() ? IICERR_STOP : 0;

	qDebug() << "I2CBus::Stop() = " << err_no << " - OUT";

	return err_no;
}

int I2CBus::Reset(void)
{
	qDebug() << "I2CBus::Reset() - IN";

	SetDelay();

	uint8_t c;
	Read(0x00, &c, 0);
	setSCLSDA();
	WaitMsec(100);          //tolto il commento il 25/01/1999 e raddoppiato per permettere il funzionamento della 2402 non Cmos

	qDebug() << "I2CBus::Reset() - OUT";

	return OK;
}

void I2CBus::Close(void)
{
	qDebug() << "I2CBus::Close() busI=" << (hex) << busI << (dec);

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

		if (err != OK  &&  err != IICERR_NOADDRACK)
		{
			ret_val = err;
		}
	}

	busI->TestRestore();

	return ret_val;
}
