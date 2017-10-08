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

#ifndef _I2CINTERFACE_H
#define _I2CINTERFACE_H

#include "types.h"
#include "errcode.h"

#include <QDebug>


class BusInterface
{
  public:                //------------------------------- public
	BusInterface()
	{
		cmd2cmd_delay = 0;
		installed = -1;
		old_portno = -1;
	}
	//      virtual ~BusInterface();

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

  protected:             //------------------------------- protected
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

	int             old_portno;             // TestSave() save the status here

  private:               //------------------------------- private
	int             installed;              // -1 --> not installed, >= 0 number if the installed port
	int             cmd2cmd_delay;  // <> 0 if a delay between commands is needed
};

#endif
