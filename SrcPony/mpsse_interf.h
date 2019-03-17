//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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

#ifndef _MPSSE_INTERFACE_H
#define _MPSSE_INTERFACE_H

#include "businter.h"
#include "ftdi.hpp"

class MpsseInterface : public BusInterface
{
  public:
	MpsseInterface();
	virtual ~MpsseInterface();

	virtual int Open(int com_no);
	virtual void Close();

	virtual void SetDataOut(int sda = 1);
	virtual void SetClock(int scl = 1);
	virtual int GetDataIn();
	virtual int GetClock();
	virtual void SetClockData();
	virtual void ClearClockData();
	virtual int IsClockDataUP();
	virtual int IsClockDataDOWN();

	//      virtual int TestPort(int port);

	int SetPower(bool onoff);
	void SetControlLine(int res = 1);

	void List();

	const int usb_vid = 0x0403;
	const int usb_pid = 0xcff8;

  protected:
	//      int GetPresence() const;

  private:
	int InitPins();
	void DeInitPins();

	int SetFrequency(uint32_t freq);
	int SendPins();
	int GetPins();
	void OutDataMask(int mask, int val);

	Ftdi::Context ctx;

	unsigned int last_data;
	unsigned int read_data;

	unsigned int pin_ctrl;
	unsigned int pin_datain;
	unsigned int pin_dataout;
	unsigned int pin_clock;
};

#endif
