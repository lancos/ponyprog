//=========================================================================//
//-------------------------------------------------------------------------//
// businter.h -- Header for BusInterface class                             //
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

#ifndef	_I2CINTERFACE_H
#define	_I2CINTERFACE_H

#include "types.h"
//#include "portint.h"
#include "errcode.h"

//#ifdef	vDEBUG
#include <v/vdebug.h>
//#endif

class BusInterface
{
 public:		//------------------------------- public
//	BusInterface();
//	virtual ~BusInterface();

	virtual int Open(int port) = 0;
	virtual void Close() = 0;

	virtual int TestOpen(int port)
	{
		UserDebug1(UserApp1, "BusInterface::TestPort(%d) IN\n", port);

		int ret_val = TestSave(port);

		TestRestore();

		UserDebug1(UserApp2, "BusInterface::TestPort() = %d OUT\n", ret_val);

		return ret_val;
	}
	virtual int TestPort(int port)
	{
		return TestOpen(port);
	}
	virtual int TestSave(int port)
	{
		int ret_val;

		UserDebug1(UserApp1, "BusInterface::TestSave(%d) IN\n", port);

		if ( (old_portno = installed) )
		{
//			old_cpreg = cpreg;
		}
		Close();
		if ( (ret_val = Open(port)) == OK )
		{
		}
		UserDebug1(UserApp2, "BusInterface::TestSave() = %d OUT\n", ret_val);

		return ret_val;
	}
	virtual void TestRestore()
	{
		UserDebug1(UserApp1, "BusInterface::TestRestore() IN *** Inst=%d\n", installed);

		if (installed)
			Close();
		if (old_portno)
		{
			Open(old_portno);
//			cpreg = old_cpreg;
		}
		old_portno = 0;

		UserDebug(UserApp2, "BusInterface::TestRestore() OUT\n");
	}

	virtual int SetPower(int onoff)
		{ return OK; }
	virtual void SetControlLine(int res = 1)
		{ }

	virtual void SetDataOut(int sda = 1) = 0;
	virtual void SetClock(int scl = 1) = 0;
	virtual int GetDataIn() = 0;
	virtual int GetClock() = 0;
	virtual void SetClockData() = 0;
	virtual void ClearClockData()
		{ }
	virtual int IsClockDataUP() = 0;
	virtual int IsClockDataDOWN() = 0;

	int IsInstalled() const
		{ return installed; }

 protected:		//------------------------------- protected
	void Install(int val)
		{ installed = val; }

	int		old_portno;		// TestSave() save the status here

 private:		//------------------------------- private
	int		installed;		// 0 --> not installed, <> 0 number if the installed port
};

#endif
