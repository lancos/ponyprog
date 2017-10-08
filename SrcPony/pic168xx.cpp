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
#include "pic168xx.h"           // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

#include "e2cmdw.h"
#include "e2awinfo.h"

//=====>>> Costruttore <<<======
Pic168xx::Pic168xx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Pic16xx(wininfo, busp)
{
}

//--- Distruttore
Pic168xx::~Pic168xx()
{
}

int Pic168xx::CodeProtectAdjust(uint16_t &config, int read)
{
	long type = 0;
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

typedef struct
{
	int code;
	int type;
} IdType;

static IdType IdArray[] =
{
	{0x560, PIC1684A},      // 00 0101 011 0 0000

	{0xd00, PIC16870},      // 00 1101 000 0 0000
	{0xd20, PIC16871},      // 00 1101 001 0 0000
	{0x8e0, PIC16872},      // 00 1000 111 0 0000

	{0x960, PIC16873},      // 00 1001 011 0 0000
	{0x920, PIC16874},      // 00 1001 001 0 0000
	{0x9e0, PIC16876},      // 00 1001 111 0 0000
	{0x9a0, PIC16877},      // 00 1001 101 0 0000

	{0xe40, PIC16873A},     // 00 1110 0100 XXXX
	{0xe60, PIC16874A},     // 00 1110 0110 XXXX
	{0xe00, PIC16876A},     // 00 1110 0000 XXXX
	{0xe20, PIC16877A},     // 00 1110 0010 XXXX

	{0x7a0, PIC16627},      // 00 0111 101 x xxxx
	{0x7c0, PIC16628},      // 00 0111 110 x xxxx

	{0x00, PIC16800}
};

int Pic168xx::QueryType(long &type)
{
	int rv = DEVICE_UNKNOWN;

	id_locations[6] = 0;

	if (GetBus()->ReadConfig(id_locations) == OK)
	{
		int code = id_locations[6];

		qDebug() << "Pic168xx::ParseID() *** " << (hex) << code << (dec);

		code &= 0x3fe0;         //Strip revision number

		type = 0;
		int k;

		for (k = 0; IdArray[k].code != 0x00; k++)
		{
			if (IdArray[k].code == code)
			{
				type = IdArray[k].type;
				rv = OK;
				break;
			}
		}
	}

	return rv;
}

int Pic168xx::Probe(int probe_size)
{
	int rv = OK;
	long type;

	rv = QueryType(type);
//	int pritype = GetE2PPriType(type);
	int subtype = GetE2PSubType(type);

	if (probe_size)
	{
		if (rv == OK)
		{
			SetNoOfBank(GetEEPTypeSize(type));
			SetSplitted(GetEEPTypeSplit(type));
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
				if (GetE2PSubType(GetAWInfo()->GetEEPId()) == subtype)
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
