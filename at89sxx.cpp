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
// At89sxx Class (Atmel 8051 device class)

#include "types.h"
#include "at89sxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2awinfo.h"

#undef	BANK_SIZE
#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At89sxx::At89sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "At89sxx::At89sxx()\n");
}

int At89sxx::SecurityRead(DWORD &bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv != DEVICE_UNKNOWN)
	{
		rv = GetBus()->ReadLockBits(bits, GetAWInfo()->GetEEPType());
	}
	return rv;
}

int At89sxx::SecurityWrite(DWORD bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv != DEVICE_UNKNOWN)
	{
		rv = GetBus()->WriteLockBits(bits, GetAWInfo()->GetEEPType());
	}
	return rv;
}

int At89sxx::FusesRead(DWORD &bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv != DEVICE_UNKNOWN)
	{
		rv = GetBus()->ReadFuseBits(bits, GetAWInfo()->GetEEPType());
	}
	return rv;
}

int At89sxx::FusesWrite(DWORD bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv != DEVICE_UNKNOWN)
	{
		rv = GetBus()->WriteFuseBits(bits, GetAWInfo()->GetEEPType());
	}
	return rv;
}

int At89sxx::QueryType(long &type)
{
	int rv;
	int code[3];

	code[0] = GetBus()->ReadDeviceCode(0x30);
	code[1] = GetBus()->ReadDeviceCode(0x31);

	UserDebug2(UserApp2, "At89sxx::ParseID(30) *** 0x%02X - 0x%02X\n", code[0], code[1]);

	type = 0;
	if (code[0] == 0x1E && code[1] == 0x72)
	{
		type = AT89S8252;
		rv = OK;
	}
	else
	if (code[0] == 0x1E && code[1] == 0x73)
	{
		type = AT89S8253;
		rv = OK;
	}
	else
	{
		code[0] = GetBus()->ReadDeviceCode(0x000);
		code[1] = GetBus()->ReadDeviceCode(0x100);
		code[2] = GetBus()->ReadDeviceCode(0x200);

		UserDebug3(UserApp2, "At89sxx::ParseID(100) *** 0x%02X - 0x%02X - 0x%02X\n", code[0], code[1], code[2]);

		if (code[0] == 0x1E && code[1] == 0x51 && code[2] == 0x06)
		{
			type = AT89S51;
			rv = OK;
		}
		else
		if (code[0] == 0x1E && code[1] == 0x52 && code[2] == 0x06)
		{
			type = AT89S52;
			rv = OK;
		}
		else
			rv = DEVICE_UNKNOWN;
	}

	return rv;
}

int At89sxx::Probe(int probe_size)
{
	int rv = OK;

	UserDebug1(UserApp2, "At89sxx::Probe(%d) IN\n", probe_size);

	if (THEAPP->GetIgnoreFlag())
	{
		rv = GetSize();
	}
	else
	{
		switch (GetAWInfo()->GetEEPType())
		{
		case AT89S51: case AT89S52:
		case AT89S8253:
			{
				long type;
				rv = QueryType(type);
				int subtype = GetE2PSubType(type);

				if (rv == OK)
				{
					if ( GetAWInfo()->GetEEPSubType() == subtype )
						rv = GetSize();
					else
						rv = DEVICE_BADTYPE;
				}
			}
			break;
		default:
			rv = GetSize();
			break;
		}
	}
	UserDebug1(UserApp2, "At89sxx::Probe() = %d **  OUT\n", rv);

	return rv;
}

int At89sxx::Read(int probe, int type)
{
	int rv = Probe();

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			if (type & PROG_TYPE)
				rv = ReadProg();
			if (rv > 0 && GetSize() > GetSplitted())	//Check for DATA size
			{
				if (type & DATA_TYPE)
					rv = ReadData();
			}
			if (rv > 0 && (type & CONFIG_TYPE))
			{
				// read the fuses
				DWORD f = 0;
				if ( GetBus()->ReadFuseBits(f, GetAWInfo()->GetEEPType()) == OK )
					GetAWInfo()->SetFuseBits(f);
				f = 0;
				if ( GetBus()->ReadLockBits(f, GetAWInfo()->GetEEPType()) == OK )
					GetAWInfo()->SetLockBits(f);
			}
		}
	}

	return rv;
}

int At89sxx::Write(int probe, int type)
{
	int rv = Probe();

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			if (type & PROG_TYPE)
				rv = WriteProg();
			if (rv > 0 && GetSize() > GetSplitted())	//check for DATA size
			{
				if (type & DATA_TYPE)
					rv = WriteData();
			}
			if (rv > 0 && (type & CONFIG_TYPE))
			{
				//write the fuses
				DWORD f = GetAWInfo()->GetFuseBits();
				GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPType());

				//write the locks
				f = GetAWInfo()->GetLockBits();
				GetBus()->WriteLockBits(f,  GetAWInfo()->GetEEPType());
			}
		}
	}
	return rv;
}

int At89sxx::Verify(int type)
{
	if (GetSize() == 0)
		return BADPARAM;

	int rval = -1;
	if (GetSize() >= GetSplitted())
	{
		unsigned char *localbuf;
		localbuf = new unsigned char[GetSize()];
		if (localbuf == 0)
			return OUTOFMEMORY;

		int v_data = OK, v_prog = OK, v_config = OK;

		if (type & PROG_TYPE)
			v_prog = VerifyProg(localbuf);
		if (type & DATA_TYPE)
			v_data = VerifyData(localbuf);
		if (type & CONFIG_TYPE)
		{
			DWORD fval, lval;
			int fret, lret;

			// read the fuses & locks
			fret = GetBus()->ReadFuseBits(fval, GetAWInfo()->GetEEPType());
			lret = GetBus()->ReadLockBits(lval, GetAWInfo()->GetEEPType());

			if ( (lret == NOTSUPPORTED || GetAWInfo()->GetLockBits() == lval)
				&& (fret == NOTSUPPORTED || GetAWInfo()->GetFuseBits() == fval) )
			{
				v_config = OK;
			}
			else
				v_config = 1;
		}
		rval = (v_prog == OK && v_data == OK && v_config == OK) ? 1 : 0;

		delete localbuf;
	}
	return rval;
}
