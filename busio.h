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

#ifndef	_BUSIO_H
#define	_BUSIO_H

#include "types.h"
#include "wait.h"
#include "businter.h"

//Formato di bus generico seriale
class BusIO : public Wait
{
 public:		//------------------------------- public
	BusIO(BusInterface *p = 0);
//	virtual ~BusIO() { }

	virtual int Open(int port)
		{ return (err_no = busI->Open(port)); }
	virtual void Close()
		{ busI->Close(); }
	virtual int Error();

	virtual int TestPort(int port)
		{ return OK; }

	virtual int Reset() = 0;

	virtual long Read(int addr, uint8_t *data, long length, int page_size = 0) = 0;
	virtual long Write(int addr,uint8_t const *data, long length, int page_size = 0) = 0;
	virtual int Erase(int type = 0)
		{ return NOTSUPPORTED; }

	virtual int ReadDeviceCode(int addr)
		{ return OK; }
	virtual int WriteLockBits(DWORD val, long model = 0)
		{ return OK; }
	virtual int WriteFuseBits(DWORD val, long model = 0)
		{ return OK; }
	virtual DWORD ReadLockBits(long model = 0)
		{ return 0; }
	virtual DWORD ReadFuseBits(long model = 0)
		{ return 0; }

	virtual long ReadCalibration(int addr = 0)
		{ return -1; }			//No calibration value available

	virtual int CompareMultiWord(uint8_t *data1, uint8_t *data2, ULONG length, int split)
		{ return memcmp(data1, data2, length); }

	int GetErrNo()
		{ return err_no; }
	int GetLastAddr() const		//useful in I2C Bus transaction,
		{ return last_addr; }	// tell the address where error occurs

	void SetBusInterface(BusInterface *ptr)
		{ if (ptr) busI = ptr; }

	int CheckAbort(int progress = 0);

	virtual void SetDelay();
	virtual void SetDelay(int delay);
	int GetDelay() const
		{ return shot_delay; }

	long GetLastProgrammedAddress() const
		{ return last_programmed_addr; }

	void ClearLastProgrammedAddress()
		{ last_programmed_addr = 0; }

	void SetLastProgrammedAddress(long addr)
	{
		if (addr > last_programmed_addr)
			last_programmed_addr = addr;
	}

 protected:		//------------------------------- protected

	int	err_no;			//error code
	int	last_addr;

	int shot_delay;		//delay unit to perform bus timing

	BusInterface *busI;

 private:		//------------------------------- private

	int old_progress;
	long last_programmed_addr;	//record last programmed address for verify

};

#endif
