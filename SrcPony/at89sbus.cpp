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
#include "at89sbus.h"
#include "errcode.h"
#include "eeptypes.h"
#include "e2profil.h"

#include "e2cmdw.h"


#include <QDebug>

/*Attenzione!! il format Intel Hex e` Little Endian */
#undef  _BIG_ENDIAN_

#ifndef __linux__
#  ifdef        __BORLANDC__
#    define     __inline__
#  else // _MICROSOFT_ VC++
#    define     __inline__ __inline
#    define _export
#  endif
#endif

// Costruttore
At89sBus::At89sBus(BusInterface *ptr)
	: SPIBus(ptr),
	  EnableProg0(0xAC), EnableProg1(0x53),
	  ChipErase0(0xAC), ChipErase1(0x80), OldChipErase1(0x04),
	  ReadProgByteMem(0x20), OldReadProgMem(0x01),
	  WriteProgByteMem(0x40), OldWriteProgMem(0x02),
	  ReadProgPageMem(0x30),
	  WriteProgPageMem(0x50),
	  ReadDataByteMem(0xA0), OldReadDataMem(0x05),
	  WriteDataByteMem(0xC0), OldWriteDataMem(0x06),
	  ReadDataPageMem(0xB0),
	  WriteDataPageMem(0xD0),
	  ReadUserFuses0(0x21), ReadUserFuses1(0x00),
	  WriteUserFuses0(0xAC), WriteUserFuses1(0x10),
	  ReadLockBits0(0x24), ReadLockBits1(0x00),
	  WriteLockBits0(0xAC), WriteLockBits1(0xE0), OldWriteLockBits1(0x07),
	  ReadSignatureByte(0x28),
	  twd_erase(50),
	  oldmode(true)
{
}

void At89sBus::SetDelay()
{
	int val = E2Profile::GetSPISpeed();
	int n;

	switch (val)
	{
	case TURBO:
		n = 1;
		break;

	case FAST:
		n = 5;
		break;

	case SLOW:
		n = 30;
		break;

	case VERYSLOW:
		n = 100;
		break;

	case ULTRASLOW:
		n = 1000;
		break;

	default:
		n = 15;
		break;
	}

	BusIO::SetDelay(n);

	qDebug() << "At89sBus::SetDelay() = " << n;
}

int At89sBus::ReadDataByte(long addr)
{
	if (oldmode)
	{
		SendDataByte(OldReadDataMem | ((addr >> 5) & 0xF8));
	}
	else
	{
		SendDataByte(ReadDataByteMem);
		SendDataByte(addr >> 8);
	}

	SendDataByte(addr & 0xFF);
	return RecDataByte();
}

void At89sBus::WriteDataByte(long addr, int data)
{
	if (oldmode)
	{
		SendDataByte(OldWriteDataMem | ((addr >> 5) & 0xF8));
	}
	else
	{
		SendDataByte(WriteDataByteMem);
		SendDataByte(addr >> 8);
	}

	SendDataByte(addr & 0xFF);
	SendDataByte(data);
}


int At89sBus::ReadProgByte(long addr)
{
	if (oldmode)
	{
		SendDataByte(OldReadProgMem | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04));
	}
	else
	{
		SendDataByte(ReadProgByteMem);
		SendDataByte(addr >> 8);
	}

	SendDataByte(addr & 0xFF);
	return RecDataByte();
}

void At89sBus::WriteProgByte(long addr, int data)
{
	SetLastProgrammedAddress(addr);

	if (oldmode)
	{
		SendDataByte(OldWriteProgMem | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04));
	}
	else
	{
		SendDataByte(WriteProgByteMem);
		SendDataByte(addr >> 8);
	}

	SendDataByte(addr & 0xFF);
	SendDataByte(data);
}

int At89sBus::WriteProgPage(long addr, uint8_t const *data, long page_size, long timeout)
{
	long k;
	bool okflag;

	if (page_size <= 0 || data == NULL)
	{
		return BADPARAM;
	}

	//align addr to page boundary
	addr &= ~(page_size - 1);       //0xFFFFFF00

	SendDataByte(WriteProgPageMem);
	SendDataByte(addr >> 8);
	SendDataByte(addr & 0xff);

	for (k = 0; k < page_size; k++)
	{
		SendDataByte(data[k]);
	}

	SetLastProgrammedAddress(addr + page_size - 1);

	if (enable_progpage_polling)
	{
		long polling_loc = addr + page_size - 1;        //Read back last loaded byte
		uint8_t polling_data = data[page_size - 1];
		WaitUsec(100);

		okflag = false;

		for (k = timeout; k > 0; k--)
		{
			if (ReadProgByte(polling_loc) == polling_data)
			{
				okflag = true;
				break;
			}
		}
	}
	else
	{
		okflag = true;
		WaitMsec(twd_prog);
	}

	return okflag ? OK : E2P_TIMEOUT;
}

