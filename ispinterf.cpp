//=========================================================================//
//-------------------------------------------------------------------------//
// ispinterf.cpp -- AVR ISP card interface                                 //
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

#include "errcode.h"
#include "ispinterf.h"
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

#undef	AVRISP
#define	AVRISP

#ifdef	AVRISP			//Real AVR ISP
# define WB_TEST1 0		/* DATA (pin 2) */
# define WB_TEST2 1		/* DATA (pin 3) */
# define WB_ENA1 2		/* DATA (pin 4) */
# define WB_ENA2 3		/* DATA (pin 5) */
# define WB_SCK	4		/* DATA (pin 6) */
# define WB_DOUT 5		/* DATA (pin 7) */
# define WB_LED 6		/* DATA (pin 8) */
# define WB_RST 7		/* DATA (pin 9) */

# define RB_DIN	6		/* STATUS (pin 10) */
# define RB_TEST1 5		/* STATUS (pin 12) */
# define RB_TEST2 7		/* STATUS (pin 11) */

#else
# define WB_RST	0		/* DATA (pin 2) */
# define WB_SCK 1		/* DATA (pin 3) */
# define WB_DOUT 2		/* DATA (pin 4) */

# define WB_ENA2 6		/* DATA (pin 8) */
# define WB_TEST2 7		/* DATA (pin 9) */

# define WB_LED 0		/* CTRL (pin 1)- */
# define WB_ENA1 2		/* CTRL (pin 16)- */
# define WB_TEST1 3		/* CTRL (pin 17)- */

# define RB_DIN	6		/* STATUS (pin 10) */
# define RB_TEST2 7		/* STATUS (pin 11) */
# define RB_TEST1 5		/* STATUS (pin 12) */
#endif

# define WF_TEST1	(1 << WB_TEST1)
# define WF_TEST2	(1 << WB_TEST2)
# define WF_ENA1	(1 << WB_ENA1)
# define WF_ENA2	(1 << WB_ENA2)
# define WF_SCK		(1 << WB_SCK)
# define WF_DOUT	(1 << WB_DOUT)
# define WF_LED		(1 << WB_LED)
# define WF_RST		(1 << WB_RST)

# define RF_DIN		(1 << RB_DIN)
# define RF_TEST1	(1 << RB_TEST1)
# define RF_TEST2	(1 << RB_TEST2)


AvrISPInterface::AvrISPInterface()
{
	UserDebug(Constructor, "AvrISPInterface::AvrISPInterface() Constructor\n");

	Install(0);
	old_portno = 0;
}

void AvrISPInterface::SetControlLine(int res)
{
	UserDebug2(UserApp3, "AvrISPInterface::SetControlLine(%d) *** Inst=%d\n", res, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & RESETINV) 
			res = !res;

		//1) Open collector RESET behaviour
		//OutDataMask(WF_RST|WF_ENA1, !res);	//When not resetted leave the RESET in Hi-Z

		//2) push-pull RESET behaviour
		OutDataMask(WF_ENA1, 0);				//Never leave the RESET in Hi-Z (AVR ISP seems to work in this way)
		OutDataMask(WF_RST, !res);
	}
}


int AvrISPInterface::SetPower(int onoff)
{
#ifdef	AVRISP
	if (onoff)
	{
		OutDataMask(WF_RST|WF_ENA1, 0);
		OutDataMask(WF_LED, 0);		//31/07/99 -- suggested by Ralph Koettlitz <koettlitz@gmx.net>
		OutDataMask(WF_ENA2, 0);
	}
	else
	{
		OutDataMask(WF_ENA2, 1);
		OutDataMask(WF_LED, 1);		//31/07/99 -- suggested by Ralph Koettlitz <koettlitz@gmx.net>
		OutDataMask(WF_RST|WF_ENA1, 1);
	}
#else
	if (onoff)
	{
		OutDataMask(WF_RST, 1);
		OutControlMask(WF_LED, 0);
		OutControlMask(WF_ENA1, 1);
	}
	else
	{
		OutControlMask(WF_ENA1, 0);
		OutControlMask(WF_LED, 1);
		OutDataMask(WF_RST, 0);
	}
#endif

	UserDebug2(UserApp3, "AvrISPInterface::SetPower(%d) *** Inst=%d\n", onoff, IsInstalled());

	return OK;
}


