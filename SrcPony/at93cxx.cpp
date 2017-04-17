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
#include "at93cxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#define	BANK_SIZE	2		//16 bit organization

//=====>>> Costruttore <<<======
At93cxx::At93cxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "At93cxx::At93cxx()\n");
}

//--- Distruttore
At93cxx::~At93cxx()
{
	UserDebug(Destructor, "At93cxx::~At93cxx()\n");
}

int At93cxx::Read(int probe, int type)
{
	UserDebug1(UserApp1, "At93cxx::Read(%d)\n", probe);

	GetBus()->SetOrganization(ORG16);

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();
	int asize = GetBus()->CalcAddressSize(GetAddrSize());

	int rv = size;
	if (type & PROG_TYPE)
	{
		rv = GetBus()->Read(asize, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}
	UserDebug1(UserApp1, "At93cxx::Read() = %d\n", rv);

	return rv;
}

int At93cxx::Write(int probe, int type)
{
	GetBus()->SetOrganization(ORG16);

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();
	int asize = GetBus()->CalcAddressSize(GetAddrSize());

	int rv = size;
	if (type & PROG_TYPE)
	{
		rv = GetBus()->Write(asize, GetBufPtr(), size);
		if (rv != size)
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
	}

	return rv;
}

int At93cxx::Verify(int type)
{
	GetBus()->SetOrganization(ORG16);

	if (GetNoOfBank() == 0)
		return BADPARAM;

	int size = GetNoOfBank() * GetBankSize();
	int asize = GetBus()->CalcAddressSize(GetAddrSize());
	unsigned char *localbuf;
	localbuf = new unsigned char[size];
	if (localbuf == 0)
		return OUTOFMEMORY;

	int rval = 1;
	if (type & PROG_TYPE)
	{
		rval = GetBus()->Read(asize, localbuf, size);
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