int At89sBus::WriteDataPage(long addr, uint8_t const *data, long page_size, long timeout)
{
	long k;
	bool okflag;

	//align addr to page boundary
	addr &= ~(page_size - 1);       //0xFFFFFF00

	SendDataByte(WriteDataPageMem);
	SendDataByte(addr >> 8);
	SendDataByte(addr & 0xff);

	for (k = 0; k < page_size; k++)
	{
		SendDataByte(data[k]);
	}

	if (enable_datapage_polling)
	{
		long polling_loc = addr + page_size - 1;        //Read back last loaded byte
		uint8_t polling_data = data[page_size - 1];
		WaitUsec(100);

		okflag = false;

		for (k = timeout; k > 0; k--)
		{
			if (ReadDataByte(polling_loc) == polling_data)
			{
				okflag = true;
				break;
			}
		}
	}
	else
	{
		okflag = true;
		WaitMsec(twd_prog);
	}

	return okflag ? OK : -1;
}

void At89sBus::ReadProgPage(long addr, uint8_t *data, long page_size, long timeout)
{
	long k;

	//align addr to page boundary
	addr &= ~(page_size - 1);       //0xFFFFFF00

	SendDataByte(ReadProgPageMem);
	SendDataByte(addr >> 8);
	SendDataByte(addr & 0xff);

	for (k = 0; k < page_size; k++)
	{
		data[k] = RecDataByte();
	}
}

void At89sBus::ReadDataPage(long addr, uint8_t *data, long page_size, long timeout)
{
	long k;

	//align addr to page boundary
	addr &= ~(page_size - 1);       //0xFFFFFF00

	SendDataByte(ReadDataPageMem);
	SendDataByte(addr >> 8);
	SendDataByte(addr & 0xff);

	for (k = 0; k < page_size; k++)
	{
		data[k] = RecDataByte();
	}
}

int At89sBus::Reset()
{
	if (oldmode)
	{
		twd_prog = 20;
	}
	else
	{
		twd_prog = 5;
	}

	SPIBus::Reset();
	WaitMsec(E2Profile::GetAT89DelayAfterReset());     // Almeno 20msec dai datasheet AVR atmel

	SendDataByte(EnableProg0);
	SendDataByte(EnableProg1);
	SendDataByte(0);

	if (!oldmode)
	{
		SendDataByte(0);
	}

	return OK;
}

int At89sBus::WriteLockBits(uint32_t param, long model)
{
	int val1, val2, val3, val4;

	val1 = -1;
	val2 = val3 = val4 = 0;

	switch (model)
	{
	case AT89S8253:
		val1 = WriteLockBits0;
		val2 = WriteLockBits1 | (~param & 0x07);
		break;

	case AT89S51:
	case AT89S52:
		//Translate from B1-B2 code to LB1-LB2-LB3
		//...
		break;

	default:
		val1 = WriteLockBits0;
		val2 = OldWriteLockBits1;
		val2 |= 0xff & ~param;
		break;
	}

	if (val1 != -1)
	{
		SendDataByte(val1);
		SendDataByte(val2);
		SendDataByte(val3);

		if (oldmode)
		{
			WaitMsec(twd_prog * 5);
		}
		else
		{
			SendDataByte(val4);
			WaitMsec(twd_prog * 10);
		}

		return OK;
	}
	else
	{
		return NOTSUPPORTED;
	}
}

int At89sBus::ReadLockBits(uint32_t &res, long model)
{
	int rval = OK;
	uint32_t rv1;

	switch (model)
	{
	case AT89S8253:
		SendDataByte(ReadLockBits0);
		SendDataByte(ReadLockBits1);
		SendDataByte(0);
		rv1 = RecDataByte();
		res = ~rv1 & 0x07;
		break;

	case AT89S51:
	case AT89S52:
		//NB.Different polarity from other devices: 1 mean programmed (should update message in the dialog)
		SendDataByte(ReadLockBits0);
		SendDataByte(ReadLockBits1);
		SendDataByte(0);
		rv1 = RecDataByte();
		res = rv1 & 0x1C;
		break;

	default:
		rval = NOTSUPPORTED;
		break;
	}

	return rval;
}

int At89sBus::WriteFuseBits(uint32_t param, long model)
{
	int val1, val2, val3, val4;

	val1 = -1;
	val2 = val3 = val4 = 0;

	switch (model)
	{
	case AT89S8253:
		val1 = WriteUserFuses0;
		val2 = WriteUserFuses1 | (~param & 0x0f);
		break;

	default:
		//No Fuses
		break;
	}

	if (val1 != -1)
	{
		SendDataByte(val1);
		SendDataByte(val2);
		SendDataByte(val3);
		SendDataByte(val4);

		WaitMsec(twd_prog * 10);
		return OK;
	}
	else
	{
		return NOTSUPPORTED;
	}
}

int At89sBus::ReadFuseBits(uint32_t &res, long model)
{
	int rval = OK;
	uint32_t rv1;

	switch (model)
	{
	case AT89S8253:
		SendDataByte(ReadUserFuses0);
		SendDataByte(ReadUserFuses1);
		SendDataByte(0);
		rv1 = RecDataByte();
		res = ~rv1 & 0x0f;
		break;

	default:        //No Fuses
		rval = NOTSUPPORTED;
		break;
	}

	return rval;
}

