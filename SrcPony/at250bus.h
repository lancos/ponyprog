//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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
  public:
	At250Bus(BusInterface *ptr = 0);
	//virtual ~At250Bus();

	long Read(int addr, quint8 *data, long length, int page_size = 0);
	long Write(int addr, quint8 const *data, long length, int page_size = 0);

	int Reset();

  protected:
	virtual int ReadEEPByte(int addr);
	virtual void WriteEEPByte(int addr, int data);
	int ReadEEPStatus();
	int WriteEEPStatus(int data);
	void EndCycle(void);

	int WaitEndOfWrite(int timeout = 0);		// 07/08/99

	//Programming commands
	const quint8 WriteEnable;
	const quint8 WriteDisable;
	const quint8 ReadStatus;
	const quint8 WriteStatus;
	const quint8 ReadData;
	const quint8 WriteData;

	//Status flags
	const quint8 NotReadyFlag;
	const quint8 WenFlag;
	const quint8 BPFlags;

	const int loop_timeout;

  private:
	void setNCS()
	{
		ClearReset();		//27/05/98
	}

	void clearNCS()
	{
		SetReset();			//27/05/98
	}
};

#endif
