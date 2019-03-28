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

class MpsseCommandQueue
{
  public:
	MpsseCommandQueue()
	{
		clear();
	}

	void clear()
	{
		cmdidx = 0;
	}

	int getSize()
	{
		return cmdidx;
	}

	bool append(uint8_t dat)
	{
		if (cmdidx < sizeof(cmdbuf))
		{
			cmdbuf[cmdidx++] = dat;
			return true;
		}
		else
		{
			return false;
		}
	}

	uint8_t *getBuffer()
	{
		return cmdbuf;
	}

	bool isFull()
	{
		return (cmdidx >= sizeof(cmdbuf));
	}

	bool almostFull(int n = 6)
	{
		return (cmdidx >= sizeof(cmdbuf) - n);
	}

	bool isEmpty()
	{
		return (cmdidx == 0);
	}

  private:
	uint8_t cmdbuf[1024];
	unsigned int cmdidx;
};

class MpsseInterface : public BusInterface
{
  public:
	MpsseInterface();
	virtual ~MpsseInterface();

	virtual int Open(int port);
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
	virtual int Flush();

	const int usb_vid = 0x0403;
	const int usb_pid = 0xcff8;

	virtual unsigned long SPI_xferWord(int &err, unsigned long word_out, int mode = 0, int bpw = 8, bool lsb_first = false);

	virtual void SetDelay(int delay);
	virtual void ShotDelay(int n = 1);

  protected:
	//int GetPresence() const;
	uint8_t SPI_xferByte(int &err, uint8_t by, int mode, int bpw, bool lsb_first);

  private:
	int InitPins();
	void DeInitPins();

	int SetFrequency(uint32_t freq);
	int SendPins(int new_data, int new_directions = -1);
	int GetPins();

	//utility to set/reset/toggle a pin
	unsigned int OutDataMask(int old_val, int mask, int val)
	{
		if (val == 0)
			return old_val & ~mask;
		else if (val == 1)
			return old_val | mask;
		else
			return old_val ^ mask;
	}
	unsigned int OutDataMask(int mask, int val)
	{
		return OutDataMask(last_data, mask, val);
	}

	Ftdi::Context ctx;

	MpsseCommandQueue cmdbuf;

	int pin_directions;

	int last_data;
	int read_data;

	int pin_ctrl;
	int pin_datain;
	int pin_dataout;
	int pin_clock;
	int pin_clockin;	//0 if unused
};

#endif
