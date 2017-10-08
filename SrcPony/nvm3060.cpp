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

#include "types.h"
#include "nvm3060.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

#include "e2cmdw.h"

//=====>>> Costruttore <<<======
Nvm3060::Nvm3060(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	qDebug() << "Nvm3060::Nvm3060()";
}

//--- Distruttore
Nvm3060::~Nvm3060()
{
	qDebug() <<  "Nvm3060::~Nvm3060()";
}

// determina il numero di banchi (dimensione) dell'eeprom
//---
int Nvm3060::Probe(int probe_size)
{
	qDebug() << "Nvm3060::Probe(" << probe_size << ")";

	return OK;
}


int Nvm3060::Read(int probe, int type)
{
	qDebug() << "Nvm3060::Read(" << probe << ")";

	if (probe || GetNoOfBank() == 0)
	{
		Probe();
	}

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;

	if (type & PROG_TYPE)
	{
		int start_addr;

		if (size > 256)
		{
			//NVM3060
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{
			//MDA2062
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}

		GetBus()->UseSecondaryAddress(false);

		rv = GetBus()->Read(start_addr, GetBufPtr(), size);

		if (rv != size)
		{
			if (rv > 0)
			{
				rv = OP_ABORTED;
			}
		}
	}

	qDebug() << "Nvm3060::Read() = " << rv;

	return rv;
}

int Nvm3060::Write(int probe, int type)
{
	if (probe || GetNoOfBank() == 0)
	{
		Probe();
	}

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;

	if (type & PROG_TYPE)
	{
		int start_addr;

		if (size > 256)
		{
			//NVM3060
			E2Profile::SetNVMProgDelay(E2Profile::GetNVMProgDelay());
			GetBus()->SetProgDelay(E2Profile::GetNVMProgDelay());
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{
			//MDA2062
			E2Profile::SetMDAProgDelay(E2Profile::GetMDAProgDelay());
			GetBus()->SetProgDelay(E2Profile::GetMDAProgDelay());
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}

		GetBus()->UseSecondaryAddress(false);

		rv = GetBus()->Write(start_addr, GetBufPtr(), size);

		if (rv != size)
		{
			if (rv > 0)
			{
				rv = OP_ABORTED;
			}
		}
	}

	return rv;
}

int Nvm3060::Verify(int type)
{
	if (GetNoOfBank() == 0)
	{
		return BADPARAM;
	}

	int size = GetNoOfBank() * GetBankSize();

	int rval = 1;

	if (type & PROG_TYPE)
	{
		int start_addr;

		if (size > 256)
		{
			//NVM3060
			GetBus()->SetDataMode(16);
			GetBus()->SetStatusLocation(526);
			start_addr = 0;
		}
		else
		{
			//MDA2062
			GetBus()->SetDataMode(8);
			GetBus()->SetStatusLocation(14);
			start_addr = 128;
		}

		GetBus()->UseSecondaryAddress(false);

		unsigned char *localbuf;
		localbuf = new unsigned char[size];

		if (localbuf == 0)
		{
			return OUTOFMEMORY;
		}

		rval = GetBus()->Read(start_addr, localbuf, size);

		if (rval != size)
		{
			if (rval > 0)
			{
				rval = OP_ABORTED;
			}
		}
		else
		{
			rval = (memcmp(GetBufPtr(), localbuf, size) != 0) ? 0 : 1;
		}

		delete[] localbuf;
	}

	return rval;
}
