//=========================================================================//
//-------------------------------------------------------------------------//
// eeprom.cpp -- Source for EEProm class (base class for every device)     //
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
#include "eeprom.h"		// Header file
#include "e2awinfo.h"

//=====>>> Costruttore <<<======
EEProm::EEProm(e2AppWinInfo *wininfo, BusIO *busp, int b_size)
	:	awi(wininfo),
		bus(busp),
		def_bank_size(b_size)
{
	DefaultBankSize();
}

//--- Distruttore
EEProm::~EEProm()
{
}

void EEProm::SetAWInfo(e2AppWinInfo *wininfo)
{
	if (wininfo)
		awi = wininfo;
}

int EEProm::GetAddrSize() const
{
	return GetEEPAddrSize(awi->GetEEPPriType(), awi->GetEEPSubType());
}

int EEProm::GetNoOfBank() const
{
	return awi->GetNoOfBlock();
}

void EEProm::SetNoOfBank(int no)
{
	if (no >= 0)
		awi->SetNoOfBlock(no);
}

BYTE *EEProm::GetBufPtr() const
{
	return (BYTE *)awi->GetBufPtr();
}

int EEProm::GetBufSize() const
{
	return awi->GetBufSize();
}

int EEProm::GetSplitted() const
{
	return awi->GetSplittedInfo();
}

void EEProm::SetSplitted(int split)
{
	if (split > 0)
		awi->SetSplittedInfo(split);
}

int EEProm::Erase()
{
	return GetBus()->Erase();
}
