//=========================================================================//
//-------------------------------------------------------------------------//
// easyi2c_com.cpp -- EasyI2CBus interface                                 //
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

#include "easyi2c_com.h"
#include "errcode.h"
#include "e2app.h"

#define WB_SCL	0		// COM: dtr (pin 4)
#define WB_SDA	1		// COM: rts (pin 7)
#define	RB_SCL	4		// COM: cts (pin 8)
#define RB_SDA	5		// COM: dsr (pin 6)

#define	WB_OFF	2
#define	RB_DATA	6
#define	RB_SQL	7		// 1 --> chiuso, 0 --> aperto

#define WF_SCL	(1 << WB_SCL)
#define WF_SDA	(1 << WB_SDA)
#define RF_SCL	(1 << RB_SCL)
#define RF_SDA	(1 << RB_SDA)

EasyI2CInterface::EasyI2CInterface()
{
	Install(0);
	old_portno = 0;
}

int EasyI2CInterface::Open(int com_no)
{
	int ret_val = OK;

	if (IsInstalled() != com_no)
	{
		if ( (ret_val = OpenSerial(com_no)) == OK )
		{
			Install(com_no);
		}
	}

	return ret_val;
}

void EasyI2CInterface::Close()
{
	if (IsInstalled())
	{
		CloseSerial();
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
		if (sda)
			OutPortMask(WF_SDA, 0);
		else
			OutPortMask(WF_SDA, 1);
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
		if (scl)
			OutPortMask(WF_SCL, 0);
		else
			OutPortMask(WF_SCL, 1);
	}
}

void EasyI2CInterface::SetClockData()
{
	UserDebug1(UserApp3, "EasyI2CInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control	= THEAPP->GetPolarity();
		UBYTE cpreg = GetCPWReg();

		if (control & CLOCKINV)
			cpreg |= WF_SCL;
		else
			cpreg &= ~WF_SCL;

		if (control & DOUTINV)
			cpreg |= WF_SDA;
		else
			cpreg &= ~WF_SDA;

		OutPort(cpreg);
	}
}

int EasyI2CInterface::GetDataIn() 
{
	UserDebug1(UserApp3, "EasyI2CInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & DININV) 
			return ~InPort() & RF_SDA;
		else
			return InPort() & RF_SDA;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::GetClock() 
{
	UserDebug1(UserApp3, "EasyI2CInterface::GetClock() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		return InPort() & RF_SCL;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int EasyI2CInterface::IsClockDataUP() 
{
	if (IsInstalled())
	{
		int val = InPort();
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
		int val = InPort();
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
