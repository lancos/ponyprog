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
#include "e2awinfo.h"
#include "e2401.h"              // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

//=====>>> Costruttore <<<======
mE2401::mE2401(e2AppWinInfo *wininfo, BusIO *busp, int def_banksize)
	:       Device(wininfo, busp, def_banksize),
			timeout_loop(200),
			sequential_read(1),             // lettura di un banco alla volta
			writepage_size(1)               // scrittura di un byte alla volta (no page write)
{
}

//Determina a quali indirizzi I2C si mappa, e
// se probe_size == 1 anche la dimensione (numero di banchi)
//---
int mE2401::Probe(int probe_size)
{
	int retval = 1;
	uint8_t ch;

	qDebug() << "mE2401::Probe(" << probe_size << ") - IN";

	if (GetBus()->Read(0, &ch, 1) != 1)
	{
		retval = GetBus()->Error();

		if (retval == IICERR_NOADDRACK)
		{
			retval = 0;
		}
	}

	qDebug() << "mE2401::Probe(" << probe_size << ") - OUT";

	return retval;
}


int mE2401::Read(int probe, int type)
{
	long size = GetSize();
	int error = Probe(probe);

	if (error < 0)
	{
		return error;
	}

	GetBus()->ReadStart();

	if (type & PROG_TYPE)
	{
		if (sequential_read)
		{
			if (GetBus()->Read(0, GetBufPtr(), size) < size)
			{
				return GetBus()->Error();
			}
		}
		else
		{
			int k;

			for (k = 0; k < size; k++)
			{
				if (GetBus()->Read(k, GetBufPtr() + k, 1) != 1)
				{
					return GetBus()->Error();
				}

				if (GetBus()->ReadProgress((k + 1) * 100 / size))
				{
					return OP_ABORTED;
				}
			}
		}
	}

	GetBus()->ReadEnd();

	return size;
}

int mE2401::Write(int probe, int type)
{
	long size = GetSize();
	int error = Probe(probe);

	if (error < 0)
	{
		return error;
	}

	GetBus()->WriteStart();

	if (type & PROG_TYPE)
	{
		int j;
		uint8_t ch;

		for (j = 0; j < size; j += writepage_size)
		{
			if (GetBus()->Write(j, GetBufPtr() + j, writepage_size) != writepage_size)
			{
				return GetBus()->Error();
			}

			int k;

			for (k = timeout_loop; k > 0 && GetBus()->Read(j, &ch, 1) != 1; k--)
				;

			if (k == 0)
			{
				return E2P_TIMEOUT;
			}

			if (GetBus()->WriteProgress((j + 1) * 100 / size))
			{
				return OP_ABORTED;
			}
		}
	}

	GetBus()->WriteEnd();

	return size;
}

int mE2401::Verify(int type)
{
	Probe();

	long size = GetSize();
	unsigned char *localbuf = new unsigned char[size];

	if (localbuf == 0)
	{
		return OUTOFMEMORY;
	}

	GetBus()->ReadStart();

	int rval = 1;

	if (type & PROG_TYPE)
	{
		if (sequential_read)
		{
			if (GetBus()->Read(0, localbuf, size) < size)
			{
				return GetBus()->Error();
			}
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

				if (GetBus()->ReadProgress((k + 1) * 100 / size))
				{
					rval = OP_ABORTED;
					break;
				}
			}
		}

		if (memcmp(GetBufPtr(), localbuf, size) != 0)
		{
			rval = 0;
		}
	}

	GetBus()->ReadEnd();

	delete[] localbuf;
	return rval;
}
