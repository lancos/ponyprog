//=========================================================================//
//-------------------------------------------------------------------------//
// picbus.h -- Header for MicroWireBus class                             //
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

#ifndef	_PICBUS_H
#define	_PICBUS_H

#include "busio.h"
#include "pgminter.h"

class PicBus : public BusIO
{
 public:		//------------------------------- public
	PicBus(BusInterface *ptr = 0);
	virtual ~PicBus();

	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);
	
	int Erase();

	int Reset();

	long ReadConfig(UWORD *data);
	long WriteConfig(UWORD *data);

	void DisableCodeProtect();

	int CompareSingleWord(UWORD data1, UWORD data2, UWORD mask);
	int CompareMultiWord(UBYTE *data1, UBYTE *data2, ULONG length, int prog = 1);

	void SetDelay();

 protected:		//------------------------------- protected

	int SendDataWord(long wo, int wlen = 16);
	long RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 5000);

	int SendCmdCode(int opcode)
		{ return SendDataWord(opcode, 6); }
	int SendProgCode(UWORD data)
	{
		//the code is 14 bit data with leading and trailing 0's
		data &= ProgMask;
		data <<= 1;
		return SendDataWord(data);
	}
	UWORD RecvProgCode()
	{
		return (UWORD)(RecDataWord() >> 1) & ProgMask;
	}
	int SendDataCode(UWORD data)
	{
		//the code is 8 bit data with leading and trailing 0's
		data &= DataMask;
		data <<= 1;
		return SendDataWord(data);
	}
	UWORD RecvDataCode()
	{
		return (UWORD)(RecDataWord() >> 1) & DataMask;
	}

	void SetMCLR()
		{ busI->SetControlLine(1); }
	void ClearMCLR()
		{ busI->SetControlLine(0); }

	const UWORD DataMask;
	const UWORD ProgMask;

	//Command Opcode
	const UBYTE ReadProgCode;
	const UBYTE LoadProgCode;
	const UBYTE ReadDataCode;
	const UBYTE LoadDataCode;
	const UBYTE LoadConfigCode;
	const UBYTE IncAddressCode;
	const UBYTE BeginEraseProgCode;
	const UBYTE BeginProgOnlyCode;
	const UBYTE EraseProgMem;
	const UBYTE EraseDataMem;

 private:		//------------------------------- private

	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
		{ busI->SetDataOut(!b); }

	void setDI()
		{ busI->SetDataOut(0); }

	void clearDI()
		{ busI->SetDataOut(1); }

	void setCLK()
		{ busI->SetClock(1); }

	void clearCLK()
		{ busI->SetClock(0); }

	int getDO() const
		{ return busI->GetDataIn(); }

};

#endif
