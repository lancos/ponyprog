//=========================================================================//
//-------------------------------------------------------------------------//
// at89sxx.h -- Header for for At89sxx class                               //
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

#ifndef _AT89SXX_H
#define _AT89SXX_H

#include "types.h"

#include "at90sxx.h"
#include "at89sbus.h"

class At89sxx : public At90sxx
{
  public:		//---------------------------------------- public

	At89sxx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~At89sxx();

	int Probe(int probe_size = 0);
/**
	int Read(int probe = 1);
	int Write(int probe = 1);
	int Verify();
**/
	//Write fuse bits
	int SecurityWrite(int value, int notused);

	int Erase();

	At89sBus *GetBus()
		{ return (At89sBus *)EEProm::GetBus(); }

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};
#endif
