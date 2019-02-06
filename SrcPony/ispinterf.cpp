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

#include "errcode.h"
#include "globals.h"
#include "wait.h"


#include "ispinterf.h"

#include "e2cmdw.h"

#include <QDebug>

/* data register (0x378)
 * bit 0: D0    (pin 2)
 * bit 1: D1    (pin 3)
 * bit 2: D2    (pin 4)
 * bit 3: D3    (pin 5)
 * bit 4: D4    (pin 6)
 * bit 5: D5    (pin 7)
 * bit 6: D6    (pin 8)
 * bit 7: D7    (pin 9)
 */

/* control register (0x37A)
 * bit 0: STROBE        (pin 1)-
 * bit 1: AUTOLF        (pin 14)-
 * bit 2: INIT          (pin 16)-
 * bit 3: SELECTIN      (pin 17)-
 */

/* status register (0x379)
 * bit 3: ERROR         (pin 15)
 * bit 4: SELECT        (pin 13)
 * bit 5: POUT          (pin 12)
 * bit 6: ACK           (pin 10)
 * bit 7: BUSY          (pin 11)-
 */

//Real AVR ISP
#define AVRISP_MAPPED

#ifdef  AVRISP_MAPPED
//Real AVR ISP
#define WB_TEST1 0             /* DATA (pin 2) */
#define WB_TEST2 1             /* DATA (pin 3) */
#define WB_ENA1 2              /* DATA (pin 4) */
#define WB_ENA2 3              /* DATA (pin 5) */
#define WB_SCK 4               /* DATA (pin 6) */
#define WB_DOUT 5              /* DATA (pin 7) */
#define WB_LED 6               /* DATA (pin 8) */
#define WB_RST 7               /* DATA (pin 9) */

#define RB_DIN 6               /* STATUS (pin 10) */
#define RB_TEST1 5             /* STATUS (pin 12) */
#define RB_TEST2 7             /* STATUS (pin 11) */

#else
//Remapped AVR ISP
#define WB_RST 0               /* DATA (pin 2) */
#define WB_SCK 1               /* DATA (pin 3) */
#define WB_DOUT 2              /* DATA (pin 4) */

#define WB_ENA2 6              /* DATA (pin 8) */
#define WB_TEST2 7             /* DATA (pin 9) */

#define WB_LED 0               /* CTRL (pin 1)- */
#define WB_ENA1 2              /* CTRL (pin 16)- */
#define WB_TEST1 3             /* CTRL (pin 17)- */

#define RB_DIN 6               /* STATUS (pin 10) */
#define RB_TEST2 7             /* STATUS (pin 11) */
#define RB_TEST1 5             /* STATUS (pin 12) */
#endif

#define WF_TEST1       (1 << WB_TEST1)
#define WF_TEST2       (1 << WB_TEST2)
#define WF_ENA1        (1 << WB_ENA1)
#define WF_ENA2        (1 << WB_ENA2)
#define WF_SCK         (1 << WB_SCK)
#define WF_DOUT        (1 << WB_DOUT)
#define WF_LED         (1 << WB_LED)
#define WF_RST         (1 << WB_RST)

#define RF_DIN         (1 << RB_DIN)
#define RF_TEST1       (1 << RB_TEST1)
#define RF_TEST2       (1 << RB_TEST2)


AvrISPInterface::AvrISPInterface(bool use_io)
	: LptExtInterface(use_io)
{
	//qDebug() << "AvrISPInterface::AvrISPInterface()";

	//DeInstall();
	//old_portno = GetInstalled();
}

void AvrISPInterface::SetControlLine(int res)
{
	qDebug() << "AvrISPInterface::SetControlLine(" << res << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		//1) Open collector RESET behaviour
		//OutDataMask(WF_RST|WF_ENA1, !res);    //When not resetted leave the RESET in Hi-Z

		//2) push-pull RESET behaviour
		OutDataMask(WF_ENA1, 0);                                //Never leave the RESET in Hi-Z (AVR ISP seems to work in this way)
		OutDataMask(WF_RST, !res);
	}
}


