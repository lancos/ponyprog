//=========================================================================//
//-------------------------------------------------------------------------//
// at25xxx.cpp -- Source for At25xxx class (big SPI eeprom class)          //
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
#include "at25xxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

//#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At25xxx::At25xxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	At250xx(wininfo, busp)
{
}

//--- Distruttore
At25xxx::~At25xxx()
{
}

/**
// determina il numero di banchi (dimensione) dell'eeprom
//---
int At25xxx::Probe(int probe_size)
{
//	if (probe_size)
//	{
//		SetNoOfBank(n_bank);
//	}

	return OK;
}
**/

/**
int At25xxx::Read(int probe)
{
//	GetBus()->Reset();

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();

	int rv = GetBus()->Read(0, GetBufPtr(), size);
	if (rv != size)
	{
		if (rv > 0)
			rv = OP_ABORTED;
	}

	return rv;
}
**/

/**
int At25xxx::Write(int probe)
{
//	GetBus()->Reset();

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();
	int rv = GetBus()->Write(0, GetBufPtr(), size);
	if (rv != size)
	{
		if (rv > 0)
			rv = OP_ABORTED;
	}

	return rv;
}
**/

/**
int At25xxx::Verify()
 {
	if (GetNoOfBank() == 0)
		return BADPARAM;

//	GetBus()->Reset();

	int rval = -1;
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

	return rval;
}
**/