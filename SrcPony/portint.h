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

#ifndef _PORTINTERFACE_H
#define _PORTINTERFACE_H

#include <QtCore>

#include "types.h"

#ifdef  Q_OS_WIN32
#include <windows.h>

typedef void (__stdcall *lpOut32)(short, short);
typedef short(__stdcall *lpInp32)(short);
typedef BOOL (__stdcall *lpIsInpOutDriverOpen)(void);
typedef BOOL (__stdcall *lpIsXP64Bit)(void);

#endif

// Maximum number of printer ports that would be installed on a system
#define MAX_LPTPORTS    4
#define MAX_COMPORTS    8

struct base_len
{
	int base, len;
};

class PortInterface
{
  public:                //------------------------------- public
	PortInterface();
	virtual ~PortInterface();

	int OpenPort(const base_len *ports);
	void ClosePort();
	int OpenSerial(int no);
	void CloseSerial();
	int OpenParallel(int no);
	void CloseParallel();
	int GetFirstPort() const
	{
		return first_port;
	}
	int GetLastPort() const
	{
		return last_port;
	}
	int GetNoPorts() const
	{
		return no_ports;
	}
	int GetSerBasePort(int no);
	int GetParBasePort(int no);

	virtual int InPort(int no = -1) const;
	virtual int OutPort(int val, int no = -1);
	virtual int OutPortMask(int mask, int val);

  protected:             //------------------------------- protected
	uint8_t GetCPWReg()
	{
		return cpwreg;
	}

#ifdef  Q_OS_WIN32
	HANDLE  hCom;
#endif

	int write_port,             // Number of output port (write I/O port address)
		read_port;              // Number of input port (read I/O port address)
	uint8_t cpwreg;                 // write register image in memory (the content is the same of the hardware register)

  private:               //------------------------------- private
	int IOperm(int a, int b, int c);

	void DetectPorts();

#ifdef  Q_OS_WIN32
	//void DetectPorts9x();         // Win9x version
	void DetectLPTPortsNT();        // WinNT/2000 version
	void DetectCOMPortsNT();        // WinNT/2000 version

	int LPTCount;                   //Number of LPT ports on the system
	int COMCount;                   //Number of COM ports on the system

	DWORD   old_mask;

	HINSTANCE hInpOutDll;

	//Some global function pointers (messy but fine for an example)
	lpOut32 gfpOut32;
	lpInp32 gfpInp32;
	lpIsInpOutDriverOpen gfpIsInpOutDriverOpen;
	lpIsXP64Bit gfpIsXP64Bit;
#else
	int lcr_copy;
	int ier_copy;
#endif

	int first_port;
	int last_port;
	int no_ports;

	base_len ser_ports[MAX_COMPORTS];
	base_len par_ports[MAX_LPTPORTS];
};

#endif
