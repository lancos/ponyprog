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
#include "globals.h"
#include "e2profil.h"


#include "i2cbus.h"
#include "errcode.h"

#include "e2cmdw.h"

#include <QDebug>
#include <QtCore>

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
	if (busI->CheckDataLines(BUSYDELAY, 1, 1))
	{
		return OK;
	}
	else
	{
		return IICERR_BUSBUSY;
	}
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
	ShotDelay(4);		// tHD;STA = 4 usec
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
			qWarning() << Q_FUNC_INFO << "*** SCL error";
			return IICERR_SCLCONFLICT;
		}
	}
#else
	ShotDelay();
#endif
	ShotDelay(5);		// tSU;STOP = 4.7 usec
	setSDA();
	ShotDelay(2);

	if (getSDA() == 0)
	{
		qWarning() << Q_FUNC_INFO << "*** SDA error";
		return IICERR_SDACONFLICT;
	}

	//tBUF = 4.7 usec
	ShotDelay(2);

	return OK;
}

int I2CBus::SendBitMast(int inbit)
{
	int err = OK;
	busI->xferBit(err, inbit, xMODE_WRONLY);

	return err;
}

int I2CBus::RecBitMast()
{
	int err = OK;
	setSDA();
	int rv = busI->xferBit(err, 1, xMODE_RDONLY);
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

void I2CBus::RecoverSlave()
{
	int k;

	for (k = 0; k < 9; k++)
	{
		SendBitMast(1);
		if (getSDA() != 0)
		{
			break;
		}
	}
	setSCL();
	ShotDelay();
}

int I2CBus::WriteByte(int by, bool lsb)
{
	int err = OK;

	busI->xferByte(err, by, xMODE_WRONLY, 8, lsb);

	if (err == OK)
	{
		//Receive Ack
		setSDA();
		int lrb = busI->xferBit(err, 1, xMODE_RDONLY);

		if (err == OK)
		{
			if (lrb)
			{
				err = IICERR_NOTACK;
			}
		}
	}
	return err;
}

int I2CBus::ReadByte(int ack, bool lsb)
{
	int err = OK;
	setSDA();
	int rv = busI->xferByte(err, 0xff, xMODE_RDONLY, 8, lsb);
	if (err == OK)
	{
		//Send Ack
		busI->xferBit(err, ack, xMODE_WRONLY);
	}
	if (err == OK)
	{
		return rv;
	}
	else
	{
		return err;
	}
}

void I2CBus::SetDelay()
{
	int val = E2Profile::GetI2CSpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 1;          // as fast as your PC can (dangerous)
		break;

	case FAST:
		n = 2;          // > 100 Khz, < 400 Khz
		break;

	case SLOW:
		n = 20;         // (< 25 Khz)
		break;

	case VERYSLOW:
		n = 80;
		break;

	case ULTRASLOW:
		n = 500;
		break;

	default:
		n = 5;          //Default (< 100KHz)
		break;
	}

	Q_CHECK_PTR(busI);
	busI->SetDelay(n);

	qDebug() << Q_FUNC_INFO << "=" << n;
}

long I2CBus::Read(int slave, quint8 *data, long length, int page_size)
{
	long len;

	//qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << slave << "," << (void *)data << "," << (Qt::dec) << length << ") - IN";
	len = StartRead(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	//qDebug() << Q_FUNC_INFO << "=" << len << ", err_no =" << err_no << " - OUT";

	return len;
}

long I2CBus::Write(int slave, quint8 const *data, long length, int page_size)
{
	long len;

	//qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << slave << "," << data << "," << (Qt::dec) << length << ") - IN";

	len = StartWrite(slave, data, length);

	if (len == length)
		if (err_no || Stop())
		{
			len = 0;
		}

	//qDebug() << Q_FUNC_INFO << "=" << len << ", err_no =" << err_no << " - OUT";

	return len;
}

int I2CBus::Start(quint8 slave)
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
long I2CBus::StartRead(quint8 slave, quint8 *data, long length)
{
	int temp;
	long len = length;

	//qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << slave << "," << data << "," << (Qt::dec) << length << ") - IN";

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

			*data++ = (quint8)temp;
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

	*data = (quint8)temp;
	err_no = 0;

fineR:
	//qDebug() << Q_FUNC_INFO << "=" << (long)(length - len) << ", err_no =" << err_no << " - OUT";

	return length - len;
}

long I2CBus::StartWrite(quint8 slave, quint8 const *data, long length)
{
	int error;
	long len = length;

	//qDebug() << Q_FUNC_INFO << "(" << (Qt::hex) << slave << "," << data << "," << (Qt::dec) << length << ") - IN";

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
	//qDebug() << Q_FUNC_INFO << "=" << (long)(length - len) << ", err_no =" << err_no << " - OUT";

	return length - len;
}

int I2CBus::Stop(void)
{
	//qDebug() << Q_FUNC_INFO << "- IN";

	err_no = SendStop() ? IICERR_STOP : 0;

	//qDebug() << Q_FUNC_INFO << "=" << err_no << "- OUT";

	return err_no;
}

int I2CBus::Reset(void)
{
	qDebug() << Q_FUNC_INFO << "- IN";

	SetDelay();

	quint8 c;
	Read(0x00, &c, 0);
	setSCLSDA();
	WaitMsec(100);			//a big delay to allow no-CMOS 2402 to work

	qDebug() << Q_FUNC_INFO << "- OUT";

	return OK;
}

void I2CBus::Close(void)
{
	qDebug() << Q_FUNC_INFO << "busI=" << (Qt::hex) << busI << (Qt::dec);

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
