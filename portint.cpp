//=========================================================================//
//-------------------------------------------------------------------------//
// portint.cpp -- Source for PortInterface class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2000   Claudio Lanconelli                           //
//                                                                         //
//  e-mail: lanconel@cs.unibo.it                                           //
//  http://www.cs.unibo.it/~lanconel                                       //
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

#include <stdio.h>
#include "portint.h"
#include "errcode.h"

//#include "e2app.h"
#include "dlportio.h"

#ifdef	_LINUX_
#include <asm/io.h>
#include <sys/io.h>
//May be your system need this instead of <sys/io.h>
//#include <linux/ioport.h>
#include <unistd.h>

	//Request Access to I/O ports
	int PortInterface::IOperm(int a, int b, int c)
	{
		int retval;
		UserDebug3(UserApp3, "PortInterface::IOPerm(%x, %x, %x)\n", a, b, c);
		retval = ioperm(a, b, c);
		UserDebug1(UserApp3, "PortInterface::IOPerm() %d\n", retval);
		return retval;
	}
#endif

#ifdef	_WINDOWS
#ifdef __GNUC__
//# define	inb(id)		0
//# define	outb(val, id)
#else
# ifdef	__BORLANDC__
#   include <conio.h>
#   define	inb(id)			inp(id)
#   define	outb(val, id)	outp(id, val)
#   define	__inline__
# else // MICROSOFT VC++
#   include <conio.h>
#   define	inb(id)			_inp(id)
#   define	outb(val, id)	_outp(id, val)
#   define	__inline__ __inline
#   define _export
# endif
#endif
#endif

#ifdef	_WINDOWS
HIDDEN int GetWinVersion()
{ 
	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&osvi);

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)
		return 1;
	else
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return 2;
	else
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return 3;
	else
		return 4;	//WIN2000 ???
}

int PortInterface::IOperm(int a, int b, int c)
{
	int rv = OK;

	a += b;
/**
	rv = GetWinVersion();

	if (c)	//Open
	{
		if (rv == 1 || rv == 2)
		{
			rv = OK;
		}
		else
		{
			hPort = CreateFile(
			//	"\\\\.\\DirectIo0",                 // pointer to name of the file
				THEAPP->GetIODriverName(),
				GENERIC_READ | GENERIC_WRITE,       // access (read-write) mode
				FILE_SHARE_READ | FILE_SHARE_WRITE, // share mode
				NULL,                               // pointer to security attributes
				OPEN_EXISTING,                      // how to create
				0,                                  // file attributes
				NULL);                              // handle to file with attributes to copy

			if (hPort == INVALID_HANDLE_VALUE)    // did we succeed?
			{
				// possibly call GetLastError() to get a hint what failed
				rv = E2ERR_OPENFAILED;
			}
			else
				rv = OK;
		}
	}
	else
	{
		rv = OK;
	}

	//	if (hPort != INVALID_HANDLE_VALUE)
	//		CloseHandle(hPort);
	//	hPort = INVALID_HANDLE_VALUE;
**/
	return rv;
}
#endif

enum LptRegs {
	dataOfst,
	statOfst,
	ctrlOfst
};

enum UartRegs {
	thrOfst,
	ierOfst,
	iirOfst,
	lcrOfst,
	mcrOfst,
	lsrOfst,
	msrOfst
};


PortInterface::PortInterface()
{
	UserDebug(Constructor, "PortInterface::PortInterface() Constructor\n");

	ser_ports[0] = 0x3F8;
	ser_ports[1] = 0x2F8;
	ser_ports[2] = 0x3E8;
	ser_ports[3] = 0x2E8;
	par_ports[0] = 0x378;
	par_ports[1] = 0x278;
	par_ports[2] = 0x3BC;

#ifdef	_WINDOWS
	hCom = hPort = INVALID_HANDLE_VALUE;
#else
	lcr_copy = ier_copy = -1;
#endif

	cpwreg = read_port = write_port = 0;
	first_port = last_port = no_ports = 0;

	UserDebug(Constructor, "PortInterface::PortInterface() O\n");
}

PortInterface::~PortInterface()
{
#ifdef	_WINDOWS
	if (hPort != INVALID_HANDLE_VALUE)
		CloseHandle(hPort);
	hPort = INVALID_HANDLE_VALUE;
#endif
	UserDebug(Destructor, "PortInterface::~PortInterface() Destructor\n");
}

int PortInterface::InPort(int nport) const
{
	if (first_port == 0)
		return -1;

	if (nport < 0 || nport >= no_ports)		//use default read port
		nport = read_port;
	else
		nport += first_port;

#ifdef	_WINDOWS
	return DlPortReadPortUchar(nport);
#else
	return inb(nport);
#endif
}

void PortInterface::OutPort(int val, int nport)
{
	UserDebug2(UserApp3, "PortInterface::OutPort() ** %xh, %xh\n", first_port, last_port);

	if (first_port == 0)
		return;

	if (nport < 0 || nport >= no_ports)
		nport = write_port;
	else
		nport += first_port;

	if (nport == write_port)
		cpwreg = val;

	UserDebug2(UserApp3, "PortInterface::outb(%xh, %xh)\n", val, nport);

#ifdef	_WINDOWS
	DlPortWritePortUchar(nport, val);
#else
	outb(val, nport);
#endif
}

int PortInterface::OutPortMask(int mask, int val)
{
	UserDebug2(UserApp3, "PortInterface::OutPortMask(%d, %d)\n", mask, val);

	if (write_port == 0)
		return -1;

	if (val == 0)	//clear the bit 1's in the mask (other bits unchanged)
	{
		cpwreg &= ~mask;
	}
	else
	if (val == 1)	//set the bit 1's in the mask (other bits unchanged)
	{
		cpwreg |= mask;
	}
	else	//toggle the bit 1's in the mask (other bits unchanged)
	{
		cpwreg ^= mask;
	}

	UserDebug2(UserApp3, "PortInterface::outb(%xh, %xh)\n", cpwreg, write_port);

#ifdef	_WINDOWS
	DlPortWritePortUchar(write_port, cpwreg);
#else
	outb(cpwreg, write_port);
#endif

	return OK;
}

int PortInterface::OpenPort(int from_port, int len)
{
	UserDebug2(UserApp3, "PortInterface::OpenPort(%xh, %d) I\n", from_port, len);

	int ret_val = E2ERR_OPENFAILED;

	// PC ISA ports have only 10 bit addresses
	// check for the access to ports is granted
	if (from_port >= 0x100 && from_port + len <= 0x400 && len > 0)
	{
		// request the access to I/O ports
		if ( IOperm(from_port, len, 1) == 0  )
		{
			ClosePort();	// close any opened port
			first_port = from_port;
			last_port = from_port + len - 1;
			no_ports = len;

			ret_val = OK;
		}
		else
		{
			first_port = last_port = no_ports = 0;
			ret_val = E2ERR_ACCESSDENIED;
		}
	}

	UserDebug1(UserApp3, "PortInterface::OpenPort() = %d O\n", ret_val);

	return ret_val;
}

void PortInterface::ClosePort()
{
	UserDebug(UserApp3, "PortInterface::ClosePort() I\n");

	if (first_port && last_port && no_ports)
	{
		IOperm(first_port, no_ports, 0);
	}
	first_port = last_port = no_ports = 0;

	UserDebug(UserApp3, "PortInterface::ClosePort() O\n");
}

int PortInterface::OpenSerial(int no)
{
	UserDebug1(UserApp2, "PortInterface::OpenSerial(%d) I\n", no);

	int ret_val = E2ERR_OPENFAILED;

	if (no >= 1 && no <= MAX_COMPORTS)
	{
		ret_val = OK;

#ifdef	_WINDOWS
		//Windows95/98 WindowsNT Windows2000
		char str[8];

		sprintf(str, "COM%d", no);
		hCom = CreateFile(str,
			GENERIC_READ | GENERIC_WRITE,
			0,		/* comm devices must be opened w/exclusive-access */
			NULL,	/* no security attrs */
			OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
			0,		/* not overlapped I/O */
			NULL	/* hTemplate must be NULL for comm devices */
		);

		if ( hCom != INVALID_HANDLE_VALUE )
		{
			GetCommMask(hCom, &old_mask);
			SetCommMask(hCom, 0);
		}
		else
			ret_val = E2ERR_OPENFAILED;

		if (ret_val == OK)
			ret_val = OpenPort(ser_ports[no-1], 8);

		if (ret_val == OK)
		{
			write_port = GetFirstPort() + mcrOfst;
			read_port = GetFirstPort() + msrOfst;
			cpwreg = 0;
		}
		else
		{
			CloseSerial();
		}
#else	//Linux
		if (ret_val == OK)
			ret_val = OpenPort(ser_ports[no-1], 8);

		if (ret_val == OK)
		{
			lcr_copy = inb(GetFirstPort() + lcrOfst);
			outb(0, GetFirstPort() + lcrOfst);
			ier_copy = inb(GetFirstPort() + ierOfst);
			outb(0, GetFirstPort() + ierOfst);

			write_port = GetFirstPort() + mcrOfst;
			read_port = GetFirstPort() + msrOfst;
			cpwreg = 0;
		}
#endif
	}

	UserDebug1(UserApp2, "PortInterface::OpenSerial() = %d O\n", ret_val);

	return ret_val;
}

void PortInterface::CloseSerial()
{
#ifdef	_WINDOWS
	if ( hCom != INVALID_HANDLE_VALUE )
	{
		SetCommMask(hCom, old_mask);
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
#else
#define	BREAK_MASK	0x40

	if (lcr_copy != -1 && ier_copy != -1)
	{
		outb(lcr_copy&~BREAK_MASK, GetFirstPort() + lcrOfst);
		outb(ier_copy, GetFirstPort() + ierOfst);
		lcr_copy = ier_copy = -1;
	}
#endif
	read_port = write_port = 0;

	ClosePort();
}

int PortInterface::OpenParallel(int no)
{
	int ret_val = E2ERR_OPENFAILED;

	if (no >= 1 && no <= MAX_LPTPORTS)
	{
#ifdef	_WINDOWS
		char str[8];

		sprintf(str, "LPT%d", no);
		hCom = CreateFile(str,
			GENERIC_READ | GENERIC_WRITE,
			0,		/* comm devices must be opened w/exclusive-access */
			NULL,	/* no security attrs */
			OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
			0,		/* not overlapped I/O */
			NULL	/* hTemplate must be NULL for comm devices */
		);

		if ( hCom != INVALID_HANDLE_VALUE )
		{
			ret_val = OpenPort(par_ports[no-1], 8);
			if (ret_val == OK)
			{
				write_port = GetFirstPort() + dataOfst;
				read_port = GetFirstPort() + statOfst;
				cpwreg = 0;
			}
		}
#else
		ret_val = OpenPort(par_ports[no-1], 8);
		if (ret_val == OK)
		{
			write_port = GetFirstPort() + dataOfst;
			read_port = GetFirstPort() + statOfst;
			cpwreg = 0;
		}
#endif
	}

	return ret_val;
}

void PortInterface::CloseParallel()
{
#ifdef	_WINDOWS
	if ( hCom != INVALID_HANDLE_VALUE )
	{
		CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	}
#endif

	read_port = write_port = 0;

	ClosePort();
}


int PortInterface::GetSerBasePort(int no) const
{
	if (no >= 1 && no <= MAX_COMPORTS)
	{
		return ser_ports[no-1];
	}
	else
		return 0;
}

int PortInterface::GetParBasePort(int no) const
{
	if (no >= 1 && no <= MAX_LPTPORTS)
	{
		return par_ports[no-1];
	}
	else
		return 0;
}
