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
#include "dt006interf.h"
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

//DT-006 card
#define WB_DOUT 0              // DATA (pin 2)
#define WB_RST 2               // DATA (pin 4)
#define WB_SCK 3               // DATA (pin 5)

#define RB_DIN 7               // STATUS (pin 11)


#define WF_SCK         (1 << WB_SCK)
#define WF_DOUT        (1 << WB_DOUT)
#define WF_RST         (1 << WB_RST)

#define RF_DIN         (1 << RB_DIN)


Dt006Interface::Dt006Interface(bool use_io)
	: LptExtInterface(use_io)
{
	//DeInstall();
	//old_portno = GetInstalled();
}

void Dt006Interface::SetControlLine(int res)
{
	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		OutDataMask(WF_RST, res ? 0 : 1);
	}
}


int Dt006Interface::SetPower(bool onoff)
{
	//OutDataPort(0);         //No action
	SetControlLine(0);

	return OK;
}


int Dt006Interface::Open(int port_no)
{
	int ret_val = OK;

	qDebug() << "Dt006Interface::Open(port=" << port_no << ") IN *** installed=" << IsInstalled();

	if (GetInstalled() != port_no)
	{
		if (InDataPort(port_no) < 0)
		{
			ret_val = E2ERR_OPENFAILED;
		}
		else
		{
			Install(port_no);

			OutDataPort(0);
		}
	}

	qDebug() << "Dt006Interface::Open() = " << ret_val << " *** OUT";

	return ret_val;
}

void Dt006Interface::Close()
{
	qDebug() << "Dt006Interface::Close() IN *** installed=" << IsInstalled();

	if (IsInstalled())
	{
		LptExtInterface::Close();

		DeInstall();
	}

	qDebug() << "Dt006Interface::Close() OUT *** installed=" << IsInstalled();
}

void Dt006Interface::SetDataOut(int sda)
{
	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DOUTINV)
		{
			sda = !sda;
		}

		OutDataMask(WF_DOUT, sda ? 1 : 0);
	}
}

void Dt006Interface::SetClock(int scl)
{
	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & CLOCKINV)
		{
			scl = !scl;
		}

		OutDataMask(WF_SCK, scl ? 1 : 0);
	}
}

void Dt006Interface::SetClockData()
{
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


void Dt006Interface::ClearClockData()
{
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

int Dt006Interface::GetDataIn()
{
	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DININV)
		{
			return InDataPort() & RF_DIN;
		}
		else
		{
			return !(InDataPort() & RF_DIN);
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int Dt006Interface::GetClock()
{
	return 1;
}

int Dt006Interface::IsClockDataUP()
{
	return GetDataIn();
}

int Dt006Interface::IsClockDataDOWN()
{
	return !GetDataIn();
}

int Dt006Interface::TestPort(int com_no)
{
	(void)com_no;

	return OK;
}
