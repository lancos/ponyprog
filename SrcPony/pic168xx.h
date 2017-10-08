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

#ifndef _PIC168XX_H
#define _PIC168XX_H

#include "pic16xx.h"
#include "picbusnew.h"


class Pic168xx : public Pic16xx
{
  public:               //---------------------------------------- public

	Pic168xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~Pic168xx();

	int Probe(int probe_size = 0);
	//      int Read(int probe = 1, int type = ALL_TYPE);
	int Write(int probe = 1, int type = ALL_TYPE);
	//      int Verify(int type = ALL_TYPE);

  protected:    //--------------------------------------- protected

	PicBus *GetBus()
	{
		return static_cast<PicBusNew *>(Device::GetBus());
	}

	int QueryType(long &type);
	int CodeProtectAdjust(uint16_t &config, int read = 0);

  private:              //--------------------------------------- private

};
#endif
