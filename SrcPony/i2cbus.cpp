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
#include "e2profil.h"


#include "i2cbus.h"
#include "errcode.h"

#include "e2cmdw.h"

#include <QDebug>
#include <QtCore>

#define SCLTIMEOUT      900     // enable SCL check and timing (for slaves that hold down the SCL line to slow the transfer)

#define BUSYDELAY       100
#define SDATIMEOUT      200

I2CBus::I2CBus(BusInterface *ptr)
	: BusIO(ptr)
{
}

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

	return (count ? IICERR_BUSBUSY : OK);
}

/** I2CBus specification **
-- Standard I2CBus --
SCL
 T-low = 5 usec min
 T-high = 4 usec min

SCL
 Data setup time = 300 nsec min
 Data hold time = 300 nsec min

ALL
 fall time = 300 nsec max
 rise time = 1 usec max

-- Fast I2CBus --
SCL
 T-low = 1.3 usec min
 T-high = 0.6 usec min

ALL
 fall time = 300 nsec max
 rise time = 300 nsec max
**/

int I2CBus::SendStart()
{
	register int err;

	setSDA();
	ShotDelay(2);		//Help repeated start condition
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
			return IICERR_SCLCONFLICT;
		}
	}
#endif
	ShotDelay(2);

	int recoveryCnt = 0;
	while (CheckBusy() != OK && recoveryCnt++ < 3)
	{
		//Recover I2C
		RecoverSlave();
	}

	if ((err = CheckBusy()) != OK)
	{
		return err;
	}

	clearSDA();
	ShotDelay(3);		// tHD;STA = 4 usec
	clearSCL();
	ShotDelay();

	return OK;
}

int I2CBus::SendStop()
{
	clearSCL();
	ShotDelay(2);
	clearSDA();
	ShotDelay(2);
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
			qWarning() << __PRETTY_FUNCTION__ << "*** SCL error";
			return IICERR_SCLCONFLICT;
		}
	}
#else
	ShotDelay();
#endif
	ShotDelay(3);		// tSU;STOP = 4.7 usec
	setSDA();
	ShotDelay(2);

	if (getSDA() == 0)
	{
		qWarning() << __PRETTY_FUNCTION__ << "*** SDA error";
		return IICERR_SDACONFLICT;
	}

	//tBUF = 4.7 usec
	ShotDelay();

	return OK;
}

int I2CBus::SendBitMast(int inbit)
{
	bitSDA(inbit);		// SDA must be high to receive data (low dominant)
	ShotDelay();		// tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
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
			return IICERR_SCLCONFLICT;
		}
	}
#endif
	ShotDelay();		// tHIGH / 2 = 2 usec
	int outbit = getSDA();
	ShotDelay();		// tHIGH / 2 = 2 usec
	clearSCL();
	ShotDelay();		// tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)

	return outbit;
}

void I2CBus::RecoverSlave()
{
	int k;

	for (k = 0; k < 9; k++)
	{
		SendBitMast(1);
		if (getSDA() != 0)
			break;
	}
	setSCL();
	ShotDelay();
}

int I2CBus::WriteByte(int by, bool lsb)
{
	int lrb, k;

	if (lsb)
	{
		for (k = 0; k < 8; k++)
		{
			lrb = SendBitMast(by & (1 << k));
			if (lrb < 0)
				return lrb;
		}
	}
	else
	{
		for (k = 7; k >= 0; k--)
		{
			lrb = SendBitMast(by & (1 << k));
			if (lrb < 0)
				return lrb;
		}
	}

	lrb = RecBitMast();		// acknowledge bit
	if (lrb != OK)
		return lrb;

	if (lrb)
		return IICERR_NOTACK;

	return OK;
}

int I2CBus::ReadByte(int ack, bool lsb)
{
	int k, bit, val = 0;

	if (lsb)
	{
		for (k = 0; k < 8; k++)
		{
			bit = RecBitMast();
			if (bit < 0)
				return bit;

			if (bit)
			{
				val |= 1 << k;
			}
		}
	}
	else
	{
		for (k = 7; k >= 0; k--)
		{
			bit = RecBitMast();
			if (bit < 0)
				return bit;

			if (bit)
			{
				val |= 1 << k;
			}
		}
	}

	bit = SendBitMast(ack);
	if (bit < 0)		// send the ack
		return bit;

	setSDA();			// release SDA line to the slave trasmitter

	return val;
}

