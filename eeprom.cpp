//=========================================================================//
//-------------------------------------------------------------------------//
// device.cpp -- Source for Device class (base class for every device)     //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2001  Claudio Lanconelli                            //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
//                                                                         //
//-------------------------------------------------------------------------//
//  $Id$
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
#include "eeprom.h"		// Header file
#include "e2awinfo.h"

//=====>>> Costruttore <<<======
Device::Device(e2AppWinInfo *wininfo, BusIO *busp, int b_size)
	:	awi(wininfo),
		bus(busp),
		def_bank_size(b_size)
{
	DefaultBankSize();
}

//--- Distruttore
Device::~Device()
{
}

void Device::SetAWInfo(e2AppWinInfo *wininfo)
{
	if (wininfo)
		awi = wininfo;
}

int Device::GetAddrSize() const
{
	return GetEEPAddrSize(awi->GetEEPPriType(), awi->GetEEPSubType());
}

int Device::GetNoOfBank() const
{
	return awi->GetNoOfBlock();
}

void Device::SetNoOfBank(int no)
{
	if (no >= 0)
		awi->SetNoOfBlock(no);
}

BYTE *Device::GetBufPtr() const
{
	return (BYTE *)awi->GetBufPtr();
}

int Device::GetBufSize() const
{
	return awi->GetBufSize();
}

int Device::GetSplitted() const
{
	return awi->GetSplittedInfo();
}

void Device::SetSplitted(int split)
{
	if (split > 0)
		awi->SetSplittedInfo(split);
}

//Read Flash program memory
int Device::ReadProg()
{
	int retval;
	int size = GetSplitted();
	int base = 0;

	retval = GetBus()->Read(0, GetBufPtr()+base, size);
	if (retval != size)
	{
		if (retval > 0)
			retval = OP_ABORTED;
	}

	return retval;
}

//Read EEprom data memory
int Device::ReadData()
{
	int retval;
	int size = GetSize() - GetSplitted();
	int base = GetSplitted();

	retval = GetBus()->Read(1, GetBufPtr()+base, size);
	if ( retval != size )
	{
		if (retval > 0)
			retval = OP_ABORTED;
	}

	return retval;
}

//Write Flash program memory
int Device::WriteProg()
{
	int rv;
	int size = GetSplitted();
	int base = 0;

	rv = GetBus()->Write(0, GetBufPtr()+base, size);
	if ( rv != size )
	{
		if (rv > 0)
			rv = OP_ABORTED;
	}

	return rv;
}

//Write EEprom data memory
int Device::WriteData()
{
	int rv;
	int size = GetSize() - GetSplitted();
	int base = GetSplitted();

	rv = GetBus()->Write(1, GetBufPtr()+base, size);
	if ( rv != size )
	{
		if (rv > 0)
			rv = OP_ABORTED;
	}

	return rv;
}

int Device::VerifyProg(unsigned char *localbuf)
{
	int rval = -1;
	int size = GetSplitted();
	int base = 0;

	//Verify only programmed bytes (to save time in big devices)
	long v_len = size;
	if (THEAPP->GetLastProgrammedAddress() > 0 && THEAPP->GetLastProgrammedAddress() < size )
	{
		v_len = THEAPP->GetLastProgrammedAddress() + 1;
		THEAPP->ClearLastProgrammedAddress();		//reset last_programmed_addr, so next verify not preceeded by write verify all the flash
	}
	//Set blank locations to default 0xFF (erased)
	memset(localbuf, 0xFF, size);

	// read the current flash content and store it in localbuf
	rval = GetBus()->Read(0, localbuf, v_len);
	if ( rval != v_len )
	{
		if (rval > 0)
			rval = OP_ABORTED;
	}
	else
		rval = GetBus()->CompareMultiWord(GetBufPtr()+base, localbuf+base, size, 0) == 0 ? OK : 1;

	return rval;
}

int Device::VerifyData(unsigned char *localbuf)
{
	int rval;
	int size = GetSize() - GetSplitted();
	int base = GetSplitted();

	//read current EEPROM content and
	rval = GetBus()->Read(1, localbuf+base, size );
	if ( rval != size )
	{
		if (rval > 0)
			rval = OP_ABORTED;
	}
	else
		rval = GetBus()->CompareMultiWord(GetBufPtr()+base, localbuf+base, size, 1) == 0 ? OK : 1;

	return rval;
}

int Device::ReadCalibration(int addr)
{
	int val;
	val = Probe(0);
	if (val >= 0)
		val = GetBus()->ReadCalibration(addr);
	return val;
}
