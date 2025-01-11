//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2025   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                        //
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

#ifndef _MICROBUS_H
#define _MICROBUS_H

#include "busio.h"
#include "pgminter.h"

#define ORG8    8
#define ORG16   16

class MicroWireBus : public BusIO
{
  public:
	MicroWireBus(BusInterface *ptr = nullptr);
	virtual ~MicroWireBus();

	//long Read(int addr, uint8_t *data, long length);
	//long Write(int addr, uint8_t const *data, long length);

	int Reset();

	virtual int CalcAddressSize(int mem_size) const;

	void SetDelay();

  protected:

	int SendDataWord(int wo, int wlen, bool lsb = false);
	int RecDataWord(int wlen, bool lsb = false);
	int RecDataWordShort(int wlen, bool lsb = false);
	int WaitReadyAfterWrite(long timeout = 50000);

	int SendAddress(int addr, int alen)
	{
		return SendDataWord(addr, alen);
	}

	void SetReset()
	{
		busI->SetControlLine(1);
	}
	void ClearReset()
	{
		busI->SetControlLine(0);
	}

  private:

	int SendDataBit(int b);
	int RecDataBit();
	int RecDataBitShort();

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