void I2CBus::SetDelay()
{
	int val = E2Profile::GetI2CSpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 0;          // as fast as your PC can (dangerous)
		break;

	case FAST:
		n = 1;          // > 100 Khz, < 400 Khz
		break;

	case SLOW:
		n = 10;         // (< 25 Khz)
		break;

	case VERYSLOW:
		n = 50;
		break;

	case ULTRASLOW:
		n = 500;
		break;

	default:
		n = 3;          //Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << __PRETTY_FUNCTION__ << "=" << n;
}

long I2CBus::Read(int slave, uint8_t *data, long length, int page_size)
{
	long len;

	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << slave << "," << (void *)data << "," << (dec) << length << ") - IN";
	len = StartRead(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	qDebug() << __PRETTY_FUNCTION__ << "=" << len << ", err_no =" << err_no << " - OUT";

	return len;
}

long I2CBus::Write(int slave, uint8_t const *data, long length, int page_size)
{
	long len;

	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << slave << "," << data << "," << (dec) << length << ") - IN";

	len = StartWrite(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	qDebug() << __PRETTY_FUNCTION__ << "=" << len << ", err_no =" << err_no << " - OUT";

	return len;
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

	if ((temp = WriteByte(slave)) != OK)
	{
		err_no = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
		last_addr = slave;
		return err_no;
	}

	return OK;
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

	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << slave << "," << data << "," << (dec) << length << ") - IN";

	if (len > 0)
	{
		// send Start
		if ((temp = SendStart()))
		{
			err_no = temp;
			return 0;
		}

		if ((temp = WriteByte(slave | 1)) != 0)
		{
			err_no = (temp == IICERR_NOTACK) ? IICERR_NOADDRACK : temp;
			last_addr = slave | 1;
			return 0;
		}

		while (len > 1)
		{
			if ((temp = ReadByte(0)) < 0)
			{
				err_no = temp;
				goto fineR;
			}

			*data++ = (uint8_t)temp;
			len--;
		}
	}

	// last byte received without acknowledge
	if ((temp = ReadByte(1)) < 0)
	{
		err_no = temp;
		goto fineR;
	}

	len--;

	*data = (uint8_t)temp;
	err_no = 0;

fineR:
	qDebug() << __PRETTY_FUNCTION__ << "=" << (long)(length - len) << ", err_no =" << err_no << " - OUT";

	return length - len;
}

long I2CBus::StartWrite(uint8_t slave, uint8_t const *data, long length)
{
	int error;
	long len = length;

	qDebug() << __PRETTY_FUNCTION__ << "(" << (hex) << slave << "," << data << "," << (dec) << length << ") - IN";

	if (len == 0)
	{
		return 0;
	}

	if ((error = SendStart()))
	{
		err_no = error;
		return 0;
	}

	if ((error = WriteByte(slave & 0xFE)))
	{
		err_no = (error == IICERR_NOTACK) ? IICERR_NOADDRACK : error;
		last_addr = slave & 0xFE;
		return 0;
	}

	while (len > 0)
	{
		if ((error = WriteByte(*data++)) != 0)
		{
			err_no = error;
			goto fineW;
		}

		len--;
	}

fineW:
	qDebug() << __PRETTY_FUNCTION__ << "=" << (long)(length - len) << ", err_no =" << err_no << " - OUT";

	return length - len;
}

int I2CBus::Stop(void)
{
	qDebug() << __PRETTY_FUNCTION__ << "- IN";

	err_no = SendStop() ? IICERR_STOP : 0;

	qDebug() << __PRETTY_FUNCTION__ << "=" << err_no << "- OUT";

	return err_no;
}

int I2CBus::Reset(void)
{
	qDebug() << __PRETTY_FUNCTION__ << "- IN";

	SetDelay();

	uint8_t c;
	Read(0x00, &c, 0);
	setSCLSDA();
	WaitMsec(100);			//a big delay to allow no-CMOS 2402 to work

	qDebug() << __PRETTY_FUNCTION__ << "- OUT";

	return OK;
}

void I2CBus::Close(void)
{
	qDebug() << __PRETTY_FUNCTION__ << "busI=" << (hex) << busI << (dec);

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
