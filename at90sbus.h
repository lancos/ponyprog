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

#ifndef	_AT90SBUS_H
#define	_AT90SBUS_H

#include "spi-bus.h"

class At90sBus : public SPIBus
{
 public:		//------------------------------- public
	At90sBus(BusInterface *ptr = 0);
//	virtual ~At90sBus();

	long Read(int addr, UBYTE *data, long length, int page_size = 0);
	long Write(int addr, UBYTE const *data, long length, int page_size = 0);

	virtual int Reset();
	virtual int Erase(int type = 0);

	virtual int ReadDeviceCode(int addr);
	virtual int WriteLockBits(DWORD byte, long model = 0);
	virtual int WriteFuseBits(DWORD byte, long model = 0);
	virtual DWORD ReadLockBits(long model = 0);
	virtual DWORD ReadFuseBits(long model = 0);
	virtual long ReadCalibration(int addr = 0);

	void SetFlashPagePolling(bool val);
	bool GetFlashPagePolling() const;
	void SetOld1200Mode(bool val)
	{ old1200mode = val; }

 protected:		//------------------------------- protected

	//Programming commands
	const BYTE EnableProg0, EnableProg1;
	const BYTE ChipErase0, ChipErase1;
	const BYTE ReadProgMemH0, ReadProgMemH1;
	const BYTE ReadProgMemL0, ReadProgMemL1;
	const BYTE WriteProgMemH0, WriteProgMemH1;
	const BYTE WriteProgMemL0, WriteProgMemL1;
	const BYTE WriteProgPageMem;
	const BYTE ReadEEPMem0, ReadEEPMem1;
	const BYTE WriteEEPMem0, WriteEEPMem1;
	const BYTE ReadDevCode0, ReadDevCode1;
	const BYTE ReadLock0, ReadLock1;
	const BYTE WriteLock0, WriteLock1;
	const BYTE ReadFuse0, ReadFuse1;
	const BYTE WriteFuse0, WriteFuse1a, WriteFuse1b;
	const BYTE ReadFuseHigh0, ReadFuseHigh1;
	const BYTE WriteFuseHigh0, WriteFuseHigh1;
	const BYTE ReadFuseExt0, ReadFuseExt1;
	const BYTE WriteFuseExt0, WriteFuseExt1;
	const BYTE ReadCalib0, ReadCalib1;

	int ReadEEPByte(long addr);
	void WriteEEPByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);
	int WriteProgPage(long addr, UBYTE const *data, long page_size, long timeout = 10000);

	int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 10000);
	bool CheckBlankPage(UBYTE const *data, ULONG length);
	void RefreshParameters();

	//Polling codes
	const BYTE p1_a, p2_a, pflash_a;
	const BYTE p1_b, p2_b, pflash_b;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

	bool old1200mode;

 private:		//------------------------------- private

	bool enable_flashpage_polling;
};

#endif
