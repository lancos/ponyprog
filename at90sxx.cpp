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
#include "e2app.h"
#include "at90sxx.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

#include "e2awinfo.h"

#undef	BANK_SIZE
#define	BANK_SIZE	1

//=====>>> Costruttore <<<======
At90sxx::At90sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:	Device(wininfo, busp, BANK_SIZE)
{
	UserDebug(Constructor, "At90sxx::At90sxx()\n");
}

//--- Distruttore
At90sxx::~At90sxx()
{
	UserDebug(Destructor, "At90sxx::~At90sxx()\n");
}

int At90sxx::SecurityRead(uint32_t &bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadLockBits( GetAWInfo()->GetEEPType() );
		rv = OK;
	}
	return rv;
}

int At90sxx::SecurityWrite(uint32_t bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv > 0)	//Try to write even with AutoXXX device setted
	{
		rv = GetBus()->WriteLockBits(bits,  GetAWInfo()->GetEEPType());
	}
	return rv;
}

int At90sxx::FusesRead(uint32_t &bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadFuseBits( GetAWInfo()->GetEEPType() );
		rv = OK;
	}
	return rv;
}

int At90sxx::FusesWrite(uint32_t bits)
{
	int rv = Probe();		//No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->WriteFuseBits(bits, GetAWInfo()->GetEEPType());
	}
	return rv;
}

