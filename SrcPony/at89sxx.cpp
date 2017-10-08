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
// At89sxx Class (Atmel 8051 device class)

#include "types.h"
#include "at89sxx.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2cmdw.h"

#include <QDebug>

//=====>>> Costruttore <<<======
At89sxx::At89sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	qDebug() << "At89sxx::At89sxx()";
}

int At89sxx::SecurityRead(uint32_t &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->ReadLockBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

int At89sxx::SecurityWrite(uint32_t bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->WriteLockBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

int At89sxx::FusesRead(uint32_t &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->ReadFuseBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

int At89sxx::FusesWrite(uint32_t bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->WriteFuseBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

int At89sxx::QueryType(long &type)
{
	int rv;
	int code[3];

	code[0] = GetBus()->ReadDeviceCode(0x30);
	code[1] = GetBus()->ReadDeviceCode(0x31);

	qDebug() << "At89sxx::ParseID(30) *** " << (hex) << code[0] << " - " << code[1] << (dec);

	type = 0;

	if (code[0] == 0x1E && code[1] == 0x72)
	{
		type = AT89S8252;
		rv = OK;
	}
	else if (code[0] == 0x1E && code[1] == 0x73)
	{
		type = AT89S8253;
		rv = OK;
	}
	else
	{
		code[0] = GetBus()->ReadDeviceCode(0x000);
		code[1] = GetBus()->ReadDeviceCode(0x100);
		code[2] = GetBus()->ReadDeviceCode(0x200);

		qDebug() << "At89sxx::ParseID(100) *** " << (hex) << code[0] << " - " << code[1] << " - " <<  code[2] << (dec);

		if (code[0] == 0x1E && code[1] == 0x51 && code[2] == 0x06)
		{
			type = AT89S51;
			rv = OK;
		}
		else if (code[0] == 0x1E && code[1] == 0x52 && code[2] == 0x06)
		{
			type = AT89S52;
			rv = OK;
		}
		else
		{
			rv = DEVICE_UNKNOWN;
		}
	}

	return rv;
}

int At89sxx::Probe(int probe_size)
{
	int rv = OK;

	qDebug() << "At89sxx::Probe(" << probe_size << ") IN";

	if (cmdWin->GetIgnoreFlag())
	{
		rv = GetSize();
	}
	else
	{
		switch (GetAWInfo()->GetEEPId())
		{
		case AT89S51:
		case AT89S52:
		case AT89S8253:
		{
			long type;
			rv = QueryType(type);
			int subtype = GetE2PSubType(type);

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
		break;

		default:
			rv = GetSize();
			break;
		}
	}

	qDebug() << "At89sxx::Probe() = " << rv << " **  OUT";

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
			{
				rv = ReadProg();
			}

			if (rv > 0 && GetSize() > GetSplitted())        //Check for DATA size
			{
				if (type & DATA_TYPE)
				{
					rv = ReadData();
				}
			}

			if (rv > 0 && (type & CONFIG_TYPE))
			{
				// read the fuses
				uint32_t f = 0;

				if (GetBus()->ReadFuseBits(f, GetAWInfo()->GetEEPId()) == OK)
				{
					GetAWInfo()->SetFuseBits(f);
				}

				f = 0;

				if (GetBus()->ReadLockBits(f, GetAWInfo()->GetEEPId()) == OK)
				{
					GetAWInfo()->SetLockBits(f);
				}
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
				//write the fuses
				uint32_t f = GetAWInfo()->GetFuseBits();
				GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPId());

				//write the locks
				f = GetAWInfo()->GetLockBits();
				GetBus()->WriteLockBits(f,  GetAWInfo()->GetEEPId());
			}
		}
	}

	return rv;
}

int At89sxx::Verify(int type)
{
	if (GetSize() == 0)
	{
		return BADPARAM;
	}

	int rval = -1;

	if (GetSize() >= GetSplitted())
	{
		unsigned char *localbuf;
		localbuf = new unsigned char[GetSize()];

		if (localbuf == 0)
		{
			return OUTOFMEMORY;
		}

		int v_data = OK, v_prog = OK, v_config = OK;

		if (type & PROG_TYPE)
		{
			v_prog = VerifyProg(localbuf);
		}

		if (type & DATA_TYPE)
		{
			v_data = VerifyData(localbuf);
		}

		if (type & CONFIG_TYPE)
		{
			uint32_t fval, lval;
			int fret, lret;

			// read the fuses & locks
			fret = GetBus()->ReadFuseBits(fval, GetAWInfo()->GetEEPId());
			lret = GetBus()->ReadLockBits(lval, GetAWInfo()->GetEEPId());

			if ((lret == NOTSUPPORTED || GetAWInfo()->GetLockBits() == lval)
					&& (fret == NOTSUPPORTED || GetAWInfo()->GetFuseBits() == fval))
			{
				v_config = OK;
			}
			else
			{
				v_config = 1;
			}
		}

		rval = (v_prog == OK && v_data == OK && v_config == OK) ? 1 : 0;

		delete[] localbuf;
	}

	return rval;
}
