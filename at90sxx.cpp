//=========================================================================//
//-------------------------------------------------------------------------//
// at90sxx.cpp -- Source for At90sxx Class (AVR device class)              //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-1999  Claudio Lanconelli                            //
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
#include "e2app.h"
#include "at90sxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2awinfo.h"

#undef	BANK_SIZE
#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At90sxx::At90sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	EEProm(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "At90sxx::At90sxx()\n");
}

//--- Distruttore
At90sxx::~At90sxx()
{
	UserDebug(Destructor, "At90sxx::~At90sxx()\n");
}

//Doesn't work with early AVR
int At90sxx::SecurityRead(int &value, int &notused)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv == DEVICE_LOCKED)
	{
		value = 0x06;		//Both lock bits programmed
		rv = OK;
	}
	else
	if (rv >= 0)
	{
		rv = GetBus()->ReadLockBits();

		if (rv >= 0)
		{
			value = (~rv) & 0xFF;
			rv = OK;
		}
	}

	return rv;
}

int At90sxx::SecurityWrite(int value, int notused)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv >= 0)	//Try to write even with AutoXXX device setted
	{
		rv = GetBus()->WriteLockBits(value);
	}

	return rv;
}

//Doesn't work with early AVR
int At90sxx::FusesRead(int &value)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv >= 0)
	{
		rv = GetBus()->ReadFuseBits();

		if (rv >= 0)
		{
			value = (~rv) & 0xFF;
			rv = OK;
		}
	}

	return rv;
}

int At90sxx::FusesWrite(int value)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv >= 0)	//Try to write even with AutoXXX device setted
		rv = GetBus()->WriteFuseBits(value);

	return rv;
}

/**
 * code[0]: Vendor Code
 *			0x1E (manucaftured by Atmel)
 * code[1]: Part Family and Flash Size
 *			0x90 AVR with 1K flash
 *			0x91 AVR with 2K flash
 *			0x92 AVR with 4K flash
 *			0x93 AVR with 8K flash
 *			0x94 AVR with 16K flash (?)
 *			0x95 AVR with 32K flash (?)
 *			0x96 AVR with 64K flash
 *			0x97 AVR with 128K flash
 * code[2]: Part Number
 *
 * Part number identification table
 * [ 1 ]	[ 2 ]
 * 0x90		0x01		AT90S1200 (1K + 64)
 * 0x91		0x01		AT90S2313 (2K + 128)
 * 0x92		0x01		AT90S4414 (4K + 256)
 * 0x93		0x01		AT90S8515 (8K + 512)
 *
 * 0x06		0x01		ATmega603 (64K + 2K) first few pieces
 * 0x01		0x01		ATmega103 (128K + 4K) first few pieces
 * 0x96		0x01		ATmega603 (64K + 2K)
 * 0x97		0x01		ATmega103 (128K + 4K)
 *
 * 0x91		0x02		AT90S2323 (2K + 128)
 * 0x91		0x03		AT90S2343 (2K + 128)
 *
 * 0x92		0x03		AT90S4434 (4K + 256)
 * 0x93		0x03		AT90S8535 (8K + 512)
 *
 * 0x91		0x05		AT90S2333 (2K + 128)
 * 0x92		0x02		AT90S4433 (4K + 256)
 *
 * 0x93		0x04		AT90C8534 (8K + 512)
 *
 * 0x90		0x03		ATtiny10 (1K)
 * 0x90		0x04		ATtiny11 (1K)
 * 0x90		0x05		ATtiny12 (1K + 64)
 *
 * 0x91		0x06		ATtiny22 (2K + 128)
 **/

typedef struct {
	int code1;
	int code2;
	int type;
} IdType;

static IdType IdArray[] = {
	{0x90, 0x01,	AT90S1200},
	{0x91, 0x01,	AT90S2313},
	{0x92, 0x01,	AT90S4414},
	{0x93, 0x01,	AT90S8515},

	{0x96, 0x01,	ATmega603},
	{0x97, 0x01,	ATmega103},
	{0x06, 0x01,	ATmega603},
	{0x01, 0x01,	ATmega103},

	{0x91, 0x02,	AT90S2323},
	{0x91, 0x03,	AT90S2343},

	{0x92, 0x02,	AT90S4434},
	{0x93, 0x03,	AT90S8535},

	{0x91, 0x05,	AT90S2333},
	{0x92, 0x03,	AT90S4433},

	{0x93, 0x04,	AT90S8534},

	{0x90, 0x03,	ATtiny10},
	{0x90, 0x04,	ATtiny11},
	{0x90, 0x05,	ATtiny12},

	{0x91, 0x06,	ATtiny22},

	{0x00, 0x00,	AT90S0000}
};

int At90sxx::QueryType(long &type)
{
	int rv;

	int code[3];

	code[0] = GetBus()->ReadDeviceCode(0);
	code[1] = GetBus()->ReadDeviceCode(1);
	code[2] = GetBus()->ReadDeviceCode(2);

	UserDebug3(UserApp2, "At90sxx::ParseID() *** 0x%02X - 0x%02X - 0x%02X\n", code[0], code[1], code[2]);

	type = 0;
	if (code[0] == 0x00 && code[1] == 0x01 && code[2] == 0x02)
	{
		//device is locked
		rv = DEVICE_LOCKED;
	}
	else
	if (code[0] == 0x1E)
	{
		int k;

		for (k = 0; IdArray[k].code1 != 0x00; k++)
		{
			if (IdArray[k].code1 == code[1] && IdArray[k].code2 == code[2])
			{
				type = IdArray[k].type;
				break;
			}
		}

		rv = type ? OK : DEVICE_UNKNOWN;
	}
	else
	{	//Preproduction device or device missing
		rv = DEVICE_UNKNOWN;
	}

	return rv;
}

