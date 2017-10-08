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
// 24XX-2 are I2C Bus eeproms that uses 16 bits (2 bytes) for addresses.   //
// Total capacity can be up to 64KBytes.                                   //

#include <QString>

#include "types.h"
#include "e2profil.h"
#include "e2cmdw.h"

#include "e24xx-2.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

//=====>>> Costruttore <<<======
E24xx2::E24xx2(e2AppWinInfo *wininfo, BusIO *busp)
	:       E24xx(wininfo, busp),
			SecurityReadCode(0xC0),
			SecurityWriteCode(0x80),
			HEnduranceReadCode(0x40),
			HEnduranceWriteCode(0x00)
{
	writepage_size = E2Profile::GetI2CPageWrite();
	E2Profile::SetI2CPageWrite(writepage_size);
}

int E24xx2::Probe(int probe_size)
{
	int rval = E24xx::Probe(probe_size);

	if (rval > 0)
	{
		long size = GetSize();

		n_bank = 1;
		SetBankSize(size / n_bank);
		SetNoOfBank(n_bank);
	}

	return rval;
}

int E24xx2::Write(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	if (n_bank == 0)
	{
		return BADPARAM;
	}

	GetBus()->WriteStart();

	long size = GetSize();
	unsigned char *localbuf = new unsigned char[writepage_size + 2];

	if (localbuf == 0)
	{
		return OUTOFMEMORY;
	}

	int rval = OK;

	if (type & PROG_TYPE)
	{
		long j;

		for (j = 0; j < size; j += writepage_size)
		{
			memcpy(localbuf + 2, GetBufPtr() + j, writepage_size);

			localbuf[0] = (uint8_t)((j >> 8) & 0xFF);
			localbuf[1] = (uint8_t)(j & 0xFF);

			if (GetBus()->Write(eeprom_addr[0], localbuf, 2 + writepage_size) != (2 + writepage_size))
			{
				rval = GetBus()->Error();
				break;
			}

			int k;

			for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[0], localbuf, 1) != 1; k--)
				;

			if (k == 0)
			{
				rval = E2P_TIMEOUT;
				break;
			}

			if (GetBus()->WriteProgress(j * 100 / size))
			{
				rval = OP_ABORTED;
				break;
			}
		}
	}

	GetBus()->WriteEnd();

	delete[] localbuf;

	if (rval == OK)
	{
		return GetSize();
	}
	else
	{
		return rval;
	}
}

int E24xx2::Read(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	GetBus()->ReadStart();

	error = OK;

	if (type & PROG_TYPE)
	{
		uint8_t index[2];
		long readpage_size = 256;
		long k;
		long size = GetSize();

		for (k = 0; k < size; k += readpage_size)
		{
			//Scrive l'indice del sottoindirizzamento
			// partiamo sempre da 0.
			index[0] = (uint8_t)((k >> 8) & 0xFF);
			index[1] = (uint8_t)(k & 0xFF);

			if (GetBus()->StartWrite(eeprom_addr[0], index, 2) != 2)
			{
				error = GetBus()->Error();
				break;
			}

			if (GetBus()->Read(eeprom_addr[0], GetBufPtr() + k, readpage_size) < readpage_size)
			{
				error = GetBus()->Error();
				break;
			}

			if (GetBus()->ReadProgress(k * 100 / size))
			{
				error = OP_ABORTED;
				break;
			}
		}
	}

	GetBus()->ReadEnd();

	if (error == OK)
	{
		return GetSize();
	}
	else
	{
		return error;
	}
}

