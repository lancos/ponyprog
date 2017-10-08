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

#ifndef _mE2401_H
#define _mE2401_H

#include "types.h"

#include "device.h"
#include "i2cbus.h"


class mE2401 : public Device
{
  public:               //---------------------------------------- public

	mE2401(e2AppWinInfo *wininfo = 0, BusIO *busp = 0, int def_banksize = 128);
	//      virtual ~mE2401();

	int Probe(int probe_size = 0);
	int Read(int probe = 1, int type = ALL_TYPE);
	int Write(int probe = 1, int type = ALL_TYPE);
	int Verify(int type = ALL_TYPE);

  protected:    //--------------------------------------- protected

	int const timeout_loop;         //eeprom timeout

	I2CBus *GetBus()
	{
		return static_cast<I2CBus *>(Device::GetBus());
	}

	//-- Parte riguardante la EEPROM
	int sequential_read;                    //1 --> legge un banco in una volta
	int writepage_size;                             //se > 1 scrive una pagina alla volta

  private:              //--------------------------------------- private

};
#endif
