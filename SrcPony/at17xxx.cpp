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
// Source for AT1765/... class (FPGA eeproms)

#include <QString>

#include "types.h"
#include "e2cmdw.h"
#include "at17xxx.h"            // Header file
#include "errcode.h"
#include "eeptypes.h"

//=====>>> Costruttore <<<======
At17xxx::At17xxx(e2AppWinInfo *wininfo, BusIO *busp)
	:       E24xx(wininfo, busp, 8, 1)
{
	writepage_size = 64;
}

int At17xxx::WritePage(long addr, int addr_bytes, uint8_t *buf, int len)
{
	int j;
	int rval;

	//Write Page
	rval = GetBus()->Start(eeprom_addr[0] & ~1);

	if (rval < 0)
	{
		return rval;
	}

	for (j = addr_bytes - 1; j >= 0; j--)
	{
		rval = GetBus()->WriteByte((addr >> (j * 8)) & 0xFF, 0);

		if (rval < 0)
		{
			return rval;
		}
	}

	for (j = 0; j < len; j++)
	{
		rval = GetBus()->WriteByte(buf[j], 1);

		if (rval < 0)
		{
			return rval;
		}
	}

	GetBus()->Stop();

	//Data polling
	for (j = timeout_loop; j > 0 && GetBus()->Start(eeprom_addr[0] & ~1) < 0; j--)
		;;

	if (j == 0)
	{
		return E2P_TIMEOUT;
	}

	return OK;
}

int At17xxx::Write(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	GetBus()->WriteStart();

	int addr_bytes;
	long size = GetSize();

	if (size > 0xffff)
	{
		//512/1024 kbits
		addr_bytes = 3;
		writepage_size = 128;
	}
	else
	{
		//65/128/256 kbits
		addr_bytes = 2;
		writepage_size = 64;
	}

	int rval = OK;

	if (type & PROG_TYPE)
	{
		long j;

		for (j = 0; j < size; j += writepage_size)
		{
			rval = WritePage(j, addr_bytes, GetBufPtr() + j, writepage_size);

			if (rval != OK)
			{
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

	if (rval == OK)
	{
		return GetSize();
	}
	else
	{
		return rval;
	}
}


int At17xxx::ReadPage(long addr, int addr_bytes, uint8_t *buf, int len)
{
	int j;
	int rval;

	if (len < 1)
	{
		return BADPARAM;
	}

	//Read Page
	rval = GetBus()->Start(eeprom_addr[0] & ~1);

	if (rval < 0)
	{
		return rval;
	}

	for (j = addr_bytes - 1; j >= 0; j--)
	{
		rval = GetBus()->WriteByte((addr >> (j * 8)) & 0xFF, 0);

		if (rval < 0)
		{
			return rval;
		}
	}

	//Read Bytes
	rval = GetBus()->Start(eeprom_addr[0] | 1);

	if (rval < 0)
	{
		return rval;
	}

	for (j = 0; j < len - 1; j++)
	{
		rval = GetBus()->ReadByte(0, 1);

		if (rval < 0)
		{
			return rval;
		}

		*buf++ = (uint8_t)rval;
	}

	//ultimo byte senza ACK
	rval = GetBus()->ReadByte(1, 1);

	if (rval < 0)
	{
		return rval;
	}

	*buf++ = (uint8_t)rval;

	GetBus()->Stop();

	return OK;
}

int At17xxx::Read(int probe, int type)
{
	int error = Probe(probe || GetNoOfBank() == 0);

	if (error < 0)
	{
		return error;
	}

	GetBus()->ReadStart();

	int readpage_size;
	int addr_bytes;
	long size = GetSize();

	if (size > 0xffff)
	{
		//512/1024 kbits
		addr_bytes = 3;
		readpage_size = 128;
	}
	else
	{
		//65/128/256 kbits
		addr_bytes = 2;
		readpage_size = 64;
	}

	error = OK;

	if (type & PROG_TYPE)
	{
		long k;

		for (k = 0; k < size; k += readpage_size)
		{
			error = ReadPage(k, addr_bytes, GetBufPtr() + k, readpage_size);

			if (error != OK)
			{
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

int At17xxx::Verify(int type)
{
	int rval = Probe();     //Moved here from 7 lines above (10/12/99)

	if (rval < 0)
	{
		return rval;
	}

	int readpage_size;
	int addr_bytes;
	long size = GetSize();

	if (size > 0xffff)
	{
		//512/1024 kbits
		addr_bytes = 3;
		readpage_size = 128;
	}
	else
	{
		//65/128/256 kbits
		addr_bytes = 2;
		readpage_size = 64;
	}

	unsigned char *localbuf = new unsigned char[readpage_size];

	if (localbuf == 0)
	{
		return OUTOFMEMORY;
	}

	GetBus()->ReadStart();

	rval = 1;               //true

	if (type & PROG_TYPE)
	{
		long k;

		for (k = 0; k < size; k += readpage_size)
		{
			rval = ReadPage(k, addr_bytes, localbuf, readpage_size);

			if (rval != OK)
			{
				break;
			}

			if (memcmp(GetBufPtr() + k, localbuf, readpage_size) != 0)
			{
				rval = 0;
				break;
			}
			else
			{
				rval = 1;
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
