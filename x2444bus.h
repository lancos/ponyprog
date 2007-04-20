//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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

#ifndef	_X2444BUS_H
#define	_X2444BUS_H

#include "microbus.h"

class X2444Bus : public MicroWireBus
{
 public:		//------------------------------- public
	X2444Bus(BusInterface *ptr = 0);

	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);

	virtual void SetOrganization(int org)
	{
		if (org == ORG8 || org == ORG16)
		{
			organization = org;
		}
	}
	virtual int GetOrganization() const
	{
		return organization;
	}

 protected:		//------------------------------- protected

	void SendCmdAddr(int cmd, int addr);

 private:		//------------------------------- private

	//Command Opcode
	const UBYTE ReadCode;
	const UBYTE WriteCode;
	const UBYTE WriteEnableCode;
	const UBYTE WriteDisableCode;
	const UBYTE RecallCode;
	const UBYTE StoreCode;

	const long loop_timeout;

	void setCS()
		{ SetReset(); }

	void clearCS()
		{ ClearReset(); }

//	int address_len;
	int organization;
};

#endif
