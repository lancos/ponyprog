//=========================================================================//
//-------------------------------------------------------------------------//
// pic16xx.h -- Header for for Pic16xx class                       //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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
// along with this program (see COPYING);     if not, write to the         //
// Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. //
//                                                                         //
//-------------------------------------------------------------------------//
//=========================================================================//

#ifndef _PIC16XX_H
#define _PIC16XX_H

#include "types.h"

#include "eeprom.h"
#include "picbus.h"

class Pic16xx : public EEProm
{
  public:		//---------------------------------------- public

	Pic16xx(e2AppWinInfo *wininfo = 0, BusIO *busp = 0);
	virtual ~Pic16xx();

	int Probe(int probe_size = 0);
	int Read(int probe = 1);
	int Write(int probe = 1);
	int Verify();

	//Write lock bits
	int SecurityRead(int &msb, int &lsb);
	int SecurityWrite(int msb, int lsb);

  protected:	//--------------------------------------- protected

	PicBus *GetBus()
		{ return (PicBus *)EEProm::GetBus(); }

	WORD id_locations[8];

  private:		//--------------------------------------- private

};
#endif
