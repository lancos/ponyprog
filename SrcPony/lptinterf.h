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

#ifndef _LPTINTERFACE_H
#define _LPTINTERFACE_H

#include <QtCore>
#include "types.h"

#define LPTCON_CLOSEHANDLE      -1
#define LPTCON_GETVERSION       0
#define LPTCON_READ             10
#define LPTCON_WRITE_DATA       20
#define LPTCON_WRITE_CONTROL    30

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE    -1
#endif

//Define the structure used to communicate to the VxD.
typedef struct
{
	int     LPPort;           //Port can be 1-3
	uint8_t LPByte;           //Data can be any value between 00 and FFh
} LPTCONDATA;

class LPTInterface
{
  public:                //------------------------------- public
	LPTInterface();
	virtual ~LPTInterface();

	void Close();

	void SetPort(int port_no);
	int InDataPort(int port_no = -1);
	int OutDataPort(int val, int port_no = -1);
	int OutControlPort(int val, int port_no = -1);
	int OutDataMask(int mask, int val);
	virtual int OutControlMask(int mask, int val);

	int GetLastData() const
	{
		return last_data;
	}
	int GetLastCtrl() const
	{
		return last_ctrl;
	}

  protected:             //------------------------------- protected

  private:               //------------------------------- private
#ifdef Q_OS_LINUX
	int hLpt;
#endif
	LPTCONDATA lpt_control;
	uint8_t last_ctrl, last_data;
};

#endif
