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

#ifndef _PIC125XX_H
#define _PIC125XX_H

#include "types.h"

#include "pic12bus.h"
#include "device.h"


class Pic125xx : public Device
{
  public:               //---------------------------------------- public

	Pic125xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~Pic125xx();

	int Probe(int probe_size = 0);
	int Read(int probe = 1, int type = ALL_TYPE);
	int Write(int probe = 1, int type = ALL_TYPE);
	int Verify(int type = ALL_TYPE);

	//Write lock bits
	int SecurityRead(uint32_t &bits);
	int SecurityWrite(uint32_t bits);

  protected:    //--------------------------------------- protected

	Pic12Bus *GetBus()
	{
		return static_cast<Pic12Bus *>(Device::GetBus());
	}

	virtual int CodeProtectAdjust(uint16_t &config, int read = 0);
	//      virtual int WriteProg();
	virtual int VerifyProg(unsigned char *localbuf);

  private:              //--------------------------------------- private

	uint16_t config_word;
};
#endif