int At89sBus::ReadDeviceCode(int addr)
{
	SendDataByte(ReadSignatureByte);
	SendDataByte(addr >> 8);
	SendDataByte(addr & 0xff);

	return RecDataByte();
}

int At89sBus::Erase(int type)
{
	//Erase command
	SendDataByte(ChipErase0);

	if (oldmode)
	{
		SendDataByte(OldChipErase1);
		SendDataByte(0);
	}
	else
	{
		SendDataByte(ChipErase1);
		SendDataByte(0);
		SendDataByte(0);
	}

	WaitMsec(twd_erase);
	Reset();

	return 1;
}

long At89sBus::Read(int addr, uint8_t *data, long length, int page_size)
{
	long len;

	ReadStart();

	if (addr)
	{
		//Data
		if (page_size > 1)
		{
			for (addr = 0, len = 0; len < length; len += page_size, addr += page_size, data += page_size)
			{
				ReadDataPage(addr, data, page_size);

				if (ReadProgress(len * 100 / length))
				{
					break;
				}
			}
		}
		else
		{
			for (addr = 0, len = 0; len < length; len++)
			{
				*data++ = (uint8_t)ReadDataByte(addr++);

				if (ReadProgress(len * 100 / length))
				{
					break;
				}
			}
		}
	}
	else
	{
		//Prog
		if (page_size > 1)
		{
			for (addr = 0, len = 0; len < length; len += page_size, addr += page_size, data += page_size)
			{
				ReadProgPage(addr, data, page_size);

				if (ReadProgress(len * 100 / length))
				{
					break;
				}
			}
		}
		else
		{
			for (addr = 0, len = 0; len < length; len++)
			{
				*data++ = (uint8_t)ReadProgByte(addr++);

				if (ReadProgress(len * 100 / length))
				{
					break;
				}
			}
		}
	}

	ReadEnd();

	return len;
}

int At89sBus::WaitReadyAfterWrite(int type, long addr, int data, long timeout)
{
	int rval = E2P_TIMEOUT;
	int k;

	for (k = 0; k < timeout; k++)
	{
		int val;

		if (type)
		{
			val = ReadDataByte(addr);
		}
		else
		{
			val = ReadProgByte(addr);
		}

		if (val == data)
		{
			rval = OK;
			break;
		}
	}

	return rval;
}

bool At89sBus::CheckBlankPage(uint8_t const *data, long length)
{
	bool blank_page = true;

	while (length--)
	{
		if (*data++ != 0xFF)
		{
			blank_page = false;
			break;
		}
	}

	return blank_page;
}

long At89sBus::Write(int addr, uint8_t const *data, long length, int page_size)
{
	long len;

	WriteStart();

	if (addr)
	{
		//Data
		if (page_size > 1)
		{
			for (addr = 0, len = 0; len < length; len += page_size, addr += page_size, data += page_size)
			{
				if (WriteDataPage(addr, data, page_size) != OK)
				{
					return E2ERR_WRITEFAILED;
				}

				if (WriteProgress(len * 100 / length))
				{
					break;
				}
			}
		}
		else
		{
			for (addr = 0, len = 0; len < length; addr++, data++, len++)
			{
				int val = ReadDataByte(addr);

				if (val != *data)
				{
					if ((val & *data) != *data)
					{
						return E2ERR_BLANKCHECKFAILED;
					}
					else
					{
						WriteDataByte(addr, *data);
						WaitUsec(100);

						//Interrupt the writing and exit (device missing?)
						if (WaitReadyAfterWrite(1, addr, *data) != OK)
						{
							return E2ERR_WRITEFAILED;
						}
					}
				}

				if (WriteProgress(len * 100 / length))
				{
					break;
				}
			}
		}
	}
	else
	{
		//Prog
		if (page_size > 1)
		{
			for (addr = 0, len = 0; len < length; len += page_size, addr += page_size, data += page_size)
			{
				//check for FF's page to skip blank pages
				if (!CheckBlankPage(data, page_size))
					if (WriteProgPage(addr, data, page_size) != OK)
					{
						return E2ERR_WRITEFAILED;
					}

				if (WriteProgress(len * 100 / length))
				{
					break;
				}
			}
		}
		else
		{
			for (addr = 0, len = 0; len < length; addr++, data++, len++)
			{
				int val = ReadProgByte(addr);

				if (val != *data)
				{
					if ((val & *data) != *data)
					{
						return E2ERR_BLANKCHECKFAILED;
					}
					else
					{
						WriteProgByte(addr, *data);
						WaitUsec(100);

						//Interrupt the writing and exit (device missing?)
						if (WaitReadyAfterWrite(0, addr, *data) != OK)
						{
							return E2ERR_WRITEFAILED;
						}
					}
				}

				if (WriteProgress(len * 100 / length))
				{
					break;
				}
			}
		}
	}

	WriteEnd();

	return len;
}
