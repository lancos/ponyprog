//=========================================================================//
//-------------------------------------------------------------------------//
// atmegabus.h -- Header for for AtMegaBus class                           //
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

#ifndef	_ATMEGABUS_H
#define	_ATMEGABUS_H

#include "at90sbus.h"

class AtMegaBus : public At90sBus
{
 public:		//------------------------------- public
	AtMegaBus(BusInterface *ptr = 0);

//	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);
	
//	virtual int Reset();

//	int ReadDeviceCode(int addr);
//	int WriteFuseBits(int write_lock, int read_lock);

 protected:		//------------------------------- protected

	void WriteProgPage(long addr, UBYTE const *data, long page_size);

 private:		//------------------------------- private

	int CheckBlankPage(UBYTE const *data, ULONG length);
};

#endif
