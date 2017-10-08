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
// PonyIO (SI-Prog) card interface

#include "ponyioint.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QDebug>

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

# define WB_SCL 1               /* COM: rts (pin 7) */
# define WB_SDA 0               /* COM: dtr (pin 4) */
# undef RB_SCL                  /* Not used */
# define RB_SDA 4               /* COM: cts (pin 8) */
# define RB_TEST 5              /* COM: dsr (pin 6) */

# define WF_SCL (1 << WB_SCL)
# define WF_SDA (1 << WB_SDA)
//# define RF_SCL (1 << RB_SCL)
# define RF_SDA (1 << RB_SDA)
# define RF_TEST (1 << RB_TEST)

#ifdef  __linux__
# include <sys/io.h>
# include <unistd.h>
#endif

enum UartRegs
{
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
	//qDebug() << "PonyIOInterface::PonyIOInterface()";

	//DeInstall();
	//old_portno = GetInstalled();
}

PonyIOInterface::~PonyIOInterface()
{
	Close();
}

#define BREAK_MASK      0x40

//se  res == 1  abilita il BREAK --> Tx = +12v
// altrimenti disabilita il BREAK --> Tx = -12v
void PonyIOInterface::SetControlLine(int res)
{
	qDebug() << "PonyIOInterface::SetControlLine(" << res << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		qDebug() << "PonyIOInterface::SetControlLine() XX";

		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		qDebug() << "PonyIOInterface::SetControlLine() " << (hex) << lcrOfst  << (dec);

#ifdef  Q_OS_WIN32
		if (res)
		{
			SetCommBreak(hCom);
		}
		else
		{
			ClearCommBreak(hCom);
		}
#else
		if (res)
		{
			OutPort(BREAK_MASK, lcrOfst);
		}
		else
		{
			OutPort(0, lcrOfst);
		}
#endif
	}
}

//Note that with PonyProg SI interface 3 lines provide the power:
//  SCL, SDA, RESET. If any of these lines are on (>5V) the power is ON
int PonyIOInterface::SetPower(bool onoff)
{
	qDebug() << "PonyIOInterface::SetPower(" << onoff << ")";

	if (onoff)
	{
		SetControlLine(1);
		//              SetSCLSDA();            //29/05/98 non funziona la verifica subito dopo la scrittura delle 93Cx6
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
	qDebug() << "PonyIOInterface::Open(" << com_no << ") IN";

	int ret_val = OK;

	if (GetInstalled() != com_no)
	{
		if ((ret_val = OpenSerial(com_no)) == OK)
		{
			Install(com_no);
		}
	}

	qDebug() << "PonyIOInterface::Open() = " << ret_val << " OUT";

	return ret_val;
}

void PonyIOInterface::Close()
{
	qDebug() << "PonyIOInterface::Close() IN";

	if (IsInstalled())
	{
		SetPower(false);
		CloseSerial();
		DeInstall();
	}

	qDebug() << "PonyIOInterface::Close() OUT";
}

void PonyIOInterface::SetDataOut(int sda)
{
	qDebug() << "PonyIOInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		if (sda)
		{
			OutPortMask(WF_SDA, 1);
		}
		else
		{
			OutPortMask(WF_SDA, 0);
		}
	}
}

void PonyIOInterface::SetClock(int scl)
{
	qDebug() << "PonyIOInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		if (scl)
		{
			OutPortMask(WF_SCL, 1);
		}
		else
		{
			OutPortMask(WF_SCL, 0);
		}
	}
}

void PonyIOInterface::SetClockData()
{
	qDebug() << "PonyIOInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int control     = cmdWin->GetPolarity();
		uint8_t cpreg = GetCPWReg();

		if (control & CLOCKINV)
		{
			cpreg &= ~WF_SCL;
		}
		else
		{
			cpreg |= WF_SCL;
		}

		if (control & DOUTINV)
		{
			cpreg &= ~WF_SDA;
		}
		else
		{
			cpreg |= WF_SDA;
		}

		OutPort(cpreg);
	}
}


void PonyIOInterface::ClearClockData()
{
	qDebug() << "PonyIOInterface::ClearClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int control = cmdWin->GetPolarity();
		uint8_t cpreg = GetCPWReg();

		if (control & CLOCKINV)
		{
			cpreg |= WF_SCL;
		}
		else
		{
			cpreg &= ~WF_SCL;
		}

		if (control & DOUTINV)
		{
			cpreg |= WF_SDA;
		}
		else
		{
			cpreg &= ~WF_SDA;
		}

		OutPort(cpreg);
	}
}

int PonyIOInterface::GetDataIn()
{
	qDebug() << "PonyIOInterface::GetDataIn() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DININV)
		{
			return ~ InPort() & RF_SDA;
		}
		else
		{
			return InPort() & RF_SDA;
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int PonyIOInterface::GetClock()
{
	return 1;
}

int PonyIOInterface::GetPresence() const
{
	qDebug() << "PonyIOInterface::GetPresence() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		return InPort() & RF_TEST;
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int PonyIOInterface::IsClockDataUP()
{
	qDebug() << "PonyIOInterface::IsClockDataUP() *** Inst=" << IsInstalled();

	return GetDataIn();
}

int PonyIOInterface::IsClockDataDOWN()
{
	qDebug() << "PonyIOInterface::IsClockDataDOWN() *** Inst=" << IsInstalled();

	return !GetDataIn();
}

int PonyIOInterface::TestPort(int com_no)
{
	qDebug() << "PonyIOInterface::TestPort(" << com_no << ") IN";

	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		int a, b;
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
				{
					ret_val = OK;
				}
			}
		}
	}

	TestRestore();

	qDebug() << "PonyIOInterface::TestPort() = " << ret_val << " OUT";

	return ret_val;
}
