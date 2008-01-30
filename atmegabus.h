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

#ifndef	_ATMEGABUS_H
#define	_ATMEGABUS_H

#include "at90sbus.h"

class AtMegaBus : public At90sBus
{
 public:		//------------------------------- public
	AtMegaBus(BusInterface *ptr = 0, int wpage_size = 256, bool page_poll = false);		//Default to ATmega103

	long Write(int addr, UBYTE const *data, long length, int page_size = 0);

	void SetPageSize(int size);
	int GetPageSize() const;
	void SetFlashPagePolling(bool val);
	bool GetFlashPagePolling() const;

 protected:		//------------------------------- protected

	void WriteProgPage(long addr, UBYTE const *data, long page_size, long timeout = 20000);
	bool CheckBlankPage(UBYTE const *data, ULONG length);

 private:		//------------------------------- private

	bool enable_flashpage_polling;
	int write_page_size;
};

#endif
