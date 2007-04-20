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

#include "types.h"
#include "nvm3060.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2app.h"

#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
Nvm3060::Nvm3060(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "Nvm3060::Nvm3060() constructor\n");
}

//--- Distruttore
Nvm3060::~Nvm3060()
{
	UserDebug(Destructor, "Nvm3060::~Nvm3060() destructor\n");
}

// determina il numero di banchi (dimensione) dell'eeprom
//---
int Nvm3060::Probe(int probe_size)
{
	UserDebug1(UserApp1, "Nvm3060::Probe(%d)\n", probe_size);

	return OK;
}


int Nvm3060::Read(int probe, int type)
{
	UserDebug1(UserApp1, "Nvm3060::Read(%d)\n", probe);

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;
	if (type & PROG_TYPE)
	{
		int start_addr;
		if (size > 256)
		{	//NVM3060
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{	//MDA2062
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}
		GetBus()->UseSecondaryAddress(false);

		rv = GetBus()->Read(start_addr, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}

	UserDebug1(UserApp1, "Nvm3060::Read() = %d\n", rv);

	return rv;
}

int Nvm3060::Write(int probe, int type)
{
	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;
	if (type & PROG_TYPE)
	{
		int start_addr;
		if (size > 256)
		{	//NVM3060
			THEAPP->SetNVMProgDelay( THEAPP->GetNVMProgDelay() );
			GetBus()->SetProgDelay( THEAPP->GetNVMProgDelay() );
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{	//MDA2062
			THEAPP->SetMDAProgDelay( THEAPP->GetMDAProgDelay() );
			GetBus()->SetProgDelay( THEAPP->GetMDAProgDelay() );
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}
		GetBus()->UseSecondaryAddress(false);

		rv = GetBus()->Write(start_addr, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}

	return rv;
}

int Nvm3060::Verify(int type)
 {
	if (GetNoOfBank() == 0)
		return BADPARAM;

	int size = GetNoOfBank() * GetBankSize();

	int rval = 1;
	if (type & PROG_TYPE)
	{
		int start_addr;
		if (size > 256)
		{	//NVM3060
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{	//MDA2062
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}
		GetBus()->UseSecondaryAddress(false);

		unsigned char *localbuf;
		localbuf = new unsigned char[size];
		if (localbuf == 0)
			return OUTOFMEMORY;

		rval = GetBus()->Read(start_addr, localbuf, size);
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
