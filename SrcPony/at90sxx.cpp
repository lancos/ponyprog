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
#include "globals.h"
#include "at90sxx.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

#include <QDebug>

#include "e2awinfo.h"
#include "e2cmdw.h"

At90sxx::At90sxx(e2AppWinInfo *wininfo, BusIO *busp)
	:       Device(wininfo, busp, 1 /*BANK_SIZE*/)
{
	qDebug() << "At90sxx::At90sxx()";
}

At90sxx::~At90sxx()
{
	qDebug() << "At90sxx::~At90sxx()";
}

int At90sxx::SecurityRead(uint32_t &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadLockBits(GetAWInfo()->GetEEPId());
		rv = OK;
	}

	return rv;
}

int At90sxx::SecurityWrite(uint32_t bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)     //Try to write even with AutoXXX device setted
	{
		rv = GetBus()->WriteLockBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

int At90sxx::FusesRead(uint32_t &bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		bits = GetBus()->ReadFuseBits(GetAWInfo()->GetEEPId());
		rv = OK;
	}

	return rv;
}

int At90sxx::FusesWrite(uint32_t bits)
{
	int rv = Probe();               //No size probe needed, just probe for presence

	if (rv > 0)
	{
		rv = GetBus()->WriteFuseBits(bits, GetAWInfo()->GetEEPId());
	}

	return rv;
}

/**
 * code[0]: Vendor Code
 *                      0x1E (manufactured by Atmel)
 * code[1]: Part Family and Flash Size
 *                      0x90 AVR with 1K flash
 *                      0x91 AVR with 2K flash
 *                      0x92 AVR with 4K flash
 *                      0x93 AVR with 8K flash
 *                      0x94 AVR with 16K flash
 *                      0x95 AVR with 32K flash
 *                      0x96 AVR with 64K flash
 *                      0x97 AVR with 128K flash
 * code[2]: Part Number
 *
 * Part number identification table
 * [ 1 ]        [ 2 ]
 * 0x90         0x01            AT90S1200 (1K + 64)
 * 0x91         0x01            AT90S2313 (2K + 128)
 * 0x92         0x01            AT90S4414 (4K + 256)
 * 0x93         0x01            AT90S8515 (8K + 512)
 *
 * 0x06         0x01            ATmega603 (64K + 2K) first few pieces
 * 0x01         0x01            ATmega103 (128K + 4K) first few pieces
 * 0x96         0x01            ATmega603 (64K + 2K)
 * 0x97         0x01            ATmega103 (128K + 4K)
 *
 * 0x91         0x02            AT90S2323 (2K + 128)
 * 0x91         0x03            AT90S2343 (2K + 128)
 *
 * 0x92         0x03            AT90S4434 (4K + 256)
 * 0x93         0x03            AT90S8535 (8K + 512)
 *
 * 0x91         0x05            AT90S2333 (2K + 128)
 * 0x92         0x02            AT90S4433 (4K + 256)
 *
 * 0x93         0x04            AT90C8534 (8K + 512)
 *
 * 0x93         0x07            ATmega8 (8K + 512)
 * 0x94         0x03            ATmega16 (16K + 512)
 *
 * 0x94         0x01            ATmega161 (16K + 512)
 * 0x94         0x02            ATmega163 (16K + 512)
 *
 * 0x95         0x01            ATmega323 (32K + 1K)
 * 0x97         0x02            ATmega128 (128K + 4K)
 * 0x96         0x02            ATmega64 (64K + 2K)
 *
 * 0x90         0x03            ATtiny10 (1K)
 * 0x90         0x04            ATtiny11 (1K)
 * 0x90         0x05            ATtiny12 (1K + 64)
 * 0x90         0x06            ATtiny15 (1K + 64)
 *
 * 0x91         0x06            ATtiny22 (2K + 128)
 * 0x91         0x07            ATtiny28 (2K)
 **/

typedef struct
{
	int code1;
	int code2;
	int type;
} IdType;

