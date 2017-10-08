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

#ifndef _AT89SBUS_H
#define _AT89SBUS_H

#include "spi-bus.h"

class At89sBus : public SPIBus
{
  public:                //------------------------------- public
	At89sBus(BusInterface *ptr = 0);
	//      virtual ~At89sBus();

	long Read(int addr, uint8_t *data, long length, int page_size = 0);
	long Write(int addr, uint8_t const *data, long length, int page_size = 0);

	virtual int Reset();
	virtual int Erase(int type = 0);

	int ReadDeviceCode(int addr);
	int WriteLockBits(uint32_t val, long model);
	int WriteFuseBits(uint32_t val, long model);
	int ReadLockBits(uint32_t &res, long model);
	int ReadFuseBits(uint32_t &res, long model);
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

  protected:             //------------------------------- protected

	//Programming commands
	const uint8_t EnableProg0, EnableProg1;
	const uint8_t ChipErase0, ChipErase1, OldChipErase1;
	const uint8_t ReadProgByteMem, OldReadProgMem;
	const uint8_t WriteProgByteMem, OldWriteProgMem;
	const uint8_t ReadProgPageMem;
	const uint8_t WriteProgPageMem;
	const uint8_t ReadDataByteMem, OldReadDataMem;
	const uint8_t WriteDataByteMem, OldWriteDataMem;
	const uint8_t ReadDataPageMem;
	const uint8_t WriteDataPageMem;
	const uint8_t ReadUserFuses0, ReadUserFuses1;
	const uint8_t WriteUserFuses0, WriteUserFuses1;
	const uint8_t ReadLockBits0, ReadLockBits1;
	const uint8_t WriteLockBits0, WriteLockBits1, OldWriteLockBits1;
	const uint8_t ReadSignatureByte;

	int ReadDataByte(long addr);
	void WriteDataByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);
	int WriteProgPage(long addr, uint8_t const *data, long page_size, long timeout = 5000);
	int WriteDataPage(long addr, uint8_t const *data, long page_size, long timeout = 5000);
	void ReadProgPage(long addr, uint8_t *data, long page_size, long timeout = 5000);
	void ReadDataPage(long addr, uint8_t *data, long page_size, long timeout = 5000);

	int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 5000);
	bool CheckBlankPage(uint8_t const *data, long length);

  private:               //------------------------------- private

	bool enable_datapage_polling, enable_progpage_polling;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

	bool oldmode;
};

#endif
