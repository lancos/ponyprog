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

#ifndef _BUSINTERFACE_H
#define _BUSINTERFACE_H

#include "types.h"
#include "errcode.h"
#include "wait.h"
#include "globals.h"
#include "interfconv.h"

#include <QDebug>

//Some useful flags
#define SPIMODE_CPHA		0x01		// clock phase/edge
#define SPIMODE_CPOL		0x02		// clock polarity
#define SPIMODE_MASK		0x03

#define xMODE_RDONLY		0x04
#define xMODE_WRONLY		0x08

//#define I2CMODE_MASK		0x10

enum
{
	SPI_MODE_0 = (0 | 0),
	SPI_MODE_1 = (0 | SPIMODE_CPHA),
	SPI_MODE_2 = (SPIMODE_CPOL | 0),
	SPI_MODE_3 = (SPIMODE_CPOL | SPIMODE_CPHA)
};

#define SCLTIMEOUT      900     // enable SCL check and timing (for slaves that hold down the SCL line to slow the transfer)

class BusInterface
{
  public:
	BusInterface()
		: old_portno(-1),
		  usb_vp(0),
		  installed(-1),
		  cmd2cmd_delay(0),
		  shot_delay(5),
		  i2c_mode(false)
	{
	}

	virtual int Open(int port) = 0;
	virtual void Close() = 0;

	virtual int TestOpen(int port)
	{
		qDebug() << Q_FUNC_INFO << "(" << port << ") IN";

		int ret_val = TestSave(port);

		TestRestore();

		qDebug() << Q_FUNC_INFO << "=" << ret_val << " OUT";

		return ret_val;
	}
	virtual int TestPort(int port)
	{
		qDebug() << Q_FUNC_INFO << "(" << port << ") IN";

		return TestOpen(port);
	}
	virtual int TestSave(int port)
	{
		int ret_val;

		qDebug() << Q_FUNC_INFO << "(" << port << ") IN";

		old_portno = installed;

		Close();

		if ((ret_val = Open(port)) == OK)
		{
		}

		qDebug() << Q_FUNC_INFO << "=" << ret_val << " OUT";

		return ret_val;
	}
	virtual void TestRestore()
	{
		qDebug() << Q_FUNC_INFO << "IN *** Inst=" << installed;

		if (IsInstalled())
		{
			Close();
		}

		if (old_portno >= 0)
		{
			Open(old_portno);
			old_portno = -1;
		}

		qDebug() << Q_FUNC_INFO << "OUT";
	}

	virtual int SetPower(bool onoff)
	{
		return OK;
	}
	virtual void SetControlLine(int res = 1)
	{
	}

	virtual void SetDataOut(int sda = 1) = 0;
	virtual void SetInvDataOut(int sda = 1)
	{
		SetDataOut(!sda);
	}
	virtual void SetClock(int scl = 1) = 0;
	virtual int GetDataIn() = 0;
	virtual int GetClock() = 0;
	virtual void SetClockData() = 0;
	virtual void ClearClockData()
	{ }
	virtual int IsClockDataUP() = 0;
	virtual int IsClockDataDOWN() = 0;

	virtual bool CheckDataLines(int len = 1, int sda = -1, int scl = -1)
	{
		bool test = false;

		if (len > 0)
		{
			do
			{
				if (sda == 0 && scl == 0)
				{
					test = IsClockDataDOWN();
				}
				else if (sda > 0 && scl > 0)
				{
					test = IsClockDataUP();
				}
				else
				{
					bool test_sda = true, test_scl = true;

					if (sda > 0)
					{
						test_sda = GetDataIn();
					}
					else if (sda == 0)
					{
						test_sda = !GetDataIn();
					}

					if (scl > 0)
					{
						test_scl = GetClock();
					}
					else if (scl == 0)
					{
						test_scl = !GetClock();
					}

					test = (test_sda && test_scl);
				}
			}
			while (test && --len > 0);
		}

		return test;
	}

	int GetCmd2CmdDelay() const
	{
		return cmd2cmd_delay;
	}
	void SetCmd2CmdDelay(int delay)
	{
		if (delay >= 0)
		{
			cmd2cmd_delay = delay;
		}
	}

	bool IsInstalled() const
	{
		return (installed >= 0) ? true : false;
	}

	void SetUSBVidPid(VidPid vp)
	{
		usb_vp = vp;
	}
	VidPid GetUSBVid()
	{
		return usb_vp;
	}

	virtual void WaitMsec(unsigned int msec)
	{
		Flush();
		w.WaitMsec(msec);
	}

	virtual void WaitUsec(unsigned int usec)
	{
		Flush();
		w.WaitUsec(usec);
	}

	virtual void ShotDelay(int n = 1)
	{
		w.WaitUsec(shot_delay * n);
	}

