//=========================================================================//
//-------------------------------------------------------------------------//
// portint.h -- Header for PortInterface class                             //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2002   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lancos@libero.it                                               //
//  http://www.LancOS.com                                                  //
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

#ifndef	_PORTINTERFACE_H
#define	_PORTINTERFACE_H

#include "types.h"

//#ifdef	vDEBUG
#include <v/vdebug.h>
//#endif

// Maximum number of printer ports that would be installed on a system
#define	MAX_LPTPORTS	4
#define	MAX_COMPORTS	4

class PortInterface
{
 public:		//------------------------------- public
	PortInterface();
	virtual ~PortInterface();

	int OpenPort(int from_port, int len = 8);
	void ClosePort();
	int OpenSerial(int no);
	void CloseSerial();
	int OpenParallel(int no);
	void CloseParallel();
	int GetFirstPort() const
		{ return first_port; }
	int GetLastPort() const
		{ return last_port; }
	int GetNoPorts() const
		{ return no_ports; }
	int GetSerBasePort(int no);
	int GetParBasePort(int no);

	virtual int InPort(int no = -1) const;
	virtual int OutPort(int val, int no = -1);
	virtual int OutPortMask(int mask, int val);

 protected:		//------------------------------- protected
	UBYTE GetCPWReg()
		{ return cpwreg; }

#ifdef	_WINDOWS
	HANDLE	hCom;
#endif

	int		write_port,		// Number of output port (write I/O port address)
			read_port;		// Number of input port (read I/O port address)
	UBYTE	cpwreg;			// write register image in memory (the content is the same of the hardware register)

 private:		//------------------------------- private
	int IOperm(int a, int b, int c);

	void DetectPorts();

#ifdef	_WINDOWS
	void DetectPorts9x();	// Win9x version
	void DetectLPTPortsNT();	// WinNT/2000 version
	void DetectCOMPortsNT();	// WinNT/2000 version

	int LPTCount;			//Number of LPT ports on the system
	int COMCount;			//Number of COM ports on the system

	DWORD	old_mask;
#else
	int lcr_copy;
	int ier_copy;
#endif

	int first_port;
	int last_port;
	int no_ports;

	int ser_ports_base[MAX_COMPORTS+1];
	int ser_ports_len [MAX_COMPORTS+1];
	int par_ports_base[MAX_LPTPORTS+1];
	int par_ports_len [MAX_LPTPORTS+1];
};

#endif
