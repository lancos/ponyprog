//=========================================================================//
//-------------------------------------------------------------------------//
// sde2506.cpp -- Source for Sde2506 class                                 //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2001   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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
#include "sde2506.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
Sde2506::Sde2506(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "Sde2506::Sde2506() constructor\n");
}

//--- Distruttore
Sde2506::~Sde2506()
{
	UserDebug(Destructor, "Sde2506::~Sde2506() destructor\n");
}

// determina il numero di banchi (dimensione) dell'eeprom
//---
int Sde2506::Probe(int probe_size)
{
	UserDebug1(UserApp1, "Sde2506::Probe(%d)\n", probe_size);

	return OK;
}


int Sde2506::Read(int probe, int type)
{
	UserDebug1(UserApp1, "Sde2506::Read(%d)\n", probe);

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;
	if (type & PROG_TYPE)
	{
		rv = GetBus()->Read(0, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}
	UserDebug1(UserApp1, "Sde2506::Read() = %d\n", rv);

	return rv;
}

int Sde2506::Write(int probe, int type)
{
	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();
	int rv = size;
	if (type & PROG_TYPE)
	{
		rv = GetBus()->Write(0, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}

	return rv;
}

int Sde2506::Verify(int type)
 {
	if (GetNoOfBank() == 0)
		return BADPARAM;

	int rval = 1;
	if (type & PROG_TYPE)
	{
		int size = GetNoOfBank() * GetBankSize();
		unsigned char *localbuf;
		localbuf = new unsigned char[size];
		if (localbuf == 0)
			return OUTOFMEMORY;

		rval = GetBus()->Read(0, localbuf, size);
		if (rval != size)
		{
			if (rval > 0)
				rval = OP_ABORTED;
		}
		else
		{
			rval = ( memcmp(GetBufPtr(), localbuf, size) != 0 ) ? 0 : 1;
		}
		delete localbuf;
	}

	return rval;
}
