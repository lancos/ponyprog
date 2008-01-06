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

#include "pgminter.h"
#include "errcode.h"
#include "e2app.h"	//for GetPolarity

SIProgInterface::SIProgInterface()
		: RS232Interface(THEAPP)
{
	UserDebug(Constructor, "SIProgInterface::SIProgInterface() Constructor\n");

	Install(0);
	old_portno = 0;
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
		if (THEAPP->GetPolarity() & RESETINV)
			res = !res;

		SetSerialBreak(res);
	}
}

//La alimentazione e` data dalle tre linee: SCL, SDA, RESET.
// Se una qualsiasi di queste tre linee e` a 1 vi e` alimentazione
int SIProgInterface::SetPower(int onoff)
{
	if (onoff)
	{
		SetControlLine(1);
//		SetClockData();		//29/05/98 non funziona la verifica subito dopo la scrittura delle 93Cx6
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
	UserDebug2(UserApp1, "SIProgInterface::Open(%d) IN *** Inst=%d\n", com_no, IsInstalled());

	int ret_val = OK;

	if (IsInstalled() != com_no)
	{
		if ( (ret_val = RS232Interface::OpenSerial(com_no)) == OK )
		{
		//	SetSerialEventMask(0);
			//SetPower(1);	//08/02/1998 -- ora diamo alimentazione prima di ogni operazione e la togliamo subito dopo

			Install(com_no);
		}
	}

	UserDebug1(UserApp2, "SIProgInterface::Open() = %d OUT\n", ret_val);

	return ret_val;
}

void SIProgInterface::Close()
{
	UserDebug1(UserApp1, "SIProgInterface::Close() IN *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		SetPower(0);

//		SetCommMask(hCom, old_mask);

		Install(0);
		RS232Interface::CloseSerial();
	}

	UserDebug(UserApp2, "SIProgInterface::Close() OUT\n");
}

void SIProgInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "SIProgInterface::SetSDA(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
        if (THEAPP->GetPolarity() & DOUTINV)
            sda = !sda;
 
		SetSerialDTR(sda);
	}
}

void SIProgInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "SIProgInterface::SetSCL(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
        if (THEAPP->GetPolarity()&CLOCKINV)
            scl = !scl;
 
		SetSerialRTS(scl);
	}
}

void SIProgInterface::SetClockData()
{
	UserDebug1(UserApp3, "SIProgInterface::SetSCLSDA() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control = THEAPP->GetPolarity();

		if ((control & CLOCKINV) && (control & DOUTINV))
		{
			SetSerialRTSDTR(0);
		}
		else
		if (!(control & CLOCKINV) && !(control & DOUTINV))
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


void SIProgInterface::ClearClockData()	//aggiunto il 06/03/98
{
	UserDebug1(UserApp3, "SIProgInterface::ClearSCLSDA() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		int control = THEAPP->GetPolarity();

		if ((control & CLOCKINV) && (control & DOUTINV))
		{
			SetSerialRTSDTR(1);
		}
		else
		if (!(control & CLOCKINV) && !(control & DOUTINV))
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
	UserDebug1(UserApp3, "SIProgInterface::GetSDA() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
        if (THEAPP->GetPolarity() & DININV)
			return !GetSerialCTS();
		else
			return GetSerialCTS();
	}
	else
		return E2ERR_NOTINSTALLED;
}

int SIProgInterface::GetClock() 
{
	return 1;
}

int SIProgInterface::IsClockDataUP() 
{
	UserDebug1(UserApp3, "SIProgInterface::GetSCLSDAup() *** Inst=%d\n", IsInstalled());

	return GetDataIn();
}

int SIProgInterface::IsClockDataDOWN() 
{
	UserDebug1(UserApp3, "SIProgInterface::GetSCLSDAdown() *** Inst=%d\n", IsInstalled());

	return !GetDataIn();
}

int SIProgInterface::TestPort(int com_no)
{
	UserDebug1(UserApp1, "SIProgInterface::TestPort(%d) IN\n", com_no);

	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		ret_val = E2ERR_OPENFAILED;
		Wait w;

		SetPower(FALSE);
		w.WaitMsec(100);

		SetSerialRTS(TRUE);
		w.WaitMsec(50);
		if (GetSerialDSR())
		{
			SetSerialRTS(FALSE);
			w.WaitMsec(50);
			if (!GetSerialDSR())
			{
				SetSerialRTS(TRUE);
				w.WaitMsec(50);
				if (GetSerialDSR())
				{
					ret_val = OK;
				}
			}
		}
		SetPower(FALSE);
	}
	TestRestore();

	UserDebug1(UserApp2, "SIProgInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}

/***
//Salva le porte attuali ed apre una nuova seriale (per Test)
int SIProgInterface::TestSave(int port_no)
{
	UserDebug1(UserApp1, "SIProgInterface::TestSave(%d) IN\n", port_no);

	int ret_val;

	old_portno = IsInstalled();

	Close();
	if ( (ret_val = Open(port_no)) == OK )
	{
	}

	UserDebug1(UserApp2, "SIProgInterface::TestSave() = %d OUT\n", ret_val);

	return ret_val;
}

//Ripristina la situazione precedente a TestSave()
void SIProgInterface::TestRestore()
{
	UserDebug1(UserApp1, "SIProgInterface::TestRestore() IN *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		Close();
	}
	if (old_portno)
	{
		Open(old_portno);
	}
	old_portno = 0;

	UserDebug(UserApp2, "SIProgInterface::TestRestore() OUT\n");
}
***/
