//=========================================================================//
//-------------------------------------------------------------------------//
// at89sxx.cpp -- Source for At89sxx Class (Atmel 8051 device class)       //
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

#include "types.h"
#include "at89sxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

//#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At89sxx::At89sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	At90sxx(wininfo, busp)
{
}

//--- Distruttore
At89sxx::~At89sxx()
{
}

int At89sxx::Probe(int probe_size)
{
	return GetSize();
}

int At89sxx::Read(int probe)
{
	return At90sxx::Read(probe);
}

int At89sxx::Write(int probe)
{
	return At90sxx::Write(probe);
}

int At89sxx::Verify()
{
	return At90sxx::Verify();
}
