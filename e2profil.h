//=========================================================================//
//-------------------------------------------------------------------------//
// e2profil.h -- Header of E2Profile class                                 //
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

#ifndef _E2PROFILE_H
#define _E2PROFILE_H

#include "types.h"
#include "globals.h"
#include "profile.h"

#define CLOCKINV (1<<0)
#define RESETINV (1<<1)
#define DININV   (1<<2)
#define DOUTINV  (1<<3)

enum {
	SLOW,
	NORMAL,
	FAST,
	TURBO
};

#define	STRBUFSIZE	256

class E2Profile : public Profile
{
  public:		//---------------------------------------- public

	E2Profile();

//	int GetParDelayLoop();
//	int SetParDelayLoop(int value);
	HInterfaceType GetParInterfType();
	int SetParInterfType(HInterfaceType type);
	int GetParPortNo();
	int SetParPortNo(int value);

	int GetBogoMips();
	int SetBogoMips(int value);

	long GetLastDevType();
	int SetLastDevType(long devtype);

	char const *GetLastFile();
	char const *GetPrevFile();
	int SetLastFile(char const *name);

	UBYTE GetPolarityControl(); 
	int SetPolarityControl(UBYTE polarity_control); 

	int GetSPIPageWrite();
	int SetSPIPageWrite(int page_write = 1);

	int GetI2CPageWrite();
	int SetI2CPageWrite(int page_write = 1);

	int GetI2CBaseAddr();
	int SetI2CBaseAddr(int base_addr = 0xA0);

	int GetI2CSpeed();
	int SetI2CSpeed(int speed = NORMAL);

	int GetSPIResetDelay();
	int SetSPIResetDelay(int delay = 100);

	int GetPowerUpDelay();
	int SetPowerUpDelay(int delay = 200);

	int GetSPISpeed();
	int SetSPISpeed(int speed = NORMAL);

	int GetMegaPageDelay();
	int SetMegaPageDelay(int delay = 25);

	int GetMicroWireSpeed();
	int SetMicroWireSpeed(int speed = NORMAL);

	int GetPICSpeed();
	int SetPICSpeed(int speed = NORMAL);

	int GetSDESpeed();
	int SetSDESpeed(int speed = NORMAL);

	int GetAVRProgDelay();
	int SetAVRProgDelay(int delay = 10);

	int GetAVREraseDelay();
	int SetAVREraseDelay(int delay = 30);

  protected:	//--------------------------------------- protected

  private:		//--------------------------------------- private

};
#endif
