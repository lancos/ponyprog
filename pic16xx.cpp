//=========================================================================//
//-------------------------------------------------------------------------//
// pic16xx.cpp -- Source for Pic16xx Class                                 //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#include "types.h"
#include "pic16xx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2awinfo.h"

#undef	BANK_SIZE
#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
Pic16xx::Pic16xx(e2AppWinInfo *wininfo, BusIO *busp)
	:	EEProm(wininfo, busp, BANK_SIZE)
{
	int j;
	for (j = 0; j < 8; j++)
		id_locations[j] = 0xffff;
}

//--- Distruttore
Pic16xx::~Pic16xx()
{
}

int Pic16xx::SecurityRead(int &msb, int &lsb)
{
	GetBus()->ReadConfig(id_locations);

	memcpy(GetBufPtr() + GetSplitted(), id_locations, 8*sizeof(WORD));

	WORD config = id_locations[7];

	config = ~config & 0x3fff;

	msb = (config >> 8) & 0xff;
	lsb = config & 0xff;

	return OK;
}

int Pic16xx::SecurityWrite(int msb, int lsb)
{
	WORD config = ((WORD)msb << 8) | lsb;

	//Extend the CP bit (PIC16F84)
	if (lsb & (1<<4))
		config |= 0xfff0;
	else
		config &= 0x000f;

	config = ~config & 0x3fff;

	id_locations[7] = config;

	if ( GetBus()->WriteConfig(id_locations) == OK )
	{
		memcpy(GetBufPtr() + GetSplitted(), id_locations, 8*sizeof(WORD));
	}

	return OK;
}

int Pic16xx::Probe(int probe_size)
{
//	GetBus()->ReadConfig(id_locations);

	return OK;
}

int Pic16xx::Read(int probe)
{
	Probe( probe || GetNoOfBank() == 0 );

	int rv;
	int size = GetNoOfBank() * GetBankSize();
	if (size > GetSplitted())
	{
		// legge il contenuto attuale della FlashEPROM in memoria
		rv = GetBus()->Read(0, GetBufPtr(), GetSplitted());

		if (rv != GetSplitted())
		{
			if (rv > 0)
				rv = OP_ABORTED;
		}
		else
		{
			// legge il contenuto attuale della EEPROM in memoria subito dopo la Flash
			rv = GetBus()->Read(1, GetBufPtr()+GetSplitted()+16, size - (GetSplitted()+16) );
			if (rv != size - (GetSplitted()+16) )
			{
				if (rv > 0)
					rv = OP_ABORTED;
			}
			else
			{
				// read the config locations
				// this must be the LAST operation (to exit from config mode we have to clear Vpp)
				int f1,f0;
				SecurityRead(f1, f0);
				GetAWInfo()->SetFuseBits(f0);
				GetAWInfo()->SetLockBits(f1);

				rv = size;
			}
		}
	}
	else
		rv = -1;	//Fatal error!!

	return rv;
}

int Pic16xx::Write(int probe)
{
	GetBus()->Erase();

	if (probe || GetNoOfBank() == 0)
		Probe();

	int size = GetNoOfBank() * GetBankSize();
	if (size > GetSplitted())
	{
	//	GetBus()->Reset();		// 10/11/99

		// write the Program FlashEPROM
		GetBus()->Write(0, GetBufPtr(), GetSplitted());

		// write the Data EEPROM
		GetBus()->Write(1, GetBufPtr()+GetSplitted()+16, size - (GetSplitted()+16) );

		// write the config locations
		// this must be the LAST operation (to exit from config mode we have to clear Vpp)
		GetBus()->WriteConfig( (WORD *) (GetBufPtr()+GetSplitted()) );
	}
	else
		return -1;	//Fatal error!!

	return GetNoOfBank();
}

int Pic16xx::Verify()
{
	GetBus()->Reset();

	if (GetNoOfBank() == 0)
		return BADPARAM;

	int rval = -1;
	int size = GetNoOfBank() * GetBankSize();
	if (size > GetSplitted())
	{
		unsigned char *localbuf;
		localbuf = new unsigned char[size];
		if (localbuf == 0)
			return OUTOFMEMORY;

		//initialize local buffer to all FF
		memset(localbuf, 0xff, size);

		// legge il contenuto attuale della FlashEPROM in memoria
		GetBus()->Read(0, localbuf, GetSplitted());

		// legge il contenuto attuale della EEPROM in memoria subito dopo la Flash
		GetBus()->Read(1, localbuf+GetSplitted()+16, size - (GetSplitted()+16) );

		// read the config locations
		// this must be the LAST operation (to exit from config mode we have to clear Vpp)
		GetBus()->ReadConfig( (UWORD *) (localbuf+GetSplitted()) );

		if ( memcmp(GetBufPtr()+GetSplitted()+16, localbuf+GetSplitted()+16,  size - (GetSplitted()+16)) != 0 ||
			GetBus()->CompareMultiWord(GetBufPtr(), localbuf, GetSplitted()+16) != 0 )
		{
			rval = 0;
		}
		else
			rval = 1;
		delete localbuf;
	}

	return rval;
}
