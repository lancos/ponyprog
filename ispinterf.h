//=========================================================================//
//-------------------------------------------------------------------------//
// ispinterf.h -- Header for AvrISPInterface class                         //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
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

#ifndef	_AVRISPINTERFACE_H
#define	_AVRISPINTERFACE_H

#include "businter.h"
#include "lptinterf.h"
#include "lpt_io_interf.h"

class AvrISPInterface : public BusInterface
{
 public:		//------------------------------- public
	AvrISPInterface(int use_io = 0);
	virtual ~AvrISPInterface() { };

	virtual int Open(int com_no);
	virtual void Close();

	virtual void SetDataOut(int sda = 1);
	virtual void SetClock(int scl = 1);
	virtual int GetDataIn();
	virtual int GetClock();
	virtual void SetClockData();
	virtual void ClearClockData();
	virtual int IsClockDataUP();
	virtual int IsClockDataDOWN();

	virtual int TestPort(int port);

	int SetPower(int onoff);
	void SetControlLine(int res = 1);

	void SetIOmode(int use_io)
		{ io_mode = use_io; }

 protected:		//------------------------------- protected
	int InDataPort(int port_no = 0)
	{
		return io_mode ? lptio.InDataPort(port_no) : lpt.InDataPort(port_no);
	}
	int OutDataPort(int val, int port_no = 0)
	{
		return io_mode ? lptio.OutDataPort(val, port_no) : lpt.OutDataPort(val, port_no);
	}
	int OutControlPort(int val, int port_no = 0)
	{
		return io_mode ? lptio.OutControlPort(val, port_no) : lpt.OutControlPort(val, port_no);
	}
	int OutDataMask(int mask, int val)
	{
		return io_mode ? lptio.OutDataMask(mask, val) : lpt.OutDataMask(mask, val);
	}
	int OutControlMask(int mask, int val)
	{
		return io_mode ? lptio.OutControlMask(mask, val) : lpt.OutControlMask(mask, val);
	}
	int GetLastData() const
	{
		return io_mode ? lptio.GetLastData() : lpt.GetLastData();
	}
	int GetLastCtrl() const
	{
		return io_mode ? lptio.GetLastCtrl() : lpt.GetLastCtrl();
	}

 private:		//------------------------------- private
	int GetPresence();

	LPTInterface lpt;
	LPTIOInterface lptio;

	int io_mode;
};

#endif