static IdType IdArray[] =
{
	{0x90, 0x01,    AT90S1200},

	{0x91, 0x01,    AT90S2313},
	{0x91, 0x02,    AT90S2323},
	{0x91, 0x03,    AT90S2343},
	{0x91, 0x05,    AT90S2333},

	{0x92, 0x01,    AT90S4414},
	{0x92, 0x02,    AT90S4434},
	{0x92, 0x03,    AT90S4433},

	{0x93, 0x01,    AT90S8515},
	{0x93, 0x03,    AT90S8535},
	{0x93, 0x04,    AT90S8534},

	//{0x90, 0x08,    ATtiny9},
	{0x90, 0x03,    ATtiny10},              //only HV prog
	{0x90, 0x04,    ATtiny11},              //only HV prog
	{0x90, 0x05,    ATtiny12},
	{0x90, 0x07,    ATtiny13},              //new (Paul 2005/03/24)
	{0x90, 0x06,    ATtiny15},
	//{0x90, 0x0c,    ATtiny102},
	//{0x90, 0x0b,    ATtiny104},

	//{0x91, 0x0f,    ATtiny20},
	{0x91, 0x06,    ATtiny22},
	{0x91, 0x0B,    ATtiny24},      // new 08.01.2015 @RG
	{0x91, 0x08,    ATtiny25},
	{0x91, 0x09,    ATtiny26},
	{0x91, 0x07,    ATtiny28},              //only HV parallel prog
	{0x91, 0x0A,    ATtiny2313},
	{0x91, 0x0C,    ATtiny261},
	//{0x91, 0x23,    ATtiny202},
	//{0x91, 0x22,    ATtiny204},
	//{0x91, 0x21,    ATtiny212},
	//{0x91, 0x20,    ATtiny214},

	//{0x92, 0x0e,    ATtiny40},
	{0x92, 0x07,    ATtiny44},      // new 08.01.2015 @RG
	{0x92, 0x06,    ATtiny45},
	//{0x92, 0x09,    ATtiny48},
	{0x92, 0x08,    ATtiny461},
	{0x92, 0x0D,    ATtiny4313},    // new 16.09.2015 @RG
	//{0x92, 0x0c,    ATtiny43},	//ATtiny43U
	//{0x92, 0x15,    ATtiny441},
	//{0x92, 0x27,    ATtiny402},
	//{0x92, 0x26,    ATtiny404},
	//{0x92, 0x25,    ATtiny406},
	//{0x92, 0x23,    ATtiny412},
	//{0x92, 0x22,    ATtiny414},
	//{0x92, 0x21,    ATtiny416},
	//{0x92, 0x20,    ATtiny417},
	//{0x92, 0x2c,    ATtiny424},
	//{0x92, 0x2b,    ATtiny426},
	//{0x92, 0x2a,    ATtiny427},

	{0x93, 0x0C,    ATtiny84},      // new 08.01.2015 @RG
	{0x93, 0x0B,    ATtiny85},
	{0x93, 0x0D,    ATtiny861},
	//{0x93, 0x87,    ATtiny87},
	//{0x93, 0x11,    ATtiny88},
	//{0x93, 0x14,    ATtiny828},
	//{0x93, 0x15,    ATtiny841},
	//{0x93, 0x25,    ATtiny804},
	//{0x93, 0x24,    ATtiny806},
	//{0x93, 0x23,    ATtiny807},
	//{0x93, 0x22,    ATtiny814},
	//{0x93, 0x21,    ATtiny816},
	//{0x93, 0x20,    ATtiny817},
	//{0x93, 0x29,    ATtiny824},
	//{0x93, 0x28,    ATtiny826},
	//{0x93, 0x27,    ATtiny827},

	//{0x94, 0x12,    ATtiny1634},
	//{0x94, 0x87,    ATtiny167},

	//{0x94, 0x25,    ATtiny1604},
	//{0x94, 0x24,    ATtiny1606},
	//{0x94, 0x23,    ATtiny1607},
	//{0x94, 0x22,    ATtiny1614},
	//{0x94, 0x21,    ATtiny1616},
	//{0x94, 0x20,    ATtiny1617},
	//{0x94, 0x2A,    ATtiny1624},
	//{0x94, 0x29,    ATtiny1626},
	//{0x94, 0x28,    ATtiny1627},

	//{0x95, 0x21,    ATtiny3216},
	//{0x95, 0x22,    ATtiny3217},


	{0x92, 0x05,    ATmega48},      // ATmega48A
	{0x92, 0x0A,    ATmega48},      // ATmega48PA
	{0x92, 0x10,    ATmega48},      // ATmega48PB

	{0x93, 0x06,    ATmega8515},
	{0x93, 0x08,    ATmega8535},
	{0x93, 0x07,    ATmega8},
	{0x93, 0x10,    ATmega8},		//ATmega8HVA
	{0x93, 0x89,    ATmega8},		//ATmega8U2
	{0x93, 0x0A,    ATmega88},      // ATmega88A
	{0x93, 0x0F,    ATmega88},      // ATmega88PA
	{0x93, 0x16,    ATmega88},      // ATmega88PB
	//{0x93, 0x26,    ATmega808},      // ATmega808
	//{0x93, 0x2a,    ATmega809},      // ATmega809

	{0x94, 0x01,    ATmega161},
	{0x94, 0x02,    ATmega163},
	{0x94, 0x03,    ATmega16},
	{0x94, 0x0C,    ATmega16},		//ATmega16HVA
	{0x94, 0x0D,    ATmega16},		//ATmega16HVB
	{0x94, 0x84,    ATmega16},		//ATmega16M1
	{0x94, 0x88,    ATmega16},		//ATmega16U4
	{0x94, 0x89,    ATmega16},		//ATmega16U2
	{0x94, 0x04,    ATmega162},
	{0x94, 0x05,    ATmega169},
	{0x94, 0x11,    ATmega169},
	{0x94, 0x06,    ATmega168},     // ATmega168A
	{0x94, 0x0B,    ATmega168},     // ATmega168PA
	{0x94, 0x15,    ATmega168},     // ATmega168PB
	{0x94, 0x0A,    ATmega164},     // ATmega164PA
	{0x94, 0x0F,    ATmega164},     // new ATmega164A (RG 30.08.2013)
	//{0x94, 0x10,	ATmega165},
	//{0x94, 0x07,	ATmega165},     // ATmega165P
	//{0x94, 0x27,	ATmega1608},
	//{0x94, 0x26,	ATmega1609},

	{0x95, 0x01,    ATmega323},
	{0x95, 0x02,    ATmega32},
	{0x95, 0x86,    ATmega32},		// ATmega32C1
	{0x95, 0x10,    ATmega32},		// ATmega32HVB
	{0x95, 0x84,    ATmega32},		// ATmega32M1
	{0x95, 0x8A,    ATmega32},		// ATmega32U2
	{0x95, 0x87,    ATmega32},		// ATmega32U4
	{0x95, 0x08,    ATmega324},
	{0x95, 0x15,    ATmega324},     // new ATmega324A (RG 30.08.2013)
	{0x95, 0x11,    ATmega324},     // new ATmega324PA (RG 30.08.2013)
	{0x95, 0x17,    ATmega324},     // ATmega324PB
	{0x95, 0x14,    ATmega328},     // ATmega328
	{0x95, 0x0F,    ATmega328},     // ATmega328P
	{0x95, 0x16,    ATmega328},     // ATmega328PB
	//{0x95, 0x03,    ATmega329},		// ATmega329
	//{0x95, 0x0b,    ATmega329},		// ATmega329P
	//{0x95, 0x04,    ATmega3290},	// ATmega3290
	//{0x95, 0x0c,    ATmega3290},	// ATmega3290P
	//{0x95, 0x05,    ATmega325},		// ATmega325
	//{0x95, 0x0d,    ATmega325},		// ATmega325P
	//{0x95, 0x06,    ATmega3250},	// ATmega3250
	//{0x95, 0x0e,    ATmega3250},	// ATmega3250P
	//{0x95, 0x30,    ATmega3208},	// ATmega3208
	//{0x95, 0x31,    ATmega3209},	// ATmega3209
	//{0x95, 0x07,    ATmega406},	// ATmega406

	{0x96, 0x02,    ATmega64},
	{0x96, 0x84,    ATmega64},		// ATmega64M1
	{0x96, 0x86,    ATmega64},		// ATmega64C1
	{0x96, 0x10,    ATmega64},		// ATmega64HVE2
	//{0x96, 0x03,	ATmega649},
	//{0x96, 0x0b,	ATmega649},		// ATmega649P
	//{0x96, 0x04,	ATmega6490},
	//{0x96, 0x0c,	ATmega6490},
	{0x96, 0x09,    ATmega644},     // ATmega644A
	{0x96, 0x0A,    ATmega644},     // new ATmega644PA (RG 18.04.2012)
	//{0x96, 0x05,	ATmega645},
	//{0x96, 0x0d,	ATmega645},		// ATmega645P
	//{0x96, 0x06,	ATmega6450},
	//{0x96, 0x0e,	ATmega6450},	// ATmega6450P
	{0x96, 0x08,    ATmega640},
	//{0x96, 0x50,    ATmega4808},	// ATmega4808
	//{0x96, 0x55,    ATmega4809},	// ATmega4809

	{0x96, 0x01,    ATmega603},
	{0x97, 0x01,    ATmega103},
	{0x06, 0x01,    ATmega603},
	{0x01, 0x01,    ATmega103},

	{0x97, 0x02,    ATmega128},
	{0x97, 0x03,    ATmega1280},
	{0x97, 0x04,    ATmega1281},
	{0x97, 0x05,    ATmega1284},     // new ATmega1284P (RG 10.06.2017)
	{0x97, 0x06,    ATmega1284},

	{0x98, 0x01,    ATmega2560},
	{0x98, 0x02,    ATmega2561},

	{0x95, 0x81,    AT90CAN32},
	{0x96, 0x81,    AT90CAN64},
	{0x97, 0x81,    AT90CAN128},

	//      {0x51, 0x06,    AT89551},
	//      {0x52, 0x06,    AT89552},
	//      {0x91, 0x81,    AT86RF401},

	{0x00, 0x00,    AT90S0000}
};

