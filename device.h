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

#ifndef _DEVICE_H
#define _DEVICE_H

#include "types.h"
#include "globals.h"
#include "busio.h"

class e2AppWinInfo;

class Device
{
  public:		//---------------------------------------- public

	Device(e2AppWinInfo *wininfo = 0, BusIO *busp = 0, int b_size = 0);
	virtual ~Device();

	virtual int Probe(int probe_size = 0)
		{ return OK; }
	virtual int Read(int probe = 1, int type = ALL_TYPE) = 0;
	virtual int Write(int probe = 1, int type = ALL_TYPE) = 0;
	virtual int Verify(int type = ALL_TYPE) = 0;

	virtual int Erase(int probe = 1, int type = ALL_TYPE)
		{ return GetBus()->Erase(); }

	virtual int BankRollOverDetect(int force)
		{ return 4; }	//4 means no need to know Bank Rollover for this device

	virtual int SecurityRead(DWORD &bits)
		{ bits = 0; return 0; }
	virtual int SecurityWrite(DWORD bits)
		{ return 0; }
	virtual int FusesRead(DWORD &bits)
		{ bits = 0; return 0; }
	virtual int FusesWrite(DWORD bits)
		{ return 0; }
	virtual int HighEnduranceRead(DWORD &block_no)
		{ return 0; }
	virtual int HighEnduranceWrite(DWORD block_no)
		{ return 0; }

	virtual int ReadCalibration(int addr = 0);

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
	virtual void DefaultBankSize()
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

	virtual int ReadProg();
	virtual int ReadData();
	virtual int WriteProg();
	virtual int WriteData();
	virtual int VerifyProg(unsigned char *localbuf);
	virtual int VerifyData(unsigned char *localbuf);

	e2AppWinInfo *GetAWInfo() const
		{ return awi; }

  private:		//--------------------------------------- private

	e2AppWinInfo *awi;	// pointer to container object
	BusIO *bus;			// bus used by the device
	int bank_size;		// dimensione di un banco

	int const def_bank_size;	// dimensione di default di un banco
};
#endif
