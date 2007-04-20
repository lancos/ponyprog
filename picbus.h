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
	
	int Erase(int type = ALL_TYPE);

	int Reset();

	long ReadConfig(UWORD *data);
	long WriteConfig(UWORD *data);

	void DisableCodeProtect();

	int CompareSingleWord(UWORD data1, UWORD data2, UWORD mask);
	int CompareMultiWord(UBYTE *data1, UBYTE *data2, ULONG length, int split);

	void SetDelay();

 protected:		//------------------------------- protected

	int SendDataWord(long wo, int wlen = 16);
	long RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 5000);

	int SendCmdCode(int opcode);
	int SendProgCode(UWORD data);
	UWORD RecvProgCode();
	int SendDataCode(UWORD data);
	UWORD RecvDataCode();

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
	const UBYTE EraseProgMem;
	const UBYTE EraseDataMem;
	const UBYTE BeginEraseProgCode;
	const UBYTE BeginProgOnlyCode;

 private:		//------------------------------- private

	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
		{ busI->SetInvDataOut(b); }

	void setDI()
		{ busI->SetInvDataOut(1); }

	void clearDI()
		{ busI->SetInvDataOut(0); }

	void setCLK()
		{ busI->SetClock(1); }

	void clearCLK()
		{ busI->SetClock(0); }

	int getDO() const
		{ return busI->GetDataIn(); }

};

#endif