int E24xx2::Verify(int type)
{
	int rval = Probe();     //Moved here from 7 lines above (10/12/99)

	if (rval < 0)
	{
		return rval;
	}

	long readpage_size = 256;
	unsigned char *localbuf = new unsigned char[readpage_size];

	if (localbuf == 0)
	{
		return OUTOFMEMORY;
	}

	GetBus()->ReadStart();

	rval = 1;               //true

	if (type & PROG_TYPE)
	{
		uint8_t index[2];
		long k;
		long size = GetSize();

		for (k = 0; k < size; k += readpage_size)
		{
			//Scrive l'indice del sottoindirizzamento
			// partiamo sempre da 0.
			index[0] = (uint8_t)((k >> 8) & 0xFF);
			index[1] = (uint8_t)(k & 0xFF);

			if (GetBus()->StartWrite(eeprom_addr[0], index, 2) != 2)
			{
				rval = GetBus()->Error();
				break;
			}

			if (GetBus()->Read(eeprom_addr[0], localbuf, readpage_size) < readpage_size)
			{
				rval = GetBus()->Error();
				break;
			}

			if (memcmp(GetBufPtr() + k, localbuf, readpage_size) != 0)
			{
				rval = 0;
				break;
			}

			if (GetBus()->ReadProgress(k * 100 / size))
			{
				rval = OP_ABORTED;
				break;
			}
		}
	}

	GetBus()->ReadEnd();

	delete[] localbuf;
	return rval;
}


int E24xx2::SecurityRead(uint32_t &blocks)
{
	int rv = Probe();       //Determina gli indirizzi I2C

	if (rv > 0)
	{
		uint8_t buf[4];

		buf[0] = 0xFF;
		buf[1] = 0xFF;
		buf[2] = SecurityReadCode;

		if (GetBus()->StartWrite(eeprom_addr[0], buf, 3) != 3)
		{
			return GetBus()->Error();
		}

		int val;

		if ((val = GetBus()->ReadByte(0)) < 0)
		{
			return GetBus()->Error();
		}

		blocks = (val << 4) & 0xF0;

		if ((val = GetBus()->ReadByte(1)) < 0)
		{
			return GetBus()->Error();
		}

		blocks |= (val & 0x0F);

		GetBus()->Stop();

		rv = OK;
	}
	else
	{
		rv = IICERR_NOADDRACK;
	}

	return rv;
}

int E24xx2::SecurityWrite(uint32_t blocks)
{
	int rv = Probe();       //Determina gli indirizzi I2C

	if (rv > 0)
	{
		uint8_t buf[4];

		int start_block = (blocks >> 4) & 0x0F;
		int no_of_block = blocks & 0x0F;

		buf[0] = 0x80 | (start_block << 1);
		buf[1] = 0xFF;
		buf[2] = SecurityWriteCode | (no_of_block & 0x3F);

		if (GetBus()->Write(eeprom_addr[0], buf, 3) != 3)
		{
			return GetBus()->Error();
		}

		rv = OK;
	}
	else
	{
		rv = IICERR_NOADDRACK;
	}

	return rv;
}

int E24xx2::HighEnduranceRead(uint32_t &block_no)
{
	int rv = Probe();       //Determina gli indirizzi I2C

	if (rv > 0)
	{
		uint8_t buf[4];

		buf[0] = 0xFF;
		buf[1] = 0xFF;
		buf[2] = HEnduranceReadCode;

		if (GetBus()->StartWrite(eeprom_addr[0], buf, 3) != 3)
		{
			return GetBus()->Error();
		}

		int val;

		if ((val = GetBus()->ReadByte(1)) < 0)
		{
			return GetBus()->Error();
		}

		block_no = val & 0x0F;

		GetBus()->Stop();

		rv = OK;
	}
	else
	{
		rv = IICERR_NOADDRACK;
	}

	return rv;
}

int E24xx2::HighEnduranceWrite(uint32_t block_no)
{
	int rv = Probe();       //Determina gli indirizzi I2C

	if (rv > 0)
	{
		uint8_t buf[4];

		buf[0] = 0x80 | (uint8_t)(block_no << 1);
		buf[1] = 0xFF;
		buf[2] = HEnduranceWriteCode;

		if (GetBus()->Write(eeprom_addr[0], buf, 3) != 3)
		{
			return GetBus()->Error();
		}

		rv = OK;
	}
	else
	{
		rv = IICERR_NOADDRACK;
	}

	return rv;
}
