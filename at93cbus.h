//=========================================================================//
//-------------------------------------------------------------------------//
// at93cbus.h -- Header for for At93cBus class                             //
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

#ifndef	_AT93CBUS_H
#define	_AT93CBUS_H

#include "microbus.h"

class At93cBus : public MicroWireBus
{
 public:		//------------------------------- public
	At93cBus(BusInterface *ptr = 0);
//	virtual ~At93cBus();

	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);
	
 protected:		//------------------------------- protected

 private:		//------------------------------- private

	//Command Opcode
	const UBYTE ReadCode;
	const UBYTE WriteCode;
	const UBYTE WriteEnableCode;

	const long loop_timeout;
	
	void setCS()
		{ SetReset(); }		//27/05/98

	void clearCS()
		{ ClearReset(); }	//27/05/98

	int address_len;
};

#endif
