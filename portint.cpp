//=========================================================================//
//-------------------------------------------------------------------------//
// portint.cpp -- Source for PortInterface class                           //
// This file is part of PonyProg.                                          //
//-------------------------------------------------------------------------//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997, 1998  Claudio Lanconelli                           //
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

#ifdef	_LINUX_
#include <asm/io.h>
#include <sys/io.h>
#include <unistd.h>

	int PortInterface::IOperm(int a, int b, int c)
	{
		int retval;
		UserDebug3(UserApp3, "PortInterface::IOPerm(%x, %x, %x)\n", a, b, c);
		retval = ioperm(a, b, c);
		UserDebug1(UserApp3, "PortInterface::IOPerm() %d\n", retval);
		return retval;
	}
#else
	int PortInterface::IOperm(int a, int b, int c)
	{
		c = a + b;
		return 0;
	}
#endif

#ifdef	_WINDOWS
#ifdef __GNUC__
# define	inb(id)		0
# define	outb(val, id)
#else
# ifdef	__BORLANDC__
#    include <conio.h>
#    define	inb(id)			inp(id)
#    define	outb(val, id)	outp(id, val)
#   define	__inline__
# else // MICROSOFT VC++
#    include <conio.h>
#    define	inb(id)			_inp(id)
#    define	outb(val, id)	_outp(id, val)
#   define	__inline__ __inline
#   define _export
# endif
#endif
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
	hCom = INVALID_HANDLE_VALUE;
#else
	lcr_copy = ier_copy = -1;
#endif

	cpwreg = read_port = write_port = 0;
	first_port = last_port = no_ports = 0;

	UserDebug(Constructor, "PortInterface::PortInterface() O\n");
}

PortInterface::~PortInterface()
{
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

	return inb(nport);
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
	outb(val, nport);
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

	outb(cpwreg, write_port);

	return OK;
}

int PortInterface::OpenPort(int from_port, int len)
{
	UserDebug2(UserApp3, "PortInterface::OpenPort(%xh, %d) I\n", from_port, len);

	int ret_val = E2ERR_OPENFAILED;

	// le porte ISA del PC sono di soli 10 bit di indirizzi
	// controlla che le porte siano accessibili
	if (from_port >= 0x100 && from_port + len <= 0x400 && len > 0)
	{
		// richiede il permesso di accedere alle porte di I/O
		if ( IOperm(from_port, len, 1) == 0  )
		{
			ClosePort();	// chiude eventuali porte precedenti
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
#endif
		if (ret_val == OK)
		{
			ret_val = OpenPort(ser_ports[no-1], 8);

			if (ret_val == OK)
			{
#ifndef	_WINDOWS
				lcr_copy = inb(GetFirstPort() + lcrOfst);
				outb(0, GetFirstPort() + lcrOfst);
				ier_copy = inb(GetFirstPort() + ierOfst);
				outb(0, GetFirstPort() + ierOfst);
#endif
				write_port = GetFirstPort() + mcrOfst;
				read_port = GetFirstPort() + msrOfst;
				cpwreg = 0;
			}
		}
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
#endif
		{
			ret_val = OpenPort(par_ports[no-1], 8);
			if (ret_val == OK)
			{
				write_port = GetFirstPort() + dataOfst;
				read_port = GetFirstPort() + statOfst;
				cpwreg = 0;
			}
		}
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
