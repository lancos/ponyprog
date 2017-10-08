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

#ifndef _AT250BUS_H
#define _AT250BUS_H

#include "spi-bus.h"

class At250Bus : public SPIBus
{
  public:                //------------------------------- public
	At250Bus(BusInterface *ptr = 0);
	//      virtual ~At250Bus();

	long Read(int addr, uint8_t *data, long length, int page_size = 0);
	long Write(int addr, uint8_t const *data, long length, int page_size = 0);

	int Reset();

  protected:             //------------------------------- protected

	virtual int ReadEEPByte(int addr);
	virtual void WriteEEPByte(int addr, int data);
	int ReadEEPStatus();
	int WriteEEPStatus(int data);
	void EndCycle(void);

	int WaitEndOfWrite(int timeout = 0);            // 07/08/99

	//Programming commands
	const uint8_t WriteEnable;
	const uint8_t WriteDisable;
	const uint8_t ReadStatus;
	const uint8_t WriteStatus;
	const uint8_t ReadData;
	const uint8_t WriteData;

	//Status flags
	const uint8_t NotReadyFlag;
	const uint8_t WenFlag;
	const uint8_t BPFlags;

	const int loop_timeout;

  private:               //------------------------------- private

	void setNCS()
	{
		ClearReset();        //27/05/98
	}

	void clearNCS()
	{
		SetReset();        //27/05/98
	}
};

#endif
