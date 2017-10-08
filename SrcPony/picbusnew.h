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

#ifndef _PICBUSNEW_H
#define _PICBUSNEW_H

#include "picbus.h"

class PicBusNew : public PicBus
{
  public:                //------------------------------- public
	PicBusNew(BusInterface *ptr = 0);

	//      long Read(int addr, uint8_t *data, long length);
	long Write(int addr, uint8_t const *data, long length, int page_size = 0);

	int Reset();

  protected:             //------------------------------- protected

	int WaitReadyAfterWrite(long timeout = 5000);

  private:               //------------------------------- private

};

#endif
