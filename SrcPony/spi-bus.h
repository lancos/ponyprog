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

#ifndef _SPIBUS_H
#define _SPIBUS_H

#include "busio.h"
#include "pgminter.h"

class SPIBus : public BusIO
{
  public:                //------------------------------- public
	SPIBus(BusInterface *ptr = 0, bool cpha = false);
	virtual ~SPIBus();

	virtual int Reset();

	void SetDelay();
	void SetFallingPhase(bool cpha)
	{
		fall_edge_sample = cpha;
	}

  protected:             //------------------------------- protected

	int SendDataByte(int by);
	int RecDataByte();

	void SetReset()
	{
		busI->SetControlLine(1);
	}
	void ClearReset()
	{
		busI->SetControlLine(0);
	}

	void setSCK()
	{
		busI->SetClock(1);
	}

	void clearSCK()
	{
		busI->SetClock(0);
	}

	int SendDataBit(int b);
	int RecDataBit();

  private:
	void bitMOSI(int b)
	{
		busI->SetDataOut(b);
	}

	void setMOSI()
	{
		busI->SetDataOut(1);
	}

	void clearMOSI()
	{
		busI->SetDataOut(0);
	}

	int getMISO() const
	{
		return busI->GetDataIn();
	}

	bool fall_edge_sample;
};

#endif
