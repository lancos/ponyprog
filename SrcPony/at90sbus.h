//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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

#ifndef _AT90SBUS_H
#define _AT90SBUS_H

#include "spi-bus.h"

class At90sBus : public SPIBus
{
  public:
	At90sBus(BusInterface *ptr = 0);
	//virtual ~At90sBus();

	long Read(int addr, quint8 *data, long length, int page_size = 0);
	long Write(int addr, quint8 const *data, long length, int page_size = 0);

	virtual int Reset();
	virtual int Erase(int type = 0);

	virtual int ReadDeviceCode(int addr);
	virtual int WriteLockBits(quint32 byte, long model = 0);
	virtual int WriteFuseBits(quint32 byte, long model = 0);
	virtual quint32 ReadLockBits(long model = 0);
	virtual quint32 ReadFuseBits(long model = 0);
	virtual long ReadCalibration(int addr = 0);

	void SetFlashPagePolling(bool val);
	bool GetFlashPagePolling() const;
	void SetOld1200Mode(bool val)
	{
		old1200mode = val;
	}

  protected:

	//Programming commands
	const quint8 EnableProg0, EnableProg1;
	const quint8 ChipErase0, ChipErase1;
	const quint8 ReadProgMemH0, ReadProgMemH1;
	const quint8 ReadProgMemL0, ReadProgMemL1;
	const quint8 WriteProgMemH0, WriteProgMemH1;
	const quint8 WriteProgMemL0, WriteProgMemL1;
	const quint8 WriteProgPageMem;
	const quint8 ReadEEPMem0, ReadEEPMem1;
	const quint8 WriteEEPMem0, WriteEEPMem1;
	const quint8 ReadDevCode0, ReadDevCode1;
	const quint8 ReadLock0, ReadLock1;
	const quint8 WriteLock0, WriteLock1;
	const quint8 ReadFuse0, ReadFuse1;
	const quint8 WriteFuse0, WriteFuse1a, WriteFuse1b;
	const quint8 ReadFuseHigh0, ReadFuseHigh1;
	const quint8 WriteFuseHigh0, WriteFuseHigh1;
	const quint8 ReadFuseExt0, ReadFuseExt1;
	const quint8 WriteFuseExt0, WriteFuseExt1;
	const quint8 ReadCalib0, ReadCalib1;

	int ReadEEPByte(long addr);
	void WriteEEPByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);
	int WriteProgPage(long addr, quint8 const *data, long page_size, long timeout = 10000);

	int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 10000);
	bool CheckBlankPage(quint8 const *data, long length);
	void RefreshParameters();

	//Polling codes
	const quint8 p1_a, p2_a, pflash_a;
	const quint8 p1_b, p2_b, pflash_b;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

	bool old1200mode;

  private:

	bool enable_flashpage_polling;
};

#endif
