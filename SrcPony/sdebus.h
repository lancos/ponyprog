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

#ifndef _SDE2506BUS_H
#define _SDE2506BUS_H

#include "busio.h"
#include "pgminter.h"

class Sde2506Bus : public BusIO
{
  public:                //------------------------------- public
	Sde2506Bus(BusInterface *ptr = 0);
	virtual ~Sde2506Bus();

	long Read(int addr, uint8_t *data, long length, int page_size = 0);
	long Write(int addr, uint8_t const *data, long length, int page_size = 0);

	int Reset();

	void SetDelay();

  protected:             //------------------------------- protected

	int SendDataWord(int wo, int wlen = 8);
	int RecDataWord(int wlen = 8);
	int WaitReadyAfterWrite(long timeout = 5000);

	int SendControlBit(int opcode)
	{
		return SendDataWord(opcode, 1);
		WaitUsec(5);
	}
	int SendAddress(int addr)
	{
		return SendDataWord(addr, 7);
	}

	void setCE()
	{
		busI->SetControlLine(0);
	}
	void clearCE()
	{
		busI->SetControlLine(1);
	}

  private:               //------------------------------- private

	int SendDataBit(int b);
	int RecDataBit();

	void bitDI(int b)
	{
		busI->SetDataOut(b);
	}

	void setDI()
	{
		busI->SetDataOut(1);
	}

	void clearDI()
	{
		busI->SetDataOut(0);
	}

	void setCLK()
	{
		busI->SetClock(1);
	}

	void clearCLK()
	{
		busI->SetClock(0);
	}

	int getDO() const
	{
		return busI->GetDataIn();
	}
};

#endif
