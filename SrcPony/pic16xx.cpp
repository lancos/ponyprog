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
#include "pic16xx.h"            // Header file
#include "errcode.h"

#include "e2awinfo.h"

#define CONFIG_SIZE     ( 8 * sizeof(quint16) )

Pic16xx::Pic16xx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	int j;

	for (j = 0; j < 8; j++)
	{
		id_locations[j] = 0xffff;
	}
}

Pic16xx::~Pic16xx()
{
}

int Pic16xx::CodeProtectAdjust(quint16 &config, int read)
{
	if (!read)
	{
		//Extend the CP bit (PIC16F84)
		if (config & (1 << 4))
		{
			config |= 0xfff0;
		}
		else
		{
			config &= 0x000f;
		}
	}

	config = ~config & 0x3fff;

	return OK;
}

int Pic16xx::SecurityRead(quint32 &bits)
{
	int rv = GetBus()->ReadConfig(id_locations);

	if (rv == OK)
	{
		quint16 config = id_locations[7];

		CodeProtectAdjust(config, 1);

		bits = config;
	}

	return rv;
}

int Pic16xx::SecurityWrite(quint32 bits)
{
	quint16 config = (quint16)bits;

	CodeProtectAdjust(config, 0);

	int k;

	for (k = 0; k < 7; k++)
	{
		id_locations[k] = 0xffff;
	}

	id_locations[7] = config;

	return GetBus()->WriteConfig(id_locations);
}

int Pic16xx::Probe(int probe_size)
{
	(void)probe_size;
	return 1;
}

int Pic16xx::Read(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
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
			// read the config locations
			// this must be the LAST operation (to exit from config mode we have to clear Vpp)
			quint32 f;
			SecurityRead(f);
			//      GetAWInfo()->SetFuseBits(f0);
			GetAWInfo()->SetLockBits(f);
		}
	}

	return rv;
}

int Pic16xx::Write(int probe, int type)
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
				quint32 f;
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

int Pic16xx::Verify(int type)
{
	GetBus()->Reset();

	if (GetNoOfBank() == 0)
	{
		return BADPARAM;
	}

	int rval = -1;

	if (GetSize() >= GetSplitted())
	{
		unsigned char *localbuf = new unsigned char[GetSize()];

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
			quint32 f;
			SecurityRead(f);

			if (GetAWInfo()->GetLockBits() == f)
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

int Pic16xx::Erase(int probe, int type)
{
	int rv;

	//      rv = Probe( probe || GetNoOfBank() == 0 );
	//      if (rv > 0)
	//      {
	rv = GetBus()->Erase(type);
	//      }

	return rv == OK ? 1 : rv;
}
