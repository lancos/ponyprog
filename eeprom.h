//=========================================================================//
//-------------------------------------------------------------------------//
// eeprom.h -- Header for for EEprom class                                 //
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

#ifndef _EEPROM_H
#define _EEPROM_H

#include "types.h"
#include "globals.h"
#include "busio.h"

class e2AppWinInfo;

//I'm too lazy to change the name, now should be Device class
class EEProm
{
  public:		//---------------------------------------- public

	EEProm(e2AppWinInfo *wininfo = 0, BusIO *busp = 0, int b_size = 0);
	virtual ~EEProm();

	virtual int Probe(int probe_size = 0) = 0;
	virtual int Read(int probe = 1) = 0;
	virtual int Write(int probe = 1) = 0;
	virtual int Verify() = 0;
	virtual int BankRollOverDetect(int force)
		{ return 2; }	//2 stand for no Bank Rollover

	virtual int Erase();

	//aggiunto il 07/03/98
	virtual int SecurityRead(int &blocks, int &bits)
		{ bits = -1; return 0; }
	virtual int SecurityWrite(int blocks, int bits)
		{ return 0; }
	virtual int HighEnduranceRead(int &block_no)
		{ return 0; }
	virtual int HighEnduranceWrite(int block_no)
		{ return 0; }
	//added 13/09/99
	virtual int FusesRead(int &bits)
		{ bits = -1; return 0; }
	virtual int FusesWrite(int bits)
		{ return 0; }
	//--------
	void SetAWInfo(e2AppWinInfo *wininfo);
	BusIO *GetBus() const
		{ return bus; }
	void SetBus(BusIO *busp)
		{ if (busp) bus = busp; }
	int GetNoOfBank() const;
	void SetNoOfBank(int no);
	int GetBankSize() const
		{ return bank_size; }
	int GetAddrSize() const;
	void DefaultBankSize()
		{ bank_size = def_bank_size; }

  protected:	//--------------------------------------- protected
	void SetBankSize(int size)
		{ if (size > 0) bank_size = size; }
	BYTE *GetBufPtr() const;
	int GetBufSize() const;
	void SetSplitted(int split);
	int GetSplitted() const;
	long GetSize() const
	{
		long size = GetNoOfBank() * bank_size;
		return size == AUTOSIZE_ID ? 0 : size;
	}

	e2AppWinInfo *GetAWInfo() const
		{ return awi; }


  private:		//--------------------------------------- private

	e2AppWinInfo *awi;	// pointer to container object
	BusIO *bus;			// bus used by the device
	int bank_size;		// dimensione di un banco

	int const def_bank_size;	// dimensione di default di un banco
};
#endif
