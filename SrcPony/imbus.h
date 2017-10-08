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

#ifndef _IMBUS_H
#define _IMBUS_H

#include "busio.h"

class IMBus : public BusIO
{
  public:                //------------------------------- public
	IMBus(BusInterface *ptr = 0);
	virtual ~IMBus();

	long Read(int addr, uint8_t *data, long length, int page_size = 0);
	long Write(int addr, uint8_t const *data, long length, int page_size = 0);

	int Reset();

	void SetDelay();

	void SetStatusLocation(int val);
	void UseSecondaryAddress(bool val);
	void SetDataMode(int val);
	void SetProgDelay(int val);

  protected:             //------------------------------- protected

	int SendAddrWord(int wo);
	int SendDataWord(long wo, int wlen = 8);
	int RecDataWord(int wlen = 8);
	int WaitReadyAfterWrite(int addr, int delay, long timeout = 5000);

	int IdentPulse();

	void setIdent()
	{
		busI->SetControlLine(1);
	}
	void clearIdent()
	{
		busI->SetControlLine(0);
	}

  private:               //------------------------------- private

	int StatusLocation;
	bool SecondaryAddress;
	bool Data16_mode;
	int ProgDelay;

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
