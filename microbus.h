//=========================================================================//
//-------------------------------------------------------------------------//
// microbus.h -- Header for MicroWireBus class                             //
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

#ifndef	_MICROBUS_H
#define	_MICROBUS_H

#include "busio.h"
#include "pgminter.h"

#define	ORG8	1
#define	ORG16	0

class MicroWireBus : public BusIO
{
 public:		//------------------------------- public
	MicroWireBus(BusInterface *ptr = 0);
	virtual ~MicroWireBus();

//	ULONG Read(int addr, UBYTE *data, ULONG length);
//	ULONG Write(int addr, UBYTE const *data, ULONG length);
	
	int Reset();

	int CalcAddressSize(int mem_size, int org = ORG16) const;

	void SetDelay();

 protected:		//------------------------------- protected

	int SendDataWord(int wo, int wlen = 16);
	int RecDataWord(int wlen = 16);
	int WaitReadyAfterWrite(long timeout = 50000);

	int SendCmdOpcode(int opcode)
		{ return SendDataWord(opcode, 3); }
	int SendAddress(int addr, int alen)
		{ return SendDataWord(addr, alen); }

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
