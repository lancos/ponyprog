//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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
#include "pic168xx.h"           // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

#include "e2cmdw.h"
#include "e2awinfo.h"

Pic168xx::Pic168xx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Pic16xx(wininfo, busp)
{
}

Pic168xx::~Pic168xx()
{
}

int Pic168xx::CodeProtectAdjust(uint16_t &config, int read)
{
	quint32 type = 0;
	Pic168xx::QueryType(type);

	if (type == PIC1684A)
	{
		Pic16xx::CodeProtectAdjust(config, read);
	}
	else
	{
		config = ~config & 0x3fff;
	}

	return OK;
}


int Pic168xx::QueryType(quint32 &type)
{
	int rv = DEVICE_UNKNOWN;

	id_locations[6] = 0;

	if (GetBus()->ReadConfig(id_locations) == OK)
	{
		quint16 code = id_locations[6];

		qDebug() << "Pic168xx::ParseID() *** " << (hex) << code << (dec);

		code &= 0x3fe0;         //Strip revision number

		type = 0;

		quint16 pri_type = PIC168XX;
		foreach (groupElement g, GetAWInfo()->groupList)
		{
			if (g.vId.indexOf(pri_type) == -1)
			{
				continue;
			}

			foreach (icElement i, g.vChip)
			{
				if (i.sign == code)
				{
					type = i.id;
					rv = OK;
					return rv;
				}
			}
		}
	}

	return rv;
}

int Pic168xx::Probe(int probe_size)
{
	int rv = OK;
	quint32 type = 0;

	rv = QueryType(type);
//	int pritype = GetE2PPriType(type);
	quint32 subtype = GetAWInfo()->GetE2PSubType(type);

	if (probe_size)
	{
		if (rv == OK)
		{
			SetNoOfBank(GetAWInfo()->GetEEPTypeSize(type));
			SetSplitted(GetAWInfo()->GetEEPTypeSplit(type));
			rv = GetSize();
		}
	}
	else
	{
		if (cmdWin->GetIgnoreFlag())
		{
			rv = GetSize();
		}
		else
		{
			if (rv == OK)
			{
				if (GetAWInfo()->GetE2PSubType(GetAWInfo()->GetEEPId()) == subtype)
				{
					rv = GetSize();
				}
				else
				{
					rv = DEVICE_BADTYPE;
				}
			}
		}
	}

	return rv;
}

int Pic168xx::Write(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
	{
		if ((type & PROG_TYPE) && (type & DATA_TYPE))
		{
			GetBus()->Erase(ALL_TYPE);
		}
		else
		{
			GetBus()->Erase(type);
		}

		if (GetSize() >= GetSplitted())
		{
			if (type & PROG_TYPE)
			{
				rv = WriteProg();
			}

			if (rv > 0 && GetSize() > GetSplitted())        //check for DATA size
			{
				if (type & DATA_TYPE)
				{
					rv = WriteData();
				}
			}

			if (rv > 0 && (type & CONFIG_TYPE))
			{
				// write the config locations
				// this must be the LAST operation (to exit from config mode we have to clear Vpp)
				uint32_t f;
				f = GetAWInfo()->GetLockBits();
				SecurityWrite(f);
			}
		}
	}
	else if (rv == 0)
	{
		rv = E2ERR_WRITEFAILED;
	}

	return rv;
}
