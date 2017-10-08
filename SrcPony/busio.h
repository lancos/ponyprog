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

#ifndef _BUSIO_H
#define _BUSIO_H

#include "types.h"
#include "wait.h"
#include "businter.h"
#include "errcode.h"

//Formato di bus generico seriale
class BusIO : public Wait
{
  public:                //------------------------------- public
	BusIO(BusInterface *p = 0);
	//      virtual ~BusIO() { }

	virtual int Open(int port)
	{
		return (err_no = busI->Open(port));
	}
	virtual void Close()
	{
		busI->Close();
	}
	virtual int Error();

	virtual int TestPort(int port)
	{
		(void)port;
		return OK;
	}

	virtual int Reset() = 0;

	virtual long Read(int addr, uint8_t *data, long length, int page_size = 0) = 0;
	virtual long Write(int addr, uint8_t const *data, long length, int page_size = 0) = 0;
	virtual int Erase(int type = 0)
	{
		(void)type;
		return NOTSUPPORTED;
	}

	void ReadStart()
	{
		CheckAbort(0);
	}
	void ReadEnd()
	{
		CheckAbort(100);
	}
	int ReadProgress(int progress)
	{
		return CheckAbort(progress);
	}

	void WriteStart()
	{
		CheckAbort(0);
	}
	void WriteEnd()
	{
		CheckAbort(100);
	}
	int WriteProgress(int progress)
	{
		return CheckAbort(progress);
	}
	void EraseStart()
	{
		CheckAbort(0);
	}
	void EraseEnd()
	{
		CheckAbort(100);
	}

	virtual int ReadDeviceCode(int addr)
	{
		(void)addr;
		return OK;
	}
	virtual int WriteLockBits(uint32_t val, long model = 0)
	{
		(void)val;
		(void)model;
		return OK;
	}
	virtual int WriteFuseBits(uint32_t val, long model = 0)
	{
		(void)val;
		(void)model;
		return OK;
	}
	virtual uint32_t ReadLockBits(long model = 0)
	{
		(void)model;
		return 0;
	}
	virtual uint32_t ReadFuseBits(long model = 0)
	{
		(void)model;
		return 0;
	}

	virtual long ReadCalibration(int addr = 0)
	{
		(void)addr;
		return -1;        //No calibration value available
	}

	virtual int CompareMultiWord(uint8_t *data1, uint8_t *data2, long length, int split)
	{
		(void)split;
		return memcmp(data1, data2, length);
	}

	int GetErrNo()
	{
		return err_no;
	}
	int GetLastAddr() const         //useful in I2C Bus transaction,
	{
		return last_addr;        // tell the address where error occurs
	}

	void SetBusInterface(BusInterface *ptr)
	{
		if (ptr)
		{
			busI = ptr;
		}
	}

	virtual void SetDelay();
	virtual void SetDelay(int delay);
	int GetDelay() const
	{
		return shot_delay;
	}

	long GetLastProgrammedAddress() const
	{
		return last_programmed_addr;
	}

	void ClearLastProgrammedAddress()
	{
		last_programmed_addr = 0;
	}

	void SetLastProgrammedAddress(long addr)
	{
		if (addr > last_programmed_addr)
		{
			last_programmed_addr = addr;
		}
	}

  protected:             //------------------------------- protected

	int     err_no;                 //error code
	int     last_addr;

	int shot_delay;         //delay unit to perform bus timing

	BusInterface *busI;

  private:               //------------------------------- private

	int CheckAbort(int progress = 0);

	int old_progress;
	long last_programmed_addr;      //record last programmed address for verify

};

#endif