int AvrISPInterface::Open(int port_no)
{
	UserDebug2(UserApp1, "AvrISPInterface::Open(%d) IN *** Inst=%d\n", port_no, IsInstalled());

	int ret_val = OK;

	if (IsInstalled() != port_no)
	{
		if ( InDataPort(port_no) < 0 )
			ret_val = E2ERR_OPENFAILED;
		else
		{
			Install(port_no);
#ifdef	AVRISP
			OutDataMask(WF_ENA1|WF_ENA2, 1);
#else
			OutControlMask(WF_ENA1, 0);
#endif
		}
	}

	UserDebug1(UserApp2, "AvrISPInterface::Open() = %d OUT\n", ret_val);

	return ret_val;
}

void AvrISPInterface::Close()
{
	UserDebug1(UserApp1, "AvrISPInterface::Close() IN *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		Install(0);
	}

	UserDebug(UserApp2, "AvrISPInterface::Close() OUT\n");
}

void AvrISPInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "AvrISPInterface::SetDataOut(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity()&DOUTINV) 
			sda = !sda;

		OutDataMask(WF_DOUT, sda ? 1 : 0);
	}
}

void AvrISPInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "AvrISPInterface::SetClock(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity()&CLOCKINV) 
			scl = !scl;

		OutDataMask(WF_SCK, scl ? 1 : 0);
	}
}

void AvrISPInterface::SetClockData()
{
	UserDebug1(UserApp3, "AvrISPInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control	= THEAPP->GetPolarity();
		UBYTE cpreg = GetLastData();

		if (control & CLOCKINV)
			cpreg &= ~WF_SCK;
		else
			cpreg |= WF_SCK;

		if (control & DOUTINV)
			cpreg &= ~WF_DOUT;
		else
			cpreg |= WF_DOUT;

		OutDataPort(cpreg);
	}
}


void AvrISPInterface::ClearClockData()
{
	UserDebug1(UserApp3, "AvrISPInterface::ClearClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control = THEAPP->GetPolarity(); 
		UBYTE cpreg = GetLastData();

		if (control & CLOCKINV)
			cpreg |= WF_SCK;
		else
			cpreg &= ~WF_SCK;

		if (control & DOUTINV)
			cpreg |= WF_DOUT;
		else
			cpreg &= ~WF_DOUT;

		OutDataPort(cpreg);
	}
}

int AvrISPInterface::GetDataIn()
{
	UserDebug1(UserApp3, "AvrISPInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		if (THEAPP->GetPolarity() & DININV) 
			return ~ InDataPort() & RF_DIN;
		else
			return InDataPort() & RF_DIN;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int AvrISPInterface::GetClock() 
{
	return 1;
}

int AvrISPInterface::GetPresence()
{
	UserDebug1(UserApp3, "AvrISPInterface::GetPresence() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		return (InDataPort() & RF_TEST1);
	}
	else
		return E2ERR_NOTINSTALLED;
}

int AvrISPInterface::IsClockDataUP()
{
	UserDebug1(UserApp3, "AvrISPInterface::IsClockDataUP() *** Inst=%d\n", IsInstalled());

	return GetDataIn();
}

int AvrISPInterface::IsClockDataDOWN()
{
	UserDebug1(UserApp3, "AvrISPInterface::IsClockDataDOWN() *** Inst=%d\n", IsInstalled());

	return !GetDataIn();
}

int AvrISPInterface::TestPort(int com_no)
{
	UserDebug1(UserApp1, "AvrISPInterface::TestPort(%d) IN\n", com_no);

	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		int test1 = FALSE, test2 = FALSE;
		Wait w;

#ifdef	AVRISP
		//Test1
		OutDataMask(WF_TEST1, 1);
		w.WaitMsec(50);
		if ( (InDataPort() & RF_TEST1) )
		{
			OutDataMask(WF_TEST1, 0);
			w.WaitMsec(50);
			if ( !(InDataPort() & RF_TEST1) )
				test1 = TRUE;
		}
#endif
		//Test2
		OutDataMask(WF_TEST2, 0);
		w.WaitMsec(50);
		if ( (InDataPort() & RF_TEST2) )
		{
			OutDataMask(WF_TEST2, 1);
			w.WaitMsec(50);
			if ( !(InDataPort() & RF_TEST2) )
				test2 = TRUE;
		}
		ret_val = ( test1 || test2 ) ?
					OK :
					E2ERR_OPENFAILED;
	}
	TestRestore();

	UserDebug1(UserApp2, "AvrISPInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}
