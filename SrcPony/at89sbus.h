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

#ifndef _AT89SBUS_H
#define _AT89SBUS_H

#include "spi-bus.h"

class At89sBus : public SPIBus
{
  public:
	At89sBus(BusInterface *ptr = 0);
	//virtual ~At89sBus();

	long Read(int addr, quint8 *data, long length, int page_size = 0);
	long Write(int addr, quint8 const *data, long length, int page_size = 0);

	virtual int Reset();
	virtual int Erase(int type = 0);

	int ReadDeviceCode(int addr);
	int WriteLockBits(quint32 val, quint32 model);
	int WriteFuseBits(quint32 val, quint32 model);
	int ReadLockBits(quint32 &res, quint32 model);
	int ReadFuseBits(quint32 &res, quint32 model);
	void SetDelay();

	void SetPagePolling(bool prog, bool val)
	{
		if (prog)
		{
			enable_progpage_polling = val;
		}
		else
		{
			enable_datapage_polling = val;
		}
	}
	bool GetPagePolling(bool prog) const
	{
		return prog ? enable_progpage_polling : enable_datapage_polling;
	}
	void SetCompatibilityMode(bool old)
	{
		oldmode = old;
	}

  protected:

	//Programming commands
	const quint8 EnableProg0, EnableProg1;
	const quint8 ChipErase0, ChipErase1, OldChipErase1;
	const quint8 ReadProgByteMem, OldReadProgMem;
	const quint8 WriteProgByteMem, OldWriteProgMem;
	const quint8 ReadProgPageMem;
	const quint8 WriteProgPageMem;
	const quint8 ReadDataByteMem, OldReadDataMem;
	const quint8 WriteDataByteMem, OldWriteDataMem;
	const quint8 ReadDataPageMem;
	const quint8 WriteDataPageMem;
	const quint8 ReadUserFuses0, ReadUserFuses1;
	const quint8 WriteUserFuses0, WriteUserFuses1;
	const quint8 ReadLockBits0, ReadLockBits1;
	const quint8 WriteLockBits0, WriteLockBits1, OldWriteLockBits1;
	const quint8 ReadSignatureByte;

	int ReadDataByte(long addr);
	void WriteDataByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);
	int WriteProgPage(long addr, quint8 const *data, long page_size, long timeout = 5000);
	int WriteDataPage(long addr, quint8 const *data, long page_size, long timeout = 5000);
	void ReadProgPage(long addr, quint8 *data, long page_size, long timeout = 5000);
	void ReadDataPage(long addr, quint8 *data, long page_size, long timeout = 5000);

	int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 5000);
	bool CheckBlankPage(quint8 const *data, long length);

  private:

	bool enable_datapage_polling, enable_progpage_polling;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

	bool oldmode;
};

#endif
