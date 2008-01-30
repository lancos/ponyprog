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

#ifndef	_AT89SBUS_H
#define	_AT89SBUS_H

#include "spi-bus.h"

class At89sBus : public SPIBus
{
 public:		//------------------------------- public
	At89sBus(BusInterface *ptr = 0);
//	virtual ~At89sBus();

	long Read(int addr, UBYTE *data, long length, int page_size = 0);
	long Write(int addr, UBYTE const *data, long length, int page_size = 0);

	virtual int Reset();
	virtual int Erase(int type = 0);

	int ReadDeviceCode(int addr);
	int WriteLockBits(DWORD val, long model);
	int WriteFuseBits(DWORD val, long model);
	int ReadLockBits(DWORD &res, long model);
	int ReadFuseBits(DWORD &res, long model);
	void SetDelay();

	void SetPagePolling(bool prog, bool val)
	{
		if (prog)
			enable_progpage_polling = val;
		else
			enable_datapage_polling = val;
	}
	bool GetPagePolling(bool prog) const
	{ return prog ? enable_progpage_polling : enable_datapage_polling; }
	void SetCompatibilityMode(bool old)
	{ oldmode = old; }

 protected:		//------------------------------- protected

	//Programming commands
	const BYTE EnableProg0, EnableProg1;
	const BYTE ChipErase0, ChipErase1, OldChipErase1;
	const BYTE ReadProgByteMem, OldReadProgMem;
	const BYTE WriteProgByteMem, OldWriteProgMem;
	const BYTE ReadProgPageMem;
	const BYTE WriteProgPageMem;
	const BYTE ReadDataByteMem, OldReadDataMem;
	const BYTE WriteDataByteMem, OldWriteDataMem;
	const BYTE ReadDataPageMem;
	const BYTE WriteDataPageMem;
	const BYTE ReadUserFuses0, ReadUserFuses1;
	const BYTE WriteUserFuses0, WriteUserFuses1;
	const BYTE ReadLockBits0, ReadLockBits1;
	const BYTE WriteLockBits0, WriteLockBits1, OldWriteLockBits1;
	const BYTE ReadSignatureByte;

	int ReadDataByte(long addr);
	void WriteDataByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);
	void WriteProgPage(long addr, UBYTE const *data, long page_size, long timeout = 5000);
	void WriteDataPage(long addr, UBYTE const *data, long page_size, long timeout = 5000);
	void ReadProgPage(long addr, UBYTE *data, long page_size, long timeout = 5000);
	void ReadDataPage(long addr, UBYTE *data, long page_size, long timeout = 5000);

	int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 5000);
	bool CheckBlankPage(UBYTE const *data, long length);

 private:		//------------------------------- private

	bool enable_datapage_polling, enable_progpage_polling;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

	bool oldmode;
};

#endif
