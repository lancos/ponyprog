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

#ifndef _I2CBUS_H
#define _I2CBUS_H

#include "busio.h"

class I2CBus : public BusIO
{
  public:                //------------------------------- public
	I2CBus(BusInterface *ptr = 0);
	virtual ~I2CBus();

	long Read(int slave, uint8_t *data, long length, int page_size = 0);
	long Write(int slave, uint8_t const *data, long length, int page_size = 0);

	int Start(uint8_t slave);
	int ReadByte(int ack, int lsb = 0);
	int WriteByte(int by, int lsb = 0);
	long StartRead(uint8_t slave, uint8_t *data, long length);
	long StartWrite(uint8_t slave, uint8_t const *data, long length);
	int Stop();
	int Reset();

	void Close();
	int TestPort(int port);
	//      int Calibration(int slave = 0xA0);

	void SetDelay();

  protected:             //------------------------------- protected

	int CheckBusy();
	int SendStart();
	int SendStop();
	int SendBitMast(int b);
	int RecBitMast();
	int SendByteMast(int by);
	int SendByteMastLSB(int by);
	int RecByteMast(int ack);
	int RecByteMastLSB(int ack);

  private:               //------------------------------- private


	void setSCLSDA()
	{
		busI->SetClockData();
	}

	int isSCLSDAuno() const
	{
		return busI->IsClockDataUP();
	}

	void setSDA()
	{
		busI->SetDataOut(1);
	}

	void clearSDA()
	{
		busI->SetDataOut(0);
	}

	void setSCL()
	{
		busI->SetClock(1);
	}

	void clearSCL()
	{
		busI->SetClock(0);
	}

	int getSCL() const
	{
		return busI->GetClock();
	}

	int getSDA() const
	{
		return busI->GetDataIn();
	}

	void bitSDA(int b)
	{
		busI->SetDataOut(b);
	}
};

#endif
