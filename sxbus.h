//=========================================================================//
//-------------------------------------------------------------------------//
// sxbus.h -- Header for SxBus class                                       //
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

#ifndef	_SXBUS_H
#define	_SXBUS_H

#include "busio.h"
#include "pgminter.h"

class SxBus : public BusIO
{
 public:		//------------------------------- public
	SxBus(BusInterface *ptr = 0);
	virtual ~SxBus();

	long Read(int addr, UBYTE *data, long length);
	long Write(int addr, UBYTE const *data, long length);
	
	int Reset();

//	int Open(int port);
//	void Close();

 protected:		//------------------------------- protected

	int SendDataWord(int wo, int wlen = 16);
	int RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 500000);

	int SendCmdOpcode(int opcode)
		{ return SendDataWord(opcode, 3); }
//	int SendAddress(int addr, int alen)
//		{ return SendDataWord(addr, alen); }

	void SetReset()
		{ busI->SetControlLine(1); }
	void ClearReset()
		{ busI->SetControlLine(0); }

 private:		//------------------------------- private

	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
		{ busI->SetDataOut(b); }

	void setDI()
		{ busI->SetDataOut(1); }

	void clearDI()
		{ busI->SetDataOut(0); }

	void setCLK()
		{ busI->SetClock(1); }

	void clearCLK()
		{ busI->SetClock(0); }

	int getDO() const
		{ return busI->GetDataIn(); }
};

#endif
