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
// Linux SysFS IO (useful on raspberry and embedded PC)

#include "linuxsysfsint.h"
#include "errcode.h"
#include "e2app.h"

#define GPIO_PIN(port, pin)	((port)*32+(pin))
#define GPIO_CTRL			(GPIO_PIN(0,5))
#define GPIO_DATAIN			(GPIO_PIN(1,17))
#define GPIO_DATAOUT		(GPIO_PIN(1,19))
#define GPIO_CLOCK			(GPIO_PIN(1,18))
#define GPIO_OUT			(1)
#define GPIO_IN				(0)

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#ifdef	_LINUX_
# include <stdio.h>
# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
#else
# ifdef	__BORLANDC__
#   define	__inline__
# else // MICROSOFT VC++
#   define	__inline__ __inline
#   define _export
# endif
#endif

LinuxSysFsInterface::LinuxSysFsInterface()
{
	UserDebug(Constructor, "LinuxSysFsInterface::LinuxSysFsInterface() Constructor\n");

	Install(0);
	old_portno = 0;
}

LinuxSysFsInterface::~LinuxSysFsInterface()
{
	Close();
}

#ifdef	_LINUX_
/****************************************************************
 * gpio_export
 ****************************************************************/
static int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
	int rval;

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	rval = write(fd, buf, len);
	close(fd);

	return rval == len ? 0 : -1;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
static int gpio_unexport(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
	int rval;

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	rval = write(fd, buf, len);
	close(fd);
	return rval == len ? 0 : -1;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
static int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
	int fd, len;
	char buf[MAX_BUF];
	int rval;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
	len = len;

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}

	if (out_flag)
		rval = write(fd, "out", 4);
	else
		rval = write(fd, "in", 3);

	close(fd);
	return (rval == 3 || rval == 4) ? 0 : -1;
}

static int gpio_set_value(unsigned int gpio, unsigned int value)
{
	int fd, len;
	char buf[MAX_BUF];
	int rval;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	len = len;

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}

	if (value)
		rval = write(fd, "1", 2);
	else
		rval = write(fd, "0", 2);

	close(fd);
	return rval == 2 ? 0 : -1;
}

/****************************************************************
 * gpio_get_value
 ****************************************************************/
static int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd, len;
	char buf[MAX_BUF];
	char ch;
	int rval;

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	len = len;

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		return fd;
	}

	rval = read(fd, &ch, 1);

	if (ch != '0') {
		*value = 1;
	} else {
		*value = 0;
	}

	close(fd);
	return rval == 1 ? 0 : -1;
}
#endif

// Per l'AVR e` la linea di RESET
void LinuxSysFsInterface::SetControlLine(int res)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetControlLine(%d) *** Inst=%d\n", res, IsInstalled());

	if (IsInstalled())
	{
		UserDebug(UserApp3, "LinuxSysFsInterface::SetControlLine() XX\n");

		if (THEAPP->GetPolarity() & RESETINV)
			res = !res;

		UserDebug(UserApp3, "LinuxSysFsInterface::SetControlLine() \n");

#ifdef	_LINUX_
		if (res)
			gpio_set_value(GPIO_CTRL, 1);
		else
			gpio_set_value(GPIO_CTRL, 0);
#endif
	}
}

int LinuxSysFsInterface::SetPower(int onoff)
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::SetPower(%d)\n", onoff);
	return OK;
}

int LinuxSysFsInterface::InitPins()
{
#ifdef	_LINUX_
	if (gpio_export(GPIO_CTRL) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_set_dir(GPIO_CTRL, GPIO_OUT) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_export(GPIO_CLOCK) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_set_dir(GPIO_CLOCK, GPIO_OUT) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_export(GPIO_DATAIN) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_set_dir(GPIO_DATAIN, GPIO_IN) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_export(GPIO_DATAOUT) < 0)
		return E2ERR_OPENFAILED;

	if (gpio_set_dir(GPIO_DATAOUT, GPIO_OUT) < 0)
		return E2ERR_OPENFAILED;
#endif
	return OK;
}

void LinuxSysFsInterface::DeInitPins()
{
#ifdef	_LINUX_
	gpio_unexport(GPIO_CTRL);
	gpio_unexport(GPIO_CLOCK);
	gpio_unexport(GPIO_DATAIN);
	gpio_unexport(GPIO_DATAOUT);
#endif
}

int LinuxSysFsInterface::Open(int com_no)
{
	UserDebug1(UserApp1, "LinuxSysFsInterface::Open(%d) IN\n", com_no);

	int ret_val = OK;

	if (IsInstalled() != com_no)
	{
		if ((ret_val = InitPins()) == OK)
			Install(com_no);
	}

	UserDebug1(UserApp2, "LinuxSysFsInterface::Open() = %d OUT\n", ret_val);

	return ret_val;
}

void LinuxSysFsInterface::Close()
{
	UserDebug(UserApp1, "LinuxSysFsInterface::Close() IN\n");

	if (IsInstalled())
	{
		SetPower(0);
		DeInitPins();
		Install(0);
	}

	UserDebug(UserApp2, "LinuxSysFsInterface::Close() OUT\n");
}

void LinuxSysFsInterface::SetDataOut(int sda)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetDataOut(%d) *** Inst=%d\n", sda, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & DOUTINV) )
			sda = !sda;

#ifdef	_LINUX_
		if (sda)
			gpio_set_value(GPIO_DATAOUT, 1);
		else
			gpio_set_value(GPIO_DATAOUT, 0);
#endif
	}
}

void LinuxSysFsInterface::SetClock(int scl)
{
	UserDebug2(UserApp3, "LinuxSysFsInterface::SetClock(%d) *** Inst=%d\n", scl, IsInstalled());

	if (IsInstalled())
	{
		if ( (THEAPP->GetPolarity() & CLOCKINV) )
			scl = !scl;

#ifdef	_LINUX_
		if (scl)
			gpio_set_value(GPIO_CLOCK, 1);
		else
			gpio_set_value(GPIO_CLOCK, 0);
#endif
	}
}

void LinuxSysFsInterface::SetClockData()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::SetClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		SetClock(1);
		SetDataOut(1);
	}
}


void LinuxSysFsInterface::ClearClockData()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::ClearClockData() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		SetClock(0);
		SetDataOut(0);
	}
}

int LinuxSysFsInterface::GetDataIn()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::GetDataIn() *** Inst=%d\n", IsInstalled());

	if (IsInstalled())
	{
		unsigned int val = 0;
		gpio_get_value(GPIO_DATAIN, &val);

		if (THEAPP->GetPolarity() & DININV)
			return !val;
		else
			return val;
	}
	else
		return E2ERR_NOTINSTALLED;
}

int LinuxSysFsInterface::GetClock()
{
	return 1;
}

int LinuxSysFsInterface::IsClockDataUP()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::IsClockDataUP() *** Inst=%d\n", IsInstalled());

	return GetDataIn();
}

int LinuxSysFsInterface::IsClockDataDOWN()
{
	UserDebug1(UserApp3, "LinuxSysFsInterface::IsClockDataDOWN() *** Inst=%d\n", IsInstalled());

	return !GetDataIn();
}

/**
int LinuxSysFsInterface::TestPort(int com_no)
{
	UserDebug1(UserApp1, "LinuxSysFsInterface::TestPort(%d) IN\n", com_no);
	int ret_val = TestSave(com_no);

	if (ret_val == OK)
	{
		int a,b;
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
					ret_val = OK;
			}
		}
	}
	TestRestore();
	UserDebug1(UserApp2, "LinuxSysFsInterface::TestPort() = %d OUT\n", ret_val);

	return ret_val;
}
**/