//=========================================================================//
//-------------------------------------------------------------------------//
// i2cbus.h -- Header for I2CBus class                                     //
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

#ifndef	_I2CBUS_H
#define	_I2CBUS_H

#include "busio.h"

class I2CBus : public BusIO
{
 public:		//------------------------------- public
	I2CBus(BusInterface *ptr = 0);
	virtual ~I2CBus();

	long Read(int slave, UBYTE *data, long length);
	long Write(int slave, UBYTE const *data, long length);
	
	int Start(UBYTE slave);
	int ReadByte(int ack);
	ULONG StartRead(UBYTE slave, UBYTE *data, ULONG length);
	ULONG StartWrite(UBYTE slave, UBYTE const *data, ULONG length);
	int Stop();
	int Reset();

	void Close();
	int TestPort(int port);
//	int Calibration(int slave = 0xA0);

	void SetDelay();

 protected:		//------------------------------- protected

 private:		//------------------------------- private

	int CheckBusy();
	int SendStart();
	int SendStop();
	int SendBitMast(int b);
	int RecBitMast();
	int SendByteMast(int by);
	int RecByteMast(int ack);

	void setSCLSDA()
		{ busI->SetClockData(); }

	int isSCLSDAuno() const
		{ return busI->IsClockDataUP(); }

	void setSDA()
		{ busI->SetDataOut(1); }

	void clearSDA()
		{ busI->SetDataOut(0); }

	void setSCL()
		{ busI->SetClock(1); }

	void clearSCL()
		{ busI->SetClock(0); }

	int getSCL() const
		{ return busI->GetClock(); }

	int getSDA() const
		{ return busI->GetDataIn(); }

	void bitSDA(int b)
		{ busI->SetDataOut(b); }
};

#endif
