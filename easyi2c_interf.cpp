//=========================================================================//
//-------------------------------------------------------------------------//
// easyi2c_interf.cpp -- EasyI2CBus interface                              //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2001   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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
// $Id$
//=========================================================================//

#include "errcode.h"
#include "easyi2c_interf.h"
#include "e2app.h"

/* data register (0x378)
 * bit 0: D0	(pin 2)
 * bit 1: D1	(pin 3)
 * bit 2: D2	(pin 4)
 * bit 3: D3	(pin 5)
 * bit 4: D4	(pin 6)
 * bit 5: D5	(pin 7)
 * bit 6: D6	(pin 8)
 * bit 7: D7	(pin 9)
 */

/* control register (0x37A)
 * bit 0: STROBE	(pin 1)-
 * bit 1: AUTOLF	(pin 14)-
 * bit 2: INIT		(pin 16)-
 * bit 3: SELECTIN	(pin 17)-
 */

/* status register (0x379)
 * bit 3: ERROR		(pin 15)
 * bit 4: SELECT	(pin 13)
 * bit 5: POUT		(pin 12)
 * bit 6: ACK		(pin 10)
 * bit 7: BUSY		(pin 11)-
 */

#define WB_SCL	0		// DATA (pin 2)
#define WB_SDA	1		// DATA (pin 3)

#define	RB_SCL	4		// STATUS (pin 13)
#define RB_SDA	5		// STATUS (pin 12)

#define WF_SCL	(1 << WB_SCL)
#define WF_SDA	(1 << WB_SDA)

#define RF_SCL	(1 << RB_SCL)
#define RF_SDA	(1 << RB_SDA)

EasyI2CInterface::EasyI2CInterface(bool use_io)
	: LptExtInterface(use_io)
{
	Install(0);
	old_portno = 0;
}

int EasyI2CInterface::Open(int port_no)
{
	int ret_val = OK;

	if (IsInstalled() != port_no)
	{
		if ( InDataPort(port_no) < 0 )
			ret_val = E2ERR_OPENFAILED;
		else
		{
			Install(port_no);

			OutDataPort(0);
		}
	}

	return ret_val;
}

void EasyI2CInterface::Close()
{
	if (IsInstalled())
	{
		LptExtInterface::Close();

		Install(0);
	}
}


void EasyI2CInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "EasyI2CInterface::SetDataOut(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity()&DOUTINV)
			sda = !sda;

		//The EasyI2C interface is inverting by default
		OutDataMask(WF_SDA, sda ? 0 : 1);
	}
}

void EasyI2CInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "EasyI2CInterface::SetClock(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity()&CLOCKINV)
			scl = !scl;

		//The EasyI2C interface is inverting by default
		OutDataMask(WF_SCL, scl ? 0 : 1);
	}
}

void EasyI2CInterface::SetClockData()
{
	UserDebug1(UserApp3, "EasyI2CInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control	= THEAPP->GetPolarity();
		UBYTE cpreg = GetLastData();

		if (control & CLOCKINV)
			cpreg |= WF_SCL;
		else
			cpreg &= ~WF_SCL;

		if (control & DOUTINV)
			cpreg |= WF_SDA;
		else
			cpreg &= ~WF_SDA;

		OutDataPort(cpreg);
	}
}

int EasyI2CInterface::GetDataIn() 
{
	UserDebug1(UserApp3, "EasyI2CInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & DININV) 
			return !(InDataPort() & RF_SDA);
		else
			return InDataPort() & RF_SDA;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::GetClock() 
{
	UserDebug1(UserApp3, "EasyI2CInterface::GetClock() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		return InDataPort() & RF_SCL;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::IsClockDataUP() 
{
	if (IsInstalled())
	{
		int val = InDataPort();
		int sda = val & RF_SDA;
		int scl = val & RF_SCL;

		if (THEAPP->GetPolarity() & DININV)
			sda = !sda;

		return (sda && scl);
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::IsClockDataDOWN()
{
	if (IsInstalled())
	{
		int val = InDataPort();
		int sda = val & RF_SDA;
		int scl = val & RF_SCL;

		if (THEAPP->GetPolarity() & DININV)
			sda = !sda;

		return (!sda && !scl);
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::TestPort(int port)
{
	UserDebug1(UserApp1, "EasyI2CInterface::TestPort(%d) IN\n", port);

	int ret_val = TestSave(port);
	Wait w;

	if (ret_val == OK)
	{
		SetDataOut(0);
		w.WaitMsec(50);
		if (GetDataIn())
			ret_val = E2ERR_OPENFAILED;
		else
		{
			SetDataOut(1);
			w.WaitMsec(50);
			if (!GetDataIn())
				ret_val = E2ERR_OPENFAILED;
		}
	}
	TestRestore();

	UserDebug1(UserApp2, "EasyI2CInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}
