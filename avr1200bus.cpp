//=========================================================================//
//-------------------------------------------------------------------------//
// avr1200bus.cpp -- AVR AT90S1200 Bus Driver                              //
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

#include "types.h"
#include "avr1200bus.h"

int Avr1200Bus::Reset()
{
	UserDebug(UserApp2, "Avr1200Bus::Reset() I\n");

	At90sBus::RefreshParameters();

	int k;
	for (k = 0; k < 4; k++)
	{
		SPIBus::Reset();

		WaitMsec(50);	// Almeno 20msec dai datasheet AVR atmel

		UserDebug(UserApp2, "Avr1200Bus::Reset() ** SendDataByte\n");

		SendDataByte(EnableProg0);
		SendDataByte(EnableProg1);
		RecDataByte();
		SendDataByte(0);

		if ( ReadDeviceCode(0) == 0x1E )
			break;
	}

	return OK;
}

int Avr1200Bus::WaitReadyAfterWrite(int type, long addr, int data, long timeout)
{
	WaitMsec(twd_prog);

	return OK;

//	return At90sBus::WaitReadyAfterWrite(type, addr, data, timeout);
}