	virtual int xferBit(int &err, int b, int mode = 0)
	{
		int ret = 0;

		if (!i2c_mode)	//(mode & I2CMODE_MASK) == 0)
		{
			switch (mode & SPIMODE_MASK)
			{
			case 3:
				SetClock(0);
				SetDataOut(b);
				ShotDelay();
				SetClock(1);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				break;
			case 2:
				SetDataOut(b);
				ShotDelay();
				SetClock(0);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				SetClock(1);
				break;
			case 1:
				SetClock(1);
				SetDataOut(b);
				ShotDelay();
				SetClock(0);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				break;
			case 0:
			default:
				SetDataOut(b);
				ShotDelay();
				SetClock(1);
				if ((mode & xMODE_WRONLY) == 0)
				{
					ret = GetDataIn();
				}
				ShotDelay();
				SetClock(0);
				break;
			}
		}
		else
		{
			//I2CBus
			SetDataOut(b);		// SDA must be high to receive data (low dominant)
			ShotDelay();		// tSU;DAT = 250 nsec (tLOW / 2 = 2 usec)
			SetClock(1);
#ifdef SCLTIMEOUT
			for (int k = SCLTIMEOUT; GetClock() == 0 && k > 0; k--)
			{
				WaitUsec(1);
			}
			if (GetClock() == 0)
			{
				return IICERR_SCLCONFLICT;
			}
#endif
			ShotDelay();		// tHIGH / 2 = 2 usec
			if ((mode & xMODE_WRONLY) == 0)
			{
				ret = GetDataIn();
			}
			ShotDelay();		// tHIGH / 2 = 2 usec
			SetClock(0);
			ShotDelay();		// tHD;DATA = 300 nsec (tLOW / 2 = 2 usec)
		}
		err = OK;
		return ret;
	}

	virtual quint8 xferByte(int &err, quint8 by, int mode = 0, int bpw = 8, bool lsb_first = false)
	{
		return (quint8)xferWord(err, by, mode, bpw, lsb_first);
	}

	virtual unsigned long xferWord(int &err, unsigned long word_out, int mode = 0, int bpw = 8, bool lsb_first = false)
	{
		quint32 word_in = 0;
		quint32 bitmask;
		err = OK;

		if (!i2c_mode)	//if ((mode & I2CMODE_MASK) == 0)
		{
			switch (mode & SPIMODE_MASK)
			{
			case 3:
			case 2:
				SetClock(1);
				break;
			case 1:
			case 0:
			default:
				SetClock(0);
				break;
			}
		}

		if (lsb_first)
		{
			bitmask = 1;
		}
		else
		{
			bitmask = 1 << (bpw - 1);
		}

		for (int k = 0; k < bpw; k++)
		{
			if (xferBit(err, word_out & bitmask, mode))
			{
				word_in |= bitmask;
			}

			if (lsb_first)
			{
				bitmask <<= 1;
			}
			else
			{
				bitmask >>= 1;
			}
		}
		SetDataOut(1);

		return word_in;
	}

	virtual void SetDelay(int delay)
	{
		if (delay >= 0)
		{
			shot_delay = delay;
		}
	}
	int GetDelay() const
	{
		return shot_delay;
	}

	void SetI2CMode(bool mode)
	{
		i2c_mode = mode;
	}
	bool GetI2CMode() const
	{
		return i2c_mode;
	}

	virtual void ConfigPins(int pinum_ctrl = -1, int pinum_datain = -1, int pinum_dataout = -1, int pinum_clock = -1, int pinum_clockin = -1, int pinum_poweron = -1, int pinum_enbus = -1, int pinnum_ctrlin = -1)
	{
		pins.ctrl = pinum_ctrl;
		pins.datain = pinum_datain;
		pins.dataout = pinum_dataout;
		pins.clock = pinum_clock;
		pins.ctrlin = pinnum_ctrlin;
		pins.clockin = pinum_clockin;
		pins.poweron = pinum_poweron;
		pins.enbus = pinum_enbus;
	}
	virtual void ConfigPins(InterfPins p)
	{
		ConfigPins(p.ctrl, p.datain, p.dataout, p.clock, p.clockin, p.poweron, p.enbus, p.ctrlin);
	}

  protected:
	void Install(int val)
	{
		installed = val;
	}

	void DeInstall()
	{
		installed = -1;
	}

	int GetInstalled() const
	{
		return installed;
	}

	virtual int Flush()
	{
		return OK;
	}

	int old_portno;             // TestSave() save the status here
	VidPid usb_vp;
	Wait w;
	InterfPins pins;

  private:
	int installed;              // -1 --> not installed, >= 0 number if the installed port
	int cmd2cmd_delay;			// <> 0 if a delay between commands is needed
	unsigned int shot_delay;	//delay unit to perform bus timing
	bool i2c_mode;
};

#endif
