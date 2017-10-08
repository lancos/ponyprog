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
// 24XX-1 are I2C Bus eeproms that uses 8 bits (1 bytes) for addresses.    //
// Total capacity can be up to 256 bytes.                                  //

#include <QString>

#include "types.h"
#include "e2awinfo.h"
#include "e24xx-1.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

//=====>>> Costruttore <<<======
E24xx1::E24xx1(e2AppWinInfo *wininfo, BusIO *busp)
	:       E24xx(wininfo, busp, 8, 128)            //Max 8 bank of 128 bytes
{
	//      SetBankSize(1);
}

//--- Distruttore
E24xx1::~E24xx1()
{
}

int E24xx1::Probe(int probe_size)
{
	(void)probe_size;

	int rval = E24xx::Probe(0);

	if (rval > 0)
	{
		//      long size = GetSize();

		n_bank = 1;
		//      SetNoOfBank(n_bank);
	}

	return rval;
}

