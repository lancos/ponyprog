//=========================================================================//
//-------------------------------------------------------------------------//
// at90sbus.h -- Header for for At90sBus class                             //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000  Claudio Lanconelli                            //
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

#ifndef	_AT90SBUS_H
#define	_AT90SBUS_H

#include "spi-bus.h"

class At90sBus : public SPIBus
{
 public:		//------------------------------- public
	At90sBus(BusInterface *ptr = 0);
//	virtual ~At90sBus();

	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);
	
	virtual int Reset();
	int Erase();

	int ReadDeviceCode(int addr);
	int WriteLockBits(int byte);
	int WriteFuseBits(int byte);
	int ReadLockBits();
	int ReadFuseBits();

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
	const BYTE WriteFuse0, WriteFuse1;
	
	int ReadEEPByte(long addr);
	void WriteEEPByte(long addr, int data);
	int ReadProgByte(long addr);
	void WriteProgByte(long addr, int data);

	void RefreshParameters();

	virtual int WaitReadyAfterWrite(int type, long addr, int data, long timeout = 5000);

	//Polling codes
	const BYTE p1_a, p2_a, pflash_a;
	const BYTE p1_b, p2_b, pflash_b;

	//Erase and programming delays
	int twd_erase;
	int twd_prog;

 private:		//------------------------------- private

};

#endif
