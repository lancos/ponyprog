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
#include "x2444.h"              // Header file
#include "errcode.h"
#include "eeptypes.h"


#include <QDebug>

//=====>>> Costruttore <<<======
X2444::X2444(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 2)
{
	qDebug() << "X2444::X2444()";
	DefaultBankSize();
}

void X2444::DefaultBankSize()
{
	if (GetBus() == 0)
	{
		Device::DefaultBankSize();
	}
	else
	{
		if (GetBus()->GetOrganization() == ORG16)
		{
			SetBankSize(2);
		}
		else
		{
			SetBankSize(1);
		}
	}
}

int X2444::Read(int probe, int type)
{
	qDebug() << "X2444::Read(" << probe << ")";

	if (probe || GetNoOfBank() == 0)
	{
		Probe();
	}

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;

	if (type & PROG_TYPE)
	{
		rv = GetBus()->Read(0, GetBufPtr(), size);

		if (rv != size)
		{
			if (rv > 0)
			{
				rv = OP_ABORTED;
			}
		}
	}

	qDebug() << "X2444::Read() = " << rv;

	return rv;
}

int X2444::Write(int probe, int type)
{
	if (probe || GetNoOfBank() == 0)
	{
		Probe();
	}

	int size = GetNoOfBank() * GetBankSize();

	int rv = size;

	if (type & PROG_TYPE)
	{
		rv = GetBus()->Write(0, GetBufPtr(), size);

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

int X2444::Verify(int type)
{
	if (GetNoOfBank() == 0)
	{
		return BADPARAM;
	}

	int rval = 1;

	if (type & PROG_TYPE)
	{
		int size = GetNoOfBank() * GetBankSize();
		unsigned char *localbuf;
		localbuf = new unsigned char[size];

		if (localbuf == 0)
		{
			return OUTOFMEMORY;
		}

		rval = GetBus()->Read(0, localbuf, size);

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
