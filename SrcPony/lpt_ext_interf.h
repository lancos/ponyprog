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

#ifndef _LPT_EXT_INTERFACE_H
#define _LPT_EXT_INTERFACE_H

#include "businter.h"
#include "lptinterf.h"
#include "lpt_io_interf.h"

class LptExtInterface : public BusInterface
{
  public:                //------------------------------- public
	LptExtInterface(bool use_io = false)
	{
		io_mode = use_io;
	}
	virtual ~LptExtInterface()
	{ }

	void Close()
	{
		lptio.Close();
		lpt.Close();
	}

	void SetIOmode(bool use_io)
	{
		io_mode = use_io;
	}

  protected:             //------------------------------- protected

	int InDataPort(int port_no = -1)
	{
		return io_mode ? lptio.InDataPort(port_no) : lpt.InDataPort(port_no);
	}

	int OutDataPort(int val, int port_no = -1)
	{
		return io_mode ? lptio.OutDataPort(val, port_no) : lpt.OutDataPort(val, port_no);
	}

	int OutControlPort(int val, int port_no = -1)
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

  private:               //------------------------------- private

	static LPTInterface lpt;
	static LPTIOInterface lptio;

	bool io_mode;
};

#endif
