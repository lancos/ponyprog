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

#include "pgminter.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QDebug>


class e2CmdWindow;

SIProgInterface::SIProgInterface()
	: RS232Interface()
{
	//qDebug() << "SIProgInterface::SIProgInterface()";

	//DeInstall();
	//old_portno = GetInstalled();
}

//SIProgInterface::~SIProgInterface()
//{
//}

//se  res == 1  abilita il BREAK --> Tx = +12v
// altrimenti disabilita il BREAK --> Tx = -12v
void SIProgInterface::SetControlLine(int res)
{
	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		SetSerialBreak(res);
	}
}

//La alimentazione e` data dalle tre linee: SCL, SDA, RESET.
// Se una qualsiasi di queste tre linee e` a 1 vi e` alimentazione
int SIProgInterface::SetPower(bool onoff)
{
	if (onoff)
	{
		SetControlLine(1);
		//SetClockData();         //29/05/98 non funziona la verifica subito dopo la scrittura delle 93Cx6
	}
	else
	{
		ClearClockData();
		SetControlLine(0);
	}

	return OK;
}

int SIProgInterface::Open(int com_no)
{
	qDebug() << "SIProgInterface::Open(" << com_no << ") IN *** Inst=" << IsInstalled();

	int ret_val = OK;

	if (GetInstalled() != com_no)
	{
		if ((ret_val = RS232Interface::OpenSerial(com_no)) == OK)
		{
			//      SetSerialEventMask(0);
			//SetPower(true);  //08/02/1998 -- ora diamo alimentazione prima di ogni operazione e la togliamo subito dopo

			Install(com_no);
		}
	}

	qDebug() << "SIProgInterface::Open() = " << ret_val << " OUT";

	return ret_val;
}

void SIProgInterface::Close()
{
	qDebug() << "SIProgInterface::Close() IN *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetPower(false);
		//SetCommMask(hCom, old_mask);
		DeInstall();
		RS232Interface::CloseSerial();
	}

	qDebug() << "SIProgInterface::Close() OUT";
}

void SIProgInterface::SetDataOut(int sda)
{
//	qDebug() << "SIProgInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DOUTINV)
		{
			sda = !sda;
		}

		SetSerialDTR(sda);
	}
}

void SIProgInterface::SetClock(int scl)
{
//	qDebug() << "SIProgInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & CLOCKINV)
		{
			scl = !scl;
		}

		SetSerialRTS(scl);
	}
}

void SIProgInterface::SetClockData()
{
//	qDebug() << "SIProgInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int control = cmdWin->GetPolarity();

		if ((control & CLOCKINV) && (control & DOUTINV))
		{
			SetSerialRTSDTR(0);
		}
		else if (!(control & CLOCKINV) && !(control & DOUTINV))
		{
			SetSerialRTSDTR(1);
		}
		else
		{
			if (control & CLOCKINV)
			{
				SetSerialRTS(0);
				SetSerialDTR(1);
			}
			else
			{
				SetSerialRTS(1);
				SetSerialDTR(0);
			}
		}
	}
}


void SIProgInterface::ClearClockData()  //aggiunto il 06/03/98
{
//	qDebug() << "SIProgInterface::ClearClockData() *** Inst=%" << IsInstalled();

	if (IsInstalled())
	{
		int control = cmdWin->GetPolarity();

		if ((control & CLOCKINV) && (control & DOUTINV))
		{
			SetSerialRTSDTR(1);
		}
		else if (!(control & CLOCKINV) && !(control & DOUTINV))
		{
			SetSerialRTSDTR(0);
		}
		else
		{
			if (control & CLOCKINV)
			{
				SetSerialRTS(1);
				SetSerialDTR(0);
			}
			else
			{
				SetSerialRTS(0);
				SetSerialDTR(1);
			}
		}
	}
}

int SIProgInterface::GetDataIn()
{
//	qDebug() << "SIProgInterface::GetDataIn() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & DININV)
		{
			return !GetSerialCTS();
		}
		else
		{
			return GetSerialCTS();
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int SIProgInterface::GetClock()
{
	return 1;
}

int SIProgInterface::IsClockDataUP()
{
//	qDebug() << "SIProgInterface::GetSCLSDAup() *** Inst=" << IsInstalled();

	return GetDataIn();
}

int SIProgInterface::IsClockDataDOWN()
{
//	qDebug() << "SIProgInterface::GetSCLSDAdown() *** Inst=" << IsInstalled();

	return !GetDataIn();
}

int SIProgInterface::TestPort(int com_no)
{
	qDebug() << "SIProgInterface::TestPort(" << com_no << ") IN";

	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		ret_val = E2ERR_OPENFAILED;
		Wait w;

		SetPower(false);
		w.WaitMsec(100);

		SetSerialRTS(true);
		w.WaitMsec(50);

		if (GetSerialDSR())
		{
			SetSerialRTS(false);
			w.WaitMsec(50);

			if (!GetSerialDSR())
			{
				SetSerialRTS(true);
				w.WaitMsec(50);

				if (GetSerialDSR())
				{
					ret_val = OK;
				}
			}
		}

		SetPower(false);
	}

	TestRestore();

	qDebug() << "SIProgInterface::TestPort() = " << ret_val << " OUT";

	return ret_val;
}

/***
//Salva le porte attuali ed apre una nuova seriale (per Test)
int SIProgInterface::TestSave(int port_no)
{
        qDebug() << "SIProgInterface::TestSave(" << port_no<< ") IN";

        int ret_val;

        old_portno = IsInstalled();

        Close();
        if ( (ret_val = Open(port_no)) == OK )
        {
        }

        qDebug() << "SIProgInterface::TestSave() = "<< ret_val<<" OUT";

        return ret_val;
}

//Ripristina la situazione precedente a TestSave()
void SIProgInterface::TestRestore()
{
        qDebug() << "SIProgInterface::TestRestore() IN *** Inst="<< IsInstalled();

        if (IsInstalled())
        {
                Close();
        }
        if (old_portno)
        {
                Open(old_portno);
        }
        old_portno = 0;

        qDebug() << "SIProgInterface::TestRestore() OUT";
}
***/
