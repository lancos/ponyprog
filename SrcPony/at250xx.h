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

#ifndef _AT250XX_H
#define _AT250XX_H

#include "types.h"

#include "device.h"
#include "at250bus.h"


class At250xx : public Device
{
  public:               //---------------------------------------- public

	At250xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~At250xx();

	virtual int Probe(int probe_size = 0);
	virtual int Read(int probe = 1, int type = ALL_TYPE);
	virtual int Write(int probe = 1, int type = ALL_TYPE);
	virtual int Verify(int type = ALL_TYPE);

  protected:    //--------------------------------------- protected

	At250Bus *GetBus()
	{
		return static_cast<At250Bus *>(Device::GetBus());
	}

  private:              //--------------------------------------- private

};
#endif
