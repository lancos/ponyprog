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
#include "e2awinfo.h"
#include "e2401.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

//=====>>> Costruttore <<<======
mE2401::mE2401(e2AppWinInfo *wininfo, BusIO *busp, int def_banksize)
	:	Device(wininfo, busp, def_banksize),
		timeout_loop(200),
		sequential_read(1),		// lettura di un banco alla volta
		writepage_size(1)		// scrittura di un byte alla volta (no page write)
{
}

//Determina a quali indirizzi I2C si mappa, e
// se probe_size == 1 anche la dimensione (numero di banchi)
//---
int mE2401::Probe(int probe_size)
{
	int retval = 1;
	uint8_t ch;

	UserDebug1(UserApp1, "mE2401::Probe(%d) - IN\n", probe_size);

	if (GetBus()->Read(0, &ch, 1) != 1)
	{
		retval = GetBus()->Error();

		if ( retval == IICERR_NOADDRACK )
			retval = 0;
	}

	UserDebug1(UserApp1, "mE2401::Probe(%d) - OUT\n", probe_size);

	return retval;
}


int mE2401::Read(int probe, int type)
{
	long size = GetSize();
	int error = Probe(probe);
	if (error < 0)
		return error;

	GetBus()->CheckAbort(0);

	if (type & PROG_TYPE)
	{
		if (sequential_read)
		{
			if (GetBus()->Read(0, GetBufPtr(), size) < size)
				return GetBus()->Error();
		}
		else
		{
			int k;

			for (k = 0; k < size; k++)
			{
				if (GetBus()->Read(k, GetBufPtr() + k, 1) != 1)
					return GetBus()->Error();

				if ( GetBus()->CheckAbort((k+1) * 100 / size) )
					return OP_ABORTED;
			}
		}
	}
	GetBus()->CheckAbort(100);

	return size;
}

int mE2401::Write(int probe, int type)
{
	long size = GetSize();
	int error = Probe(probe);
	if (error < 0)
		return error;

	GetBus()->CheckAbort(0);

	if (type & PROG_TYPE)
	{
		int j;
		uint8_t ch;
		for (j = 0; j < size; j += writepage_size)
		{
			if ( GetBus()->Write(j, GetBufPtr() + j, writepage_size) != writepage_size)
				return GetBus()->Error();

			int k;
			for (k = timeout_loop; k > 0 && GetBus()->Read(j, &ch, 1) != 1; k--)
				;
			if (k == 0)
				return E2P_TIMEOUT;

			if ( GetBus()->CheckAbort((j+1) * 100 / size) )
				return OP_ABORTED;
		}
	}
	GetBus()->CheckAbort(100);

	return size;
}

int mE2401::Verify(int type)
{
	Probe();

	long size = GetSize();
	unsigned char *localbuf = new unsigned char[size];

	if (localbuf == 0)
		return OUTOFMEMORY;

	GetBus()->CheckAbort(0);

	int rval = 1;
	if (type & PROG_TYPE)
	{
		if (sequential_read)
		{
			if (GetBus()->Read(0, localbuf, size) < size)
				return GetBus()->Error();
		}
		else
		{
			int k;

			for (k = 0; k < size; k++)
			{
				if (GetBus()->Read(k, localbuf + k, 1) != 1)
				{
					rval = GetBus()->Error();
					break;
				}

				if ( GetBus()->CheckAbort((k+1) * 100 / size) )
				{
					rval = OP_ABORTED;
					break;
				}
			}
		}

		if ( memcmp(GetBufPtr(), localbuf, size) != 0 )
			rval = 0;
	}
	GetBus()->CheckAbort(100);

	delete localbuf;
	return rval;
}
