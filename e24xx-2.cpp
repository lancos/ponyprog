//=========================================================================//
//-------------------------------------------------------------------------//
// e24xx-2.cpp -- Source for E24xx2 class (I2C Bus "large" eeproms)        //
// 24XX-2 are I2C Bus eeproms that uses 16 bits (2 bytes) for addresses.   //
// Total capacity can be up to 64KBytes.                                   //
//                                                                         //
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

#include <string.h>
#include "types.h"
#include "e2app.h"
#include "e24xx-2.h"		// Header file
#include "errcode.h"
#include "eeptypes.h"

//=====>>> Costruttore <<<======
E24xx2::E24xx2(e2AppWinInfo *wininfo, BusIO *busp)
	:	E24xx(wininfo, busp),
		SecurityReadCode(0xC0),
		SecurityWriteCode(0x80),
		HEnduranceReadCode(0x40),
		HEnduranceWriteCode(0x00)
{
	writepage_size = THEAPP->GetI2CPageWrite();
	THEAPP->SetI2CPageWrite(writepage_size);
}

//--- Distruttore
E24xx2::~E24xx2()
{
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

/**
int E24xx2::Read(int probe)
{
	int error;
	
	Probe();

	if (n_bank)
	{
		error = bank_in(GetBufPtr(), 0, GetBankSize() * GetNoOfBank());
		if (error)
		{
	//		GetBus()->Open(THEAPP->GetPort());
			return error;
		}
	}

	return GetNoOfBank();
}

int E24xx2::Write(int probe)
{
	int error;

//	Probe( probe || GetNoOfBank() == 0 );
	Probe();

	// programma la eeprom
	if (n_bank && GetNoOfBank())
	{
		error = bank_out(GetBufPtr(), 0, GetBankSize() * GetNoOfBank());
		if (error)
		{
	//		GetBus()->Open(THEAPP->GetPort());
			return error;
		}
		else
			return GetNoOfBank();
	}
	else
		return 0;
}

int E24xx2::Verify()
{
	Probe();	//Determina gli indirizzi I2C

	if (GetNoOfBank() == 0 || n_bank == 0)
		return NOTHINGTOSAVE;

	long size = GetBankSize() * GetNoOfBank();
	unsigned char *localbuf = new unsigned char[size];
	if (localbuf == 0)
		return OUTOFMEMORY;

	int error, rval = 1;
	// legge il contenuto attuale della EEPROM in memoria

	if ( (error = bank_in(localbuf, 0, size)) )
	{
	//	GetBus()->Open(THEAPP->GetPort());
		rval = error;
	}
	else
	{
		rval = ( memcmp(GetBufPtr(), localbuf, size) == 0 );
	}
	delete localbuf;
	return rval;
}
**/

// questa routine si aspetta che in ingresso ci siano SIZE bytes da programmare l'eeprom
// memorizzati nel copy_buf nelle locazioni da 2 (non da 0!) a SIZE+1
int E24xx2::bank_out(BYTE const *copy_buf, int bank, long size, long idx)
{
	if (copy_buf == 0 || n_bank == 0)
		return BADPARAM;

	if (size <= 0)
		size = GetBankSize();

	unsigned char *localbuf = new unsigned char[writepage_size+2];
	if (localbuf == 0)
		return OUTOFMEMORY;

	int rval = OK;
	long j;
	for (j = 0; j < size; j += writepage_size)
	{
		memcpy(localbuf+2, copy_buf+j, writepage_size);

		localbuf[0] = (BYTE)((j+idx) >> 8);
		localbuf[1] = (BYTE)((j+idx) & 0xFF);
		if ( GetBus()->Write(eeprom_addr[bank], localbuf, 2+writepage_size) != (2+writepage_size) )
		{
			rval = GetBus()->Error();
			break;
		}

		int k;
		for (k = timeout_loop; k > 0 && GetBus()->Read(eeprom_addr[bank], localbuf, 1) != 1; k--)
			;
		if (k == 0)
		{
			rval = E2P_TIMEOUT;
			break;
		}

		if ( GetBus()->CheckAbort(j * 100 / size) )
			return OP_ABORTED;
	}
	GetBus()->CheckAbort(100);

	delete localbuf;
	return rval;
}

int E24xx2::bank_in(BYTE *copy_buf, int bank, long size, long idx)
{
	BYTE index[2];

	if (copy_buf == 0 || n_bank == 0)
		return BADPARAM;

	if (size <= 0)
		size = GetBankSize();

#if 1
	//Scrive l'indice del sottoindirizzamento
	// partiamo sempre da 0.
	index[0] = (BYTE)(idx >> 8);
	index[1] = (BYTE)(idx & 0xFF);
	if (GetBus()->StartWrite(eeprom_addr[bank], index, 2) != 2)
		return GetBus()->Error();

	if (GetBus()->Read(eeprom_addr[bank], copy_buf, size) < size)
		return GetBus()->Error();
#else
	int readpage_size = 1024;
	long k;
	for (k = 0; k < size; k += readpage_size)
	{
		//Scrive l'indice del sottoindirizzamento
		// partiamo sempre da 0.
		index[0] = (BYTE)(((k+idx) >> 8) & 0xFF);
		index[1] = (BYTE)( (k+idx) & 0xFF );

		if (GetBus()->StartWrite(eeprom_addr[bank], index, 2) != 2)
			return GetBus()->Error();

		if (GetBus()->Read(eeprom_addr[bank], copy_buf+k, readpage_size) < (ULONG)readpage_size)
			return GetBus()->Error();

		if ( GetBus()->CheckAbort(k * 100 / size) )
			return OP_ABORTED;
	}

	GetBus()->CheckAbort(100);
#endif

	return OK;
}

int E24xx2::SecurityRead(int &blocks)
{
	int rv = Probe();	//Determina gli indirizzi I2C

	if (rv > 0)
	{
		BYTE buf[4];

		buf[0] = 0xFF;
		buf[1] = 0xFF;
		buf[2] = SecurityReadCode;
		if (GetBus()->StartWrite(eeprom_addr[0], buf, 3) != 3)
			return GetBus()->Error();

		int val;
		if ( (val = GetBus()->ReadByte(0)) < 0 )
			return GetBus()->Error();
		blocks = (val << 4) & 0xF0;

		if ( (val = GetBus()->ReadByte(1)) < 0 )
			return GetBus()->Error();
		blocks |= (val & 0x0F);

		GetBus()->Stop();

		rv = OK;
	}
	else
		rv = IICERR_NOADDRACK;

	return rv;
}

int E24xx2::SecurityWrite(int blocks)
{
	int rv = Probe();	//Determina gli indirizzi I2C

	if (rv > 0)
	{
		BYTE buf[4];

		int start_block = (blocks >> 4) & 0x0F;
		int no_of_block = blocks & 0x0F;

		buf[0] = 0x80 | (start_block << 1);
		buf[1] = 0xFF;
		buf[2] = SecurityWriteCode | (no_of_block & 0x3F);
		if (GetBus()->Write(eeprom_addr[0], buf, 3) != 3)
			return GetBus()->Error();

		rv = OK;
	}
	else
		rv = IICERR_NOADDRACK;

	return rv;
}

int E24xx2::HighEnduranceRead(int &block_no)
{
	int rv = Probe();	//Determina gli indirizzi I2C

	if (rv > 0)
	{
		BYTE buf[4];

		buf[0] = 0xFF;
		buf[1] = 0xFF;
		buf[2] = HEnduranceReadCode;
		if (GetBus()->StartWrite(eeprom_addr[0], buf, 3) != 3)
			return GetBus()->Error();

		int val;
		if ( (val = GetBus()->ReadByte(1)) < 0 )
			return GetBus()->Error();
		block_no = val & 0x0F;

		GetBus()->Stop();

		rv = OK;
	}
	else
		rv = IICERR_NOADDRACK;

	return rv;
}

int E24xx2::HighEnduranceWrite(int block_no)
{
	int rv = Probe();	//Determina gli indirizzi I2C

	if (rv > 0)
	{
		BYTE buf[4];

		buf[0] = 0x80 | (block_no << 1);
		buf[1] = 0xFF;
		buf[2] = HEnduranceWriteCode;
		if (GetBus()->Write(eeprom_addr[0], buf, 3) != 3)
			return GetBus()->Error();
		rv = OK;
	}
	else
		rv = IICERR_NOADDRACK;

	return rv;
}
