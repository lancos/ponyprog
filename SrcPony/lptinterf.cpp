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

#include "types.h"
#include "errcode.h"
#include "e2profil.h"

#include <QDebug>

#ifdef  Q_OS_WIN32
#include <windows.h>
#endif

#ifdef __linux__

//If you use latest kernel (2.4.xx) you should use standard ppdev.o module and include ppdev.h
//otherwise install the ppuser module, and include the file ppuser.h
#define USE_K2_4_PPDEV  1

// #include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#if USE_K2_4_PPDEV == 1
# include "linux/ppdev.h"
# define PARPORTDEVNAME "/dev/parport%d"
//N.B. be sure to issue a "chmod 666 /dev/parport?"
#else
# include "ppuser.h"
# define PARPORTDEVNAME "/dev/ppuser%d0"
//N.B. be sure to issue a "chmod 666 /dev/ppuser?"
#endif

#endif



#include "globals.h"
#include "lptinterf.h"

#define MAX_LPTPORTS    3

LPTInterface::LPTInterface()
{
	qDebug() << "LPTInterface::LPTInterface()";

	last_ctrl = last_data = 0;
	lpt_control.LPPort = -1;         //no used port

#ifdef __linux__
	hLpt = INVALID_HANDLE_VALUE;
#endif
}

LPTInterface::~LPTInterface()
{
	qDebug() <<  "LPTInterface::~LPTInterface()";

	Close();
}

void LPTInterface::Close()
{
#ifdef __linux__
	qDebug() << "LPTInterface::Close() I ** hLpt = " << hLpt;

	if (hLpt != INVALID_HANDLE_VALUE)
	{
		ioctl(hLpt, PPRELEASE, 0);
		close(hLpt);
		hLpt = INVALID_HANDLE_VALUE;
		lpt_control.LPPort = 0;
	}

	qDebug() << "LPTInterface::Close() O ** hLpt = " << hLpt;
#endif
}

void LPTInterface::SetPort(int port_no)
{
	qDebug() << "LPTInterface::SetPort(" << port_no << ")";

	if (port_no >= 0 && port_no < MAX_LPTPORTS)
	{
		lpt_control.LPPort = port_no;

#ifdef __linux__
		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		QString name = E2Profile::GetLPTDevName() + QString::number(lpt_control.LPPort);
		//sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1().constData(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if (ioctl(hLpt, PPCLAIM, 0))
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}

		qDebug() << "LPTInterface::SetPort() ** hLpt = " << hLpt;
#endif
	}
}

//   Bit           Bit
//    0 - Unused    4 - Pin 13
//    1 - Unused    5 - Pin 12
//    2 - Unused    6 - Pin 10
//    3 - Pin 15    7 - Pin 11 (Inverted)
//
/////////////////////////////////////////////////////////////////

int LPTInterface::InDataPort(int port_no)
{
	qDebug() << "LPTInterface::InDataPort(" << port_no << ") ** lp=" << lpt_control.LPPort;

	int ret_val = E2ERR_NOTINSTALLED;

#ifdef __linux__
	if (port_no >= 0 && port_no < MAX_LPTPORTS
			&& port_no != lpt_control.LPPort)
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		QString name = E2Profile::GetLPTDevName() + QString::number(lpt_control.LPPort);
		qDebug() << "Open " << name;

		//sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1().constData(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if (ioctl(hLpt, PPCLAIM, 0))
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}

		qDebug() << "LPTInterface::InDataPort() ** hLpt=" << hLpt << ", " << name;
	}

	if (hLpt != INVALID_HANDLE_VALUE)
	{
		if (ioctl(hLpt, PPRSTATUS, &lpt_control.LPByte))
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = lpt_control.LPByte;
		}
	}

#endif
	qDebug() << "LPTInterface::InDataPort() = " << ret_val << " (" << lpt_control.LPByte << ")";

	return ret_val;
}


