//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2007   Claudio Lanconelli                           //
//                                                                         //
//  http://ponyprog.sourceforge.net                                        //
//                                                                         //
//-------------------------------------------------------------------------//
// $Id$
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
// PonyIO (SI-Prog) card interface

#include "ponyioint.h"
#include "errcode.h"
#include "e2app.h"

/* modem control register bits
 * bit 0: DTR  (pin 4)
 * bit 1: RTS  (pin 7)
 */

 /* modem status register bits
 * bit 4: CTS  (pin 8 DB9)
 * bit 5: DSR  (pin 6)
 * bit 6: RI   (pin 9)
 * bit 7: DCD  (pin 1)
 */

# define WB_SCL	1		/* COM: rts (pin 7) */
# define WB_SDA	0		/* COM: dtr (pin 4) */
# undef	RB_SCL			/* Not used */
# define RB_SDA	4		/* COM: cts (pin 8) */
# define RB_TEST 5		/* COM: dsr (pin 6) */

# define WF_SCL	(1 << WB_SCL)
# define WF_SDA	(1 << WB_SDA)
# define RF_SCL	(1 << RB_SCL)
# define RF_SDA	(1 << RB_SDA)
# define RF_TEST (1 << RB_TEST)

#ifdef	_LINUX_
# include <sys/io.h>
# include <unistd.h>
#else
# ifdef	__BORLANDC__
#   define	__inline__
# else // MICROSOFT VC++
#   define	__inline__ __inline
#   define _export
# endif
#endif

enum UartRegs {
	thrOfst,
	ierOfst,
	iirOfst,
	lcrOfst,
	mcrOfst,
	lsrOfst,
	msrOfst
};


PonyIOInterface::PonyIOInterface()
{
	UserDebug(Constructor, "PonyIOInterface::PonyIOInterface() Constructor\n");

	Install(0);
	old_portno = 0;
}

PonyIOInterface::~PonyIOInterface()
{
	Close();
}

#define BREAK_MASK	0x40

//se  res == 1  abilita il BREAK --> Tx = +12v
// altrimenti disabilita il BREAK --> Tx = -12v
void PonyIOInterface::SetControlLine(int res)
{
	UserDebug2(UserApp3, "PonyIOInterface::SetControlLine(%d) *** Inst=%d\n", res, IsInstalled());

	if (IsInstalled())
	{
		UserDebug(UserApp3, "PonyIOInterface::SetControlLine() XX\n");

		if (THEAPP->GetPolarity() & RESETINV) 
			res = !res;

		UserDebug1(UserApp3, "PonyIOInterface::SetControlLine() %x\n", lcrOfst);

		if (res)
#ifdef	_WINDOWS
			SetCommBreak(hCom);
		else
			ClearCommBreak(hCom);
#else
			OutPort(BREAK_MASK, lcrOfst);
		else
			OutPort(0, lcrOfst);
#endif
	}
}

//Note that with PonyProg SI interface 3 lines provide the power:
//  SCL, SDA, RESET. If any of these lines are on (>5V) the power is ON
int PonyIOInterface::SetPower(int onoff)
{
	UserDebug1(UserApp3, "PonyIOInterface::SetPower(%d)\n", onoff);

	if (onoff)
	{
		SetControlLine(1);
//		SetSCLSDA();		//29/05/98 non funziona la verifica subito dopo la scrittura delle 93Cx6
	}
	else
	{
		ClearClockData();
		SetControlLine(0);
	}

	return OK;
}

int PonyIOInterface::Open(int com_no)
{
	UserDebug1(UserApp1, "PonyIOInterface::Open(%d) IN\n", com_no);

	int ret_val = OK;

	if (IsInstalled() != com_no)
	{
		if ( (ret_val = OpenSerial(com_no)) == OK )
			Install(com_no);
	}

	UserDebug1(UserApp2, "PonyIOInterface::Open() = %d OUT\n", ret_val);

	return ret_val;
}

void PonyIOInterface::Close()
{
	UserDebug(UserApp1, "PonyIOInterface::Close() IN\n");

	if (IsInstalled())
	{
		SetPower(0);
		CloseSerial();
		Install(0);
	}

	UserDebug(UserApp2, "PonyIOInterface::Close() OUT\n");
}

void PonyIOInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "PonyIOInterface::SetDataOut(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & DOUTINV) )
			sda = !sda;

		if (sda)
			OutPortMask(WF_SDA, 1);
		else
			OutPortMask(WF_SDA, 0);
	}
}

void PonyIOInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "PonyIOInterface::SetClock(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & CLOCKINV) )
			scl = !scl;

		if (scl)
			OutPortMask(WF_SCL, 1);
		else
			OutPortMask(WF_SCL, 0);
	}
}

void PonyIOInterface::SetClockData()
{
	UserDebug1(UserApp3, "PonyIOInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control	= THEAPP->GetPolarity();
		uint8_t cpreg = GetCPWReg();

		if (control & CLOCKINV)
			cpreg &= ~WF_SCL;
		else
			cpreg |= WF_SCL;

		if (control & DOUTINV)
			cpreg &= ~WF_SDA;
		else
			cpreg |= WF_SDA;

		OutPort(cpreg);
	}
}


void PonyIOInterface::ClearClockData()
{
	UserDebug1(UserApp3, "PonyIOInterface::ClearClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control = THEAPP->GetPolarity(); 
		uint8_t cpreg = GetCPWReg();

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

int PonyIOInterface::GetDataIn() 
{
	UserDebug1(UserApp3, "PonyIOInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & DININV) 
			return ~ InPort() & RF_SDA;
		else
			return InPort() & RF_SDA;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int PonyIOInterface::GetClock() 
{
	return 1;
}

int PonyIOInterface::GetPresence() const
{
	UserDebug1(UserApp3, "PonyIOInterface::GetPresence() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		return InPort() & RF_TEST;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int PonyIOInterface::IsClockDataUP() 
{
	UserDebug1(UserApp3, "PonyIOInterface::IsClockDataUP() *** Inst=%d\n", IsInstalled());

	return GetDataIn();
}

int PonyIOInterface::IsClockDataDOWN() 
{
	UserDebug1(UserApp3, "PonyIOInterface::IsClockDataDOWN() *** Inst=%d\n", IsInstalled());

	return !GetDataIn();
}

int PonyIOInterface::TestPort(int com_no)
{
	UserDebug1(UserApp1, "PonyIOInterface::TestPort(%d) IN\n", com_no);

	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		int a,b;
		Wait w;

		ret_val = E2ERR_OPENFAILED;

		SetClockData();
		w.WaitMsec(50);
		a = (GetCPWReg() & WF_SCL) ? 1 : 0;
		b = GetPresence() ? 1 : 0;

		if (a == b)
		{
			ClearClockData();
			w.WaitMsec(50);
			a = (GetCPWReg() & WF_SCL) ? 1 : 0;
			b = GetPresence() ? 1 : 0;

			if (a == b)
			{
				SetClockData();
				w.WaitMsec(50);
				a = (GetCPWReg() & WF_SCL) ? 1 : 0;
				b = GetPresence() ? 1 : 0;

				if (a == b)
					ret_val = OK;
			}
		}
	}
	TestRestore();

	UserDebug1(UserApp2, "PonyIOInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}
