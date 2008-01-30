//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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

#ifndef	_AT250BUS_H
#define	_AT250BUS_H

#include "spi-bus.h"

class At250Bus : public SPIBus
{
 public:		//------------------------------- public
	At250Bus(BusInterface *ptr = 0);
//	virtual ~At250Bus();

	long Read(int addr, UBYTE *data, long length, int page_size = 0);
	long Write(int addr, UBYTE const *data, long length, int page_size = 0);
	
	int Reset();

 protected:		//------------------------------- protected

	virtual int ReadEEPByte(int addr);
	virtual void WriteEEPByte(int addr, int data);
	int ReadEEPStatus();
	int WriteEEPStatus(int data);
	void EndCycle(void);

	int WaitEndOfWrite(int timeout = 0);		// 07/08/99

	//Programming commands
	const BYTE WriteEnable;
	const BYTE WriteDisable;
	const BYTE ReadStatus;
	const BYTE WriteStatus;
	const BYTE ReadData;
	const BYTE WriteData;

	//Status flags
	const BYTE NotReadyFlag;
	const BYTE WenFlag;
	const BYTE BPFlags;

	const int loop_timeout;

 private:		//------------------------------- private

	void setNCS()
		{ ClearReset(); }	//27/05/98

	void clearNCS()
		{ SetReset(); }		//27/05/98
};

#endif
