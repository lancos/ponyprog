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

#include <QDebug>

#ifdef  Q_OS_WIN32
#include <windows.h>
#endif

#ifdef  __linux__
// #include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include "lpt_io_interf.h"

enum LptRegs
{
	dataOfst,
	statOfst,
	ctrlOfst
};

LPTIOInterface::LPTIOInterface()
{
	qDebug() << "LPTIOInterface::LPTIOInterface()";

	last_ctrl = last_data = 0;
	lpt_port = -1;

	//PortInterface::OpenParallel(lpt_port);
}

LPTIOInterface::~LPTIOInterface()
{
	qDebug() <<  "LPTIOInterface::~LPTIOInterface()";

	PortInterface::CloseParallel();
}


void LPTIOInterface::SetPort(int port_no)
{
	qDebug() << "LPTIOInterface::SetPort(" << port_no << ")";

	if (port_no >= 0 && port_no < MAX_LPTPORTS)
	{
		lpt_port = port_no;

		PortInterface::CloseParallel();
		PortInterface::OpenParallel(lpt_port);
	}
}

//   Bit           Bit
//    0 - Unused    4 - Pin 13
//    1 - Unused    5 - Pin 12
//    2 - Unused    6 - Pin 10
//    3 - Pin 15    7 - Pin 11 (Inverted)
//
/////////////////////////////////////////////////////////////////

int LPTIOInterface::InDataPort(int port_no)
{
	qDebug() << "LPTIOInterface::InDataPort(" << port_no << ") ** lp=" << lpt_port;

	int ret_val = OK;

	if (port_no >= 0 && port_no < MAX_LPTPORTS &&
			port_no != lpt_port)
	{
		lpt_port = port_no;

		PortInterface::CloseParallel();
		ret_val = PortInterface::OpenParallel(lpt_port);
	}

	if (ret_val == OK)
	{
		ret_val = PortInterface::InPort(statOfst);
	}

	qDebug() << "LPTIOInterface::InDataPort() = " << ret_val;

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

int LPTIOInterface::OutDataPort(int val, int port_no)
{
	qDebug() << "LPTIOInterface::OutDataPort(" << val << ", " << port_no << ")";

	int ret_val = OK;

	if (port_no >= 0 && port_no < MAX_LPTPORTS &&
			port_no != lpt_port)
	{
		lpt_port = port_no;

		PortInterface::CloseParallel();
		ret_val = PortInterface::OpenParallel(lpt_port);
	}

	if (ret_val == OK)
	{
		last_data = val;
		ret_val = PortInterface::OutPort(val, dataOfst);
	}

	qDebug() << "LPTIOInterface::OutDataPort() = " << ret_val;

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

int LPTIOInterface::OutControlPort(int val, int port_no)
{
	qDebug() << "LPTIOInterface::OutControlPort(" << val << ", " << port_no << ")";

	int ret_val = OK;

	if (port_no >= 0 && port_no < MAX_LPTPORTS &&
			port_no != lpt_port)
	{
		lpt_port = port_no;

		PortInterface::CloseParallel();
		ret_val = PortInterface::OpenParallel(lpt_port);
	}

	if (ret_val == OK)
	{
		last_ctrl = val;
		ret_val = PortInterface::OutPort(val, ctrlOfst);
	}

	qDebug() << "LPTIOInterface::OutControlPort() = " << ret_val;

	return ret_val;
}

int LPTIOInterface::OutDataMask(int mask, int val)
{
	int ret_val;

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

	return ret_val;
}

int LPTIOInterface::OutControlMask(int mask, int val)
{
	int ret_val;

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

	return ret_val;
}
