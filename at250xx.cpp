//=========================================================================//
//-------------------------------------------------------------------------//
// at250xx.cpp -- Source for At250xx class (SPI eeprom class)              //
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

#include "types.h"
#include "at250xx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At250xx::At250xx(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
}

//--- Distruttore
At250xx::~At250xx()
{
}

// determina il numero di banchi (dimensione) dell'eeprom
//---
int At250xx::Probe(int probe_size)
{
	UserDebug1(UserApp1, "At250xx::Probe(%d)\n", probe_size);

//	if (probe_size)
//	{
//		SetNoOfBank(n_bank);
//	}

	return OK;
}


int At250xx::Read(int probe, int type)
{
	UserDebug1(UserApp1, "At250xx::Read(%d)\n", probe);

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();

	UserDebug1(UserApp1, "At250xx::Read() ** Size = %d\n", size);

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

	UserDebug1(UserApp1, "At250xx::Read() = %d\n", rv);

	return rv;
}

int At250xx::Write(int probe, int type)
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

int At250xx::Verify(int type)
{
	if (GetNoOfBank() == 0)
		return BADPARAM;

	int size = GetNoOfBank() * GetBankSize();
	unsigned char *localbuf;
	localbuf = new unsigned char[size];
	if (localbuf == 0)
		return OUTOFMEMORY;

	int rval = 1;
	if (type & PROG_TYPE)
	{
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
	}
	delete localbuf;

	return rval;
}