int AvrISPInterface::SetPower(bool onoff)
{
#ifdef  AVRISP_MAPPED

	if (onoff)
	{
		OutDataMask(WF_TEST2, 1);       //Some PCs need /BUSY signal high to work properly as suggested by Marek Michalkiewicz <marekm@linux.org.pl>

		OutDataMask(WF_RST | WF_ENA1, 0);
		OutDataMask(WF_LED, 0);         //31/07/99 -- suggested by Ralph Koettlitz <koettlitz@gmx.net>
		OutDataMask(WF_ENA2, 0);
	}
	else
	{
		OutDataMask(WF_ENA2, 1);
		OutDataMask(WF_LED, 1);         //31/07/99 -- suggested by Ralph Koettlitz <koettlitz@gmx.net>
		OutDataMask(WF_RST | WF_ENA1, 1);
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

	qDebug() << "AvrISPInterface::SetPower(" << onoff << ") *** Inst=" << IsInstalled();

	return OK;
}


int AvrISPInterface::Open(int port_no)
{
	qDebug() << "AvrISPInterface::Open(" << port_no << ") IN *** Inst=" << IsInstalled();

	int ret_val = OK;

	if (GetInstalled() != port_no)
	{
		if (InDataPort(port_no) < 0)
		{
			ret_val = E2ERR_OPENFAILED;
		}
		else
		{
			Install(port_no);
#ifdef  AVRISP_MAPPED
			OutDataMask(WF_ENA1 | WF_ENA2, 1);
#else
			OutControlMask(WF_ENA1, 0);
#endif
		}
	}

	qDebug() << "AvrISPInterface::Open() = " << ret_val << " OUT";

	return ret_val;
}

void AvrISPInterface::Close()
{
	qDebug() << "AvrISPInterface::Close() IN *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		LptExtInterface::Close();

		DeInstall();
	}

	qDebug() << "AvrISPInterface::Close() OUT";
}

void AvrISPInterface::SetDataOut(int sda)
{
	qDebug() << "AvrISPInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity()&DOUTINV)
		{
			sda = !sda;
		}

		OutDataMask(WF_DOUT, sda ? 1 : 0);
	}
}

void AvrISPInterface::SetClock(int scl)
{
	qDebug() << "AvrISPInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity()&CLOCKINV)
		{
			scl = !scl;
		}

		OutDataMask(WF_SCK, scl ? 1 : 0);
	}
}

void AvrISPInterface::SetClockData()
{
	qDebug() << "AvrISPInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int control     = cmdWin->GetPolarity();
		uint8_t cpreg = GetLastData();

		if (control & CLOCKINV)
		{
			cpreg &= ~WF_SCK;
		}
		else
		{
			cpreg |= WF_SCK;
		}

		if (control & DOUTINV)
		{
			cpreg &= ~WF_DOUT;
		}
		else
		{
			cpreg |= WF_DOUT;
		}

		OutDataPort(cpreg);
	}
}


void AvrISPInterface::ClearClockData()
{
	qDebug() << "AvrISPInterface::ClearClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int control = cmdWin->GetPolarity();
		uint8_t cpreg = GetLastData();

		if (control & CLOCKINV)
		{
			cpreg |= WF_SCK;
		}
		else
		{
			cpreg &= ~WF_SCK;
		}

		if (control & DOUTINV)
		{
			cpreg |= WF_DOUT;
		}
		else
		{
			cpreg &= ~WF_DOUT;
		}

		OutDataPort(cpreg);
	}
}


int AvrISPInterface::GetDataIn()
{
	qDebug() << "AvrISPInterface::GetDataIn() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DININV)
		{
			return ~ InDataPort() & RF_DIN;
		}
		else
		{
			return InDataPort() & RF_DIN;
		}
	}

	return E2ERR_NOTINSTALLED;
}


int AvrISPInterface::GetClock()
{
	return 1;
}

int AvrISPInterface::GetPresence()
{
	qDebug() << "AvrISPInterface::GetPresence() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		return (InDataPort() & RF_TEST1);
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int AvrISPInterface::IsClockDataUP()
{
	qDebug() << "AvrISPInterface::IsClockDataUP() *** Inst=" << IsInstalled();

	return GetDataIn();
}

int AvrISPInterface::IsClockDataDOWN()
{
	qDebug() << "AvrISPInterface::IsClockDataDOWN() *** Inst=" << IsInstalled();

	return !GetDataIn();
}

int AvrISPInterface::TestPort(int port)
{
	qDebug() << "AvrISPInterface::TestPort(" << port << ") IN";

	int ret_val = TestSave(port);
	Wait w;

	if (ret_val == OK)
	{
		int test1 = false, test2 = false;

#ifdef  AVRISP_MAPPED
		//Test1
		OutDataMask(WF_TEST1, 1);
		w.WaitMsec(50);

		if ((InDataPort() & RF_TEST1))
		{
			OutDataMask(WF_TEST1, 0);
			w.WaitMsec(50);

			if (!(InDataPort() & RF_TEST1))
			{
				test1 = true;
			}
		}

#endif
		//Test2
		OutDataMask(WF_TEST2, 0);
		w.WaitMsec(50);

		if ((InDataPort() & RF_TEST2))
		{
			OutDataMask(WF_TEST2, 1);
			w.WaitMsec(50);

			if (!(InDataPort() & RF_TEST2))
			{
				test2 = true;
			}
		}

		ret_val = (test1 || test2) ?
				  OK :
				  E2ERR_OPENFAILED;
	}

	TestRestore();

	qDebug() << "AvrISPInterface::TestPort() = " << ret_val << " OUT";

	return ret_val;
}