//
//   Bit           Bit
//    0 - Pin 2    4 - Pin 6
//    1 - Pin 3    5 - Pin 7
//    2 - Pin 4    6 - Pin 8
//    3 - Pin 5    7 - Pin 9
//
/////////////////////////////////////////////////////////////////

int LPTInterface::OutDataPort(int val, int port_no)
{
	qDebug() << "LPTInterface::OutDataPort(" << val << ", " << port_no << ")";

	int ret_val = E2ERR_NOTINSTALLED;

#ifdef __linux__

	if (port_no >= 0 && port_no < MAX_LPTPORTS
			&& port_no != lpt_control.LPPort)
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		QString name = E2Profile::GetLPTDevName() + QString::number(lpt_control.LPPort);
		//sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1().constData(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if (ioctl(hLpt, PPCLAIM, 0))
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}
	}

	if (hLpt != INVALID_HANDLE_VALUE)
	{
		lpt_control.LPByte = last_data = (uint8_t)val;

		if (ioctl(hLpt, PPWDATA, &lpt_control.LPByte))
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = OK;
		}
	}

#endif
	qDebug() << "LPTInterface::OutDataPort() = " << ret_val;

	return ret_val;
}


//
//   Bit
//    0 - Pin 1
//    1 - Pin 14
//    2 - Pin 16
//    3 - Pin 17      NOTE: Bits 5-7 are not used.
//
/////////////////////////////////////////////////////////////////

int LPTInterface::OutControlPort(int val, int port_no)
{
	qDebug() << "LPTInterface::OutControlPort(" << val << ", " << port_no << ")";

	int ret_val = E2ERR_NOTINSTALLED;

#ifdef __linux__

	if (port_no >= 0 && port_no < MAX_LPTPORTS
			&& port_no != lpt_control.LPPort)
	{
		lpt_control.LPPort = port_no;

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			ioctl(hLpt, PPRELEASE, 0);
			close(hLpt);
		}

		QString name = E2Profile::GetLPTDevName() + QString::number(lpt_control.LPPort);
		//sprintf(name, PARPORTDEVNAME, lpt_control.LPPort - 1);
		hLpt = open(name.toLatin1().constData(), O_RDWR);

		if (hLpt != INVALID_HANDLE_VALUE)
		{
			if (ioctl(hLpt, PPCLAIM, 0))
			{
				close(hLpt);
				hLpt = INVALID_HANDLE_VALUE;
			}
		}
	}

	if (hLpt != INVALID_HANDLE_VALUE)
	{
		lpt_control.LPByte = last_ctrl = (uint8_t)val & 0x0F;

		if (ioctl(hLpt, PPWCONTROL, &lpt_control.LPByte))
		{
			ret_val = E2ERR_ACCESSDENIED;
		}
		else
		{
			ret_val = OK;
		}
	}

#endif
	qDebug() << "LPTInterface::OutControlPort() = " << ret_val;

	return ret_val;
}

int LPTInterface::OutDataMask(int mask, int val)
{
	int ret_val = E2ERR_NOTINSTALLED;

#ifdef __linux__
	if (hLpt != INVALID_HANDLE_VALUE)
	{
		if (val == 0)
		{
			last_data &= ~mask;
		}
		else if (val == 1)
		{
			last_data |= mask;
		}
		else
		{
			last_data ^= mask;
		}

		ret_val = OutDataPort(last_data);
	}
#endif

	return ret_val;
}

int LPTInterface::OutControlMask(int mask, int val)
{
	int ret_val = E2ERR_NOTINSTALLED;

#ifdef __linux__
	if (hLpt != INVALID_HANDLE_VALUE)
	{
		if (val == 0)
		{
			last_ctrl &= ~mask;
		}
		else if (val == 1)
		{
			last_ctrl |= mask;
		}
		else
		{
			last_ctrl ^= mask;
		}

		ret_val = OutControlPort(last_ctrl);
	}
#endif

	return ret_val;
}
