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

#ifndef _PIC16XX_H
#define _PIC16XX_H

#include "types.h"

#include "device.h"
#include "picbus.h"


class Pic16xx : public Device
{
  public:               //---------------------------------------- public

	Pic16xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~Pic16xx();

	int Probe(int probe_size = 0);
	int Read(int probe = 1, int type = ALL_TYPE);
	int Write(int probe = 1, int type = ALL_TYPE);
	int Verify(int type = ALL_TYPE);

	int Erase(int probe = 1, int type = ALL_TYPE);

	//Write lock bits
	int SecurityRead(uint32_t &bits);
	int SecurityWrite(uint32_t bits);

  protected:    //--------------------------------------- protected

	PicBus *GetBus()
	{
		return static_cast<PicBus *>(Device::GetBus());
	}

	uint16_t id_locations[8];
	/**
	        int ReadProg();
	        int ReadData();
	        int WriteProg();
	        int WriteData();
	        int VerifyProg(unsigned char *localbuf);
	        int VerifyData(unsigned char *localbuf);
	**/
	virtual int CodeProtectAdjust(uint16_t &config, int read = 0);

  private:              //--------------------------------------- private

};
#endif