int At90sxx::QueryType(long &type)
{
	int rv;

	int code[3];

	code[0] = GetBus()->ReadDeviceCode(0);
	code[1] = GetBus()->ReadDeviceCode(1);
	code[2] = GetBus()->ReadDeviceCode(2);

	qDebug() << "At90sxx::ParseID() *** " << (hex) << code[0] << " - " << code[1] << " - " << code[2] << (dec);

	detected_type = type = 0;
	detected_signature = "";

	//
	if (code[0] == 0x00 && code[1] == 0x01 && code[2] == 0x02)
	{
		//device is locked
		rv = DEVICE_LOCKED;
	}
	else if (code[0] == 0x1E)
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
		{
			detected_type = type;
		}

		detected_signature.sprintf("%02X-%02X-%02X", code[0], code[1], code[2]);
		//snprintf(detected_signature, MAXMSG, "%02X-%02X-%02X", code[0], code[1], code[2]);

		rv = type ? OK : DEVICE_UNKNOWN;
	}
	else
	{
		//Preproduction device or device missing
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
int At90sxx::Probe(int probe_size)
{
	int rv;

	qDebug() << "At90sxx::Probe(" << probe_size << ") IN";

	if (cmdWin->GetIgnoreFlag())
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

	qDebug() << "At90sxx::Probe() = " << rv << " **  OUT";

	return rv;
}

int At90sxx::Erase(int probe, int type)
{
	int rv = OK;

	if ((type & PROG_TYPE) && (type & DATA_TYPE))
	{
		GetBus()->Erase();
		rv = Probe(probe || GetNoOfBank() == 0);
	}
	else
	{
		rv = NOTSUPPORTED;
	}

	return rv;
}

int At90sxx::Read(int probe, int type)
{
	int rv = Probe(probe || GetNoOfBank() == 0);

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

			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					// read the fuses
					uint32_t f = GetBus()->ReadFuseBits(GetAWInfo()->GetEEPId());
					GetAWInfo()->SetFuseBits(f);

					f = GetBus()->ReadLockBits(GetAWInfo()->GetEEPId());
					GetAWInfo()->SetLockBits(f);
				}
			}
		}
	}

	return rv;
}