// Probe() can change the bus according to the device detected.
// Note that the Verify() operation don't call probe() before
// to read the device, so it use the current bus. In the case
// of AutoXXX selected the bus setted is the AT1200S.
// So it works because of the Read routine is the same just for
// AT1200Bus, AT90SXXBus and ATmegaBus.

// Detect the size of the device. Note that if the
// device is locked, the probe don't work and returns the
// error DEVICE_LOCKED.
// If the device is missing return 0.
//---
int At90sxx::Probe(int probe_size)
{
	int rv = 0;

	UserDebug1(UserApp2, "At90sxx::Probe(%d) IN\n", probe_size);

	long type;
	rv = QueryType(type);
	int subtype = GetE2PSubType(type);

	if (probe_size)
	{
		if (rv == OK)
		{
			switch(type)
			{
			case AT90S1200:
				SetBus(THEAPP->GetBusVectorPtr()[AT1200S-1]);
				break;
			case ATmega603:
			case ATmega103:
				SetBus(THEAPP->GetBusVectorPtr()[MEGAS-1]);
				break;
			default:	//AT90S std
				SetBus(THEAPP->GetBusVectorPtr()[AT90S-1]);
				break;
			}

			SetNoOfBank(  GetEEPTypeSize(AT90SXX, subtype) );
			SetSplitted( GetEEPTypeSplit(AT90SXX, subtype) );
			rv = GetSize();
		}
	}
	else
	{
		if ( THEAPP->GetIgnoreFlag() )
		{
			rv = GetSize();
		}
		else
		{
			if (rv == OK)
			{
				if ( GetAWInfo()->GetEEPSubType() == subtype )
					rv = GetSize();
				else
					rv = DEVICE_BADTYPE;
			}
		}
	}

	UserDebug1(UserApp2, "At90sxx::Probe() = %d **  OUT\n", rv);

	return rv;
}


int At90sxx::Read(int probe)
{
	//Save Bus, so the AutoXXX can change the bus according
	// to the device with Probe(). After the read we restore
	// the original bus so the next operation still work.
	BusIO *old_bus = GetBus();

	int rv = Probe( probe || GetNoOfBank() == 0 );

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			// legge il contenuto attuale della FlashEPROM in memoria
			rv = GetBus()->Read(0, GetBufPtr(), GetSplitted());
			if (rv != GetSplitted())
			{
				if (rv > 0)
					rv = OP_ABORTED;
			}
			else
			if (GetSize() > GetSplitted())
			{
				// legge il contenuto attuale della EEPROM in memoria subito dopo la Flash
				rv = GetBus()->Read(1, GetBufPtr()+GetSplitted(), GetSize()-GetSplitted());
				if ( rv != GetSize()-GetSplitted() )
				{
					if (rv > 0)
						rv = OP_ABORTED;
				}
			}
		}
	}
	SetBus(old_bus);

	return rv;
}

int At90sxx::Write(int probe)
{
	//Save Bus, so the AutoXXX can change the bus according
	// to the device with Probe(). After the write we restore
	// the original bus so the next operation still work.
	BusIO *old_bus = GetBus();

	//11/09/99
	GetBus()->Erase();

	int rv = Probe( probe || GetNoOfBank() == 0 );

	if (rv > 0)
	{
		if (GetSize() >= GetSplitted())
		{
			//program the FlashEPROM
			rv = GetBus()->Write(0, GetBufPtr(), GetSplitted());
			if ( rv != GetSplitted() )
			{
				if (rv > 0)
					rv = OP_ABORTED;
			}
			else
			if (GetSize() > GetSplitted())
			{
				//program the EEPROM
				rv = GetBus()->Write(1, GetBufPtr()+GetSplitted(), GetSize()-GetSplitted());
				if ( rv != GetSize()-GetSplitted() )
				{
					if (rv > 0)
						rv = OP_ABORTED;
				}
			}
		}
	}
	SetBus(old_bus);

	return rv;
}

int At90sxx::Verify()
{
	if (GetSize() == 0)
		return BADPARAM;

	int rval = -1;
	long size = GetSize();
	if (size > GetSplitted())
	{
		unsigned char *localbuf;
		localbuf = new unsigned char[size];
		if (localbuf == 0)
			return OUTOFMEMORY;

		//Verify only programmed bytes (to save time in big devices)
		long v_len = GetSplitted();
		if (THEAPP->GetLastProgrammedAddress() > 0 && THEAPP->GetLastProgrammedAddress() < GetSplitted() )
		{
			v_len = THEAPP->GetLastProgrammedAddress() + 1;
			THEAPP->ClearLastProgrammedAddress();		//reset last_programmed_addr, so next verify not preceeded by write verify all the flash
		}
		//Set blank locations to default 0xFF (erased)
		memset(localbuf, 0xFF, GetSplitted());

		// read the current flash content and store it in localbuf
		rval = GetBus()->Read(0, localbuf, v_len);
		if ( rval != v_len )
		{
			if (rval > 0)
				rval = OP_ABORTED;
		}
		else
		{
			// legge il contenuto attuale della EEPROM in memoria subito dopo la Flash
			rval = GetBus()->Read(1, localbuf+GetSplitted(), size-GetSplitted());
			if ( rval != size-GetSplitted() )
			{
				if (rval > 0)
					rval = OP_ABORTED;
			}
			else
				rval = memcmp(GetBufPtr(), localbuf, size) != 0 ? 0 : 1;
		}
		delete localbuf;
	}

	return rval;
}
