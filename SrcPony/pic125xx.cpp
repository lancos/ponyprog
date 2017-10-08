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
#include "pic125xx.h"           // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

#include "e2awinfo.h"

#define CONFIG_SIZE     ( 8 * sizeof(uint16_t) )

//=====>>> Costruttore <<<======
Pic125xx::Pic125xx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	config_word = 0xffff;
}

//--- Distruttore
Pic125xx::~Pic125xx()
{
}

int Pic125xx::CodeProtectAdjust(uint16_t &config, int read)
{
	config = ~config & 0x0fff;

	return OK;
}

int Pic125xx::SecurityRead(uint32_t &bits)
{
	int rv = GetBus()->ReadConfig(config_word);

	if (rv == OK)
	{
		uint16_t config = config_word;
		CodeProtectAdjust(config, 1);

		bits = config;
	}

	return rv;
}

int Pic125xx::SecurityWrite(uint32_t bits)
{
	uint16_t config = (uint16_t)bits;

	CodeProtectAdjust(config, 0);

	config_word = config;

	return GetBus()->WriteConfig(config_word);
}

int Pic125xx::Probe(int probe_size)
{
	return 1;
}

int Pic125xx::Read(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
	{
		if (type & CONFIG_TYPE)
		{
			// read the config locations
			// this must be the FIRST operation (just after reset)
			uint32_t f;
			SecurityRead(f);
			GetAWInfo()->SetLockBits(f);
		}
		else
		{
			//Skip configuration word
			GetBus()->IncAddress(1);
		}

		if (type & PROG_TYPE)
		{
			rv = ReadProg();
		}
	}

	return rv;
}

int Pic125xx::Write(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

	if (rv > 0)
	{
		if (type & PROG_TYPE)
		{
			//Skip configuration word
			GetBus()->IncAddress(1);
			rv = WriteProg();
		}

		if (rv > 0 && (type & CONFIG_TYPE))
		{
			// write the config locations
			// this must be the FIRST operation (just after reset)
			uint32_t f;

			GetBus()->Reset();

			f = GetAWInfo()->GetLockBits();
			SecurityWrite(f);
		}
	}
	else if (rv == 0)
	{
		rv = E2ERR_WRITEFAILED;
	}

	return rv;
}

int Pic125xx::Verify(int type)
{
	GetBus()->Reset();

	if (GetNoOfBank() == 0)
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

		int v_prog = OK, v_config = OK;

		if (type & CONFIG_TYPE)
		{
			uint32_t f;
			SecurityRead(f);

			qDebug() << "Pic125xx::Verify() ** " << f << " <-> " << (unsigned long)GetAWInfo()->GetLockBits();

			if (GetAWInfo()->GetLockBits() == f)
			{
				v_config = OK;
			}
			else
			{
				v_config = 1;
			}
		}
		else
		{
			//Skip configuration word
			GetBus()->IncAddress(1);
		}

		if (type & PROG_TYPE)
		{
			v_prog = VerifyProg(localbuf);
		}

		rval = (v_prog == OK && v_config == OK) ? 1 : 0;

		delete[] localbuf;
	}

	return rval;
}

int Pic125xx::VerifyProg(unsigned char *localbuf)
{
	int rval = -1;
	int size = GetSplitted();
	int base = 0;

	//Verify only programmed bytes (to save time in big devices)
	//      long v_len = size - 2;          //Don't verify RC Calibration location
	long v_len = size;

	if (GetBus()->GetLastProgrammedAddress() > 0 && GetBus()->GetLastProgrammedAddress() < size - 1)
	{
		v_len = GetBus()->GetLastProgrammedAddress() + 2;
		GetBus()->ClearLastProgrammedAddress();         //reset last_programmed_addr, so next verify not preceeded by write verify all the flash
	}

	//Set blank locations to default 0xFF (erased)
	memset(localbuf, 0xFF, size);

	// read the current flash content and store it in localbuf
	rval = GetBus()->Read(0, localbuf, v_len);

	if (rval != v_len)
	{
		if (rval > 0)
		{
			rval = OP_ABORTED;
		}
	}
	else
	{
		rval = GetBus()->CompareMultiWord(GetBufPtr() + base, localbuf + base, v_len, 0) == 0 ? OK : 1;
	}

	return rval;
}

/**
//Write Flash program memory (don't program RC calibration value)
int Pic125xx::WriteProg()
{
        int rv;
        int size = GetSplitted()-2;             //Don't program last location (RC calibration value)
        int base = 0;

        rv = GetBus()->Write(0, GetBufPtr()+base, size);
        if ( rv != size )
        {
                if (rv > 0)
                        rv = OP_ABORTED;
        }

        return rv;
}
**/
