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

#ifndef _I2CINTERFACE_H
#define _I2CINTERFACE_H

#include "types.h"
#include "errcode.h"
#include "wait.h"

#include <QDebug>

class BusInterface
{
  public:
	BusInterface()
		: old_portno(-1),
		  installed(-1),
		  cmd2cmd_delay(0),
		  shot_delay(5)
	{
	}

	virtual int Open(int port) = 0;
	virtual void Close() = 0;

	virtual int TestOpen(int port)
	{
		qDebug() << "BusInterface::TestOpen(" << port << ") IN";

		int ret_val = TestSave(port);

		TestRestore();

		qDebug() << "BusInterface::TestOpen() = " << ret_val << " OUT";

		return ret_val;
	}
	virtual int TestPort(int port)
	{
		qDebug() << "BusInterface::TestPort(" << port << ") IN";

		return TestOpen(port);
	}
	virtual int TestSave(int port)
	{
		int ret_val;

		qDebug() << "BusInterface::TestSave(" << port << ") IN";

		old_portno = installed;

		Close();

		if ((ret_val = Open(port)) == OK)
		{
		}

		qDebug() << "BusInterface::TestSave() = " << ret_val << " OUT";

		return ret_val;
	}
	virtual void TestRestore()
	{
		qDebug() << "BusInterface::TestRestore() IN *** Inst=" << installed;

		if (IsInstalled())
		{
			Close();
		}

		if (old_portno >= 0)
		{
			Open(old_portno);
			old_portno = -1;
		}

		qDebug() << "BusInterface::TestRestore() OUT";
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

	virtual void WaitMsec(unsigned int msec)
	{
		w.WaitMsec(msec);
	}

	virtual void WaitUsec(unsigned int usec)
	{
		w.WaitUsec(usec);
	}

	virtual void ShotDelay(int n = 1)
	{
		w.WaitUsec(shot_delay * n);
	}

	virtual int SPI_xferBit(int b, int mode = 0)
	{
		int ret = 0;

		switch (mode)
		{
		case 3:
			SetClock(0);
			SetDataOut(b);
			ShotDelay();
			SetClock(1);
			ret = GetDataIn();
			ShotDelay();
			break;
		case 2:
			SetDataOut(b);
			ShotDelay();
			SetClock(0);
			ret = GetDataIn();
			ShotDelay();
			SetClock(1);
			break;
		case 1:
			SetClock(1);
			SetDataOut(b);
			ShotDelay();
			SetClock(0);
			ret = GetDataIn();
			ShotDelay();
			break;
		case 0:
		default:
			SetDataOut(b);
			ShotDelay();
			SetClock(1);
			ret = GetDataIn();
			ShotDelay();
			SetClock(0);
			break;
		}
		return ret;
	}

	virtual unsigned long SPI_xferWord(unsigned long word_out, int mode = 0, int bpw = 8, bool lsb_first = false)
	{
		uint32_t word_in = 0;
		uint32_t bitmask;

		switch (mode)
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

		if (lsb_first)
			bitmask = 1;
		else
			bitmask = 1 << (bpw - 1);

		for (int k = 0; k < bpw; k++)
		{
			if (SPI_xferBit(word_out & bitmask, mode))
				word_in |= bitmask;

			if (lsb_first)
				bitmask <<= 1;
			else
				bitmask >>= 1;
		}
		SetDataOut(1);

		return word_in;
	}

	void SetDelay(int delay)
	{
		if (delay >= 0)
			shot_delay = delay;
	}
	int GetDelay() const
	{
		return shot_delay;
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

	int old_portno;             // TestSave() save the status here

  private:
	int installed;              // -1 --> not installed, >= 0 number if the installed port
	int cmd2cmd_delay;			// <> 0 if a delay between commands is needed
	unsigned int shot_delay;	//delay unit to perform bus timing

	Wait w;
};

#endif