int At90sxx::Write(int probe, int type)
{
	//      if ( (type & PROG_TYPE) && (type & DATA_TYPE) )
	if ((type & PROG_TYPE))                 //Because to write the flash we must erase ALL the device (a msg may alert that doing so the DATA may be erased too)
	{
		GetBus()->Erase();
	}

	int rv = Probe(probe || GetNoOfBank() == 0);

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

			if (rv > 0)
			{
				if (type & CONFIG_TYPE)
				{
					if (GetAWInfo()->GetEEPId() == AT90S4433 || GetAWInfo()->GetEEPId() == AT90S2333)
					{
						//write the locks
						uint32_t f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f, GetAWInfo()->GetEEPId());

						GetBus()->WaitMsec(100);

						//write the fuses
						f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPId());
					}
					else
					{
						//write the fuses
						uint32_t f = GetAWInfo()->GetFuseBits();
						GetBus()->WriteFuseBits(f, GetAWInfo()->GetEEPId());

						//write the locks
						f = GetAWInfo()->GetLockBits();
						GetBus()->WriteLockBits(f, GetAWInfo()->GetEEPId());
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
			// read the fuses & locks
			uint32_t f = GetBus()->ReadFuseBits(GetAWInfo()->GetEEPId());
			uint32_t l = GetBus()->ReadLockBits(GetAWInfo()->GetEEPId());

			if (GetAWInfo()->GetLockBits() == l && GetAWInfo()->GetFuseBits() == f)
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
