//=========================================================================//
//-------------------------------------------------------------------------//
// lpt_io_interf.h -- Header for LPTIOInterface class                      //
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

#ifndef	_LPTIOINTERFACE_H
#define	_LPTIOINTERFACE_H

#include "types.h"
#include "portint.h"

class LPTIOInterface : public PortInterface
{
 public:		//------------------------------- public
	LPTIOInterface();
	virtual ~LPTIOInterface();

	void Close()
	{
		lpt_port = 0;
		PortInterface::CloseParallel();
	}

	void SetPort(int port_no);
	int InDataPort(int port_no = 0);
	int OutDataPort(int val, int port_no = 0);
	int OutControlPort(int val, int port_no = 0);
	int OutDataMask(int mask, int val);
	virtual int OutControlMask(int mask, int val);

	int GetLastData() const
		{ return last_data; }
	int GetLastCtrl() const
		{ return last_ctrl; }

 protected:		//------------------------------- protected

 private:		//------------------------------- private
 
	int lpt_port;
	UBYTE last_ctrl, last_data;
};

#endif