/**
 * code[0]: Vendor Code
 *			0x1E (manufactured by Atmel)
 * code[1]: Part Family and Flash Size
 *			0x90 AVR with 1K flash
 *			0x91 AVR with 2K flash
 *			0x92 AVR with 4K flash
 *			0x93 AVR with 8K flash
 *			0x94 AVR with 16K flash
 *			0x95 AVR with 32K flash
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
 * 0x93		0x07		ATmega8 (8K + 512)
 * 0x94		0x03		ATmega16 (16K + 512)
 *
 * 0x94		0x01		ATmega161 (16K + 512)
 * 0x94		0x02		ATmega163 (16K + 512)
 *
 * 0x95		0x01		ATmega323 (32K + 1K)
 * 0x97		0x02		ATmega128 (128K + 4K)
 * 0x96     0x02        ATmega64 (64K + 2K)
 *
 * 0x90		0x03		ATtiny10 (1K)
 * 0x90		0x04		ATtiny11 (1K)
 * 0x90		0x05		ATtiny12 (1K + 64)
 * 0x90		0x06		ATtiny15 (1K + 64)
 *
 * 0x91		0x06		ATtiny22 (2K + 128)
 * 0x91		0x07		ATtiny28 (2K)
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

	{0x93, 0x07,	ATmega8},
	{0x93, 0x06,	ATmega8515},
	{0x93, 0x08,	ATmega8535},

	{0x94, 0x01,	ATmega161},
	{0x94, 0x02,	ATmega163},
	{0x94, 0x03,	ATmega16},
	{0x94, 0x04,	ATmega162},
	{0x94, 0x05,	ATmega169},

	{0x95, 0x01,	ATmega323},
	{0x95, 0x02,	ATmega32},

	{0x97, 0x02,	ATmega128},
	{0x96, 0x02,	ATmega64},

	{0x90, 0x03,	ATtiny10},		//only HV prog
	{0x90, 0x04,	ATtiny11},		//only HV prog
	{0x90, 0x05,	ATtiny12},
	{0x90, 0x07,	ATtiny13},		//new (Paul 2005/03/24)
	{0x90, 0x06,	ATtiny15},

	{0x91, 0x06,	ATtiny22},
	{0x91, 0x09,	ATtiny26},
	{0x91, 0x07,	ATtiny28},		//only HV parallel prog

	{0x91, 0x0A,	ATtiny2313},
	{0x92, 0x0D,	ATtiny4313},    // new 16.09.2015 @RG

	{0x91, 0x0B,	ATtiny24},      // new 08.01.2015 @RG
	{0x92, 0x07,	ATtiny44},      // new 08.01.2015 @RG
	{0x93, 0x0C,	ATtiny84},      // new 08.01.2015 @RG

	{0x91, 0x08,	ATtiny25},
	{0x92, 0x06,	ATtiny45},
	{0x93, 0x0B,	ATtiny85},

	{0x91, 0x0C,	ATtiny261},
	{0x92, 0x08,	ATtiny461},
	{0x93, 0x0D,	ATtiny861},

	{0x92, 0x05,	ATmega48},
	{0x92, 0x0A,	ATmega48},      // new ATmega48PA (RG 18.11.2013)
	{0x93, 0x0A,	ATmega88},
	{0x93, 0x0F,	ATmega88},      // new ATmega88PA (RG 18.11.2013)
	{0x94, 0x0B,	ATmega168},     // new ATmega168PA (RG 28.09.2012)
    {0x94, 0x06,	ATmega168},     // ATmega168A
	{0x95, 0x0F,	ATmega328},     // new ATmega328P (RG 23.06.2012)
	{0x95, 0x14,	ATmega328},     // new ATmega328 (RG 18.11.2013)
	{0x95, 0x16,	ATmega328},     // ATmega328PB (RG 27.11.2020)
	{0x95, 0x0F,	ATmega328},     // ATmega328P

	{0x94, 0x0A,	ATmega164},
	{0x94, 0x0F,	ATmega164},     // new ATmega164A (RG 30.08.2013)
	{0x95, 0x08,	ATmega324},
	{0x95, 0x15,	ATmega324},     // new ATmega324A (RG 30.08.2013)
	{0x95, 0x11,	ATmega324},     // new ATmega324PA (RG 30.08.2013)
	{0x96, 0x09,	ATmega644},
	{0x96, 0x0A,	ATmega644},     // new ATmega644PA (RG 18.04.2012)

	{0x95, 0x81,	AT90CAN32},
	{0x96, 0x81,	AT90CAN64},
	{0x97, 0x81,	AT90CAN128},

	{0x96, 0x08,	ATmega640},
	{0x97, 0x03,	ATmega1280},
	{0x97, 0x04,	ATmega1281},
    {0x97, 0x05,	ATmega1284},     // new ATmega1284P (RG 10.06.2017)
    {0x97, 0x06,	ATmega1284},     // new ATmega1284 (RG 19.06.2017)

	{0x98, 0x01,	ATmega2560},
	{0x98, 0x02,	ATmega2561},

//	{0x51, 0x06,	AT89551},
//	{0x52, 0x06,	AT89552},
//	{0x91, 0x81,	AT86RF401},

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

	detected_type = type = 0;
	detected_signature[0] = '\0';
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
		if (type)
			detected_type = type;
		snprintf(detected_signature, MAXMSG, "%02X-%02X-%02X", code[0], code[1], code[2]);

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
	int rv;

	UserDebug1(UserApp2, "At90sxx::Probe(%d) IN\n", probe_size);

	if ( THEAPP->GetIgnoreFlag() )
	{
		rv = GetSize();
	}
	else
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
	UserDebug1(UserApp2, "At90sxx::Probe() = %d **  OUT\n", rv);

	return rv;
}

int At90sxx::Erase(int probe, int type)
{
	int rv = OK;

	if ( (type & PROG_TYPE) && (type & DATA_TYPE) )
	{
		GetBus()->Erase();
		rv = Probe( probe || GetNoOfBank() == 0 );
	}
	else
		rv = NOTSUPPORTED;

	return rv;
}

int At90sxx::Read(int probe, int type)
{
	int rv = Probe( probe || GetNoOfBank() == 0 );

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
			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					// read the fuses
					uint32_t f = GetBus()->ReadFuseBits( GetAWInfo()->GetEEPType() );
					GetAWInfo()->SetFuseBits(f);

					f = GetBus()->ReadLockBits( GetAWInfo()->GetEEPType() );
					GetAWInfo()->SetLockBits(f);
				}
			}
		}
	}
	return rv;
}

int At90sxx::Write(int probe, int type)
{
//	if ( (type & PROG_TYPE) && (type & DATA_TYPE) )
	if ( (type & PROG_TYPE) )		//Because to write the flash we must erase ALL the device (a msg may alert that doing so the DATA may be erased too)
		GetBus()->Erase();

	int rv = Probe( probe || GetNoOfBank() == 0 );

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
			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					if ( GetAWInfo()->GetEEPType() == AT90S4433 || GetAWInfo()->GetEEPType() == AT90S2333 )
					{
						//write the locks
						uint32_t f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f,  GetAWInfo()->GetEEPType());

						GetBus()->WaitMsec(100);

						//write the fuses
						f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPType());
					}
					else
					{
						//write the fuses
						uint32_t f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPType());

						//write the locks
						f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f,  GetAWInfo()->GetEEPType());
					}
				}
			}
		}
	}
	return rv;
}

int At90sxx::Verify(int type)
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
			// read the fuses & locks
			uint32_t f = GetBus()->ReadFuseBits( GetAWInfo()->GetEEPType() );
			uint32_t l = GetBus()->ReadLockBits( GetAWInfo()->GetEEPType() );

			if ( GetAWInfo()->GetLockBits() == l && GetAWInfo()->GetFuseBits() == f )
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
