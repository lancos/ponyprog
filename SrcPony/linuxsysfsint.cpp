//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
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
// Linux SysFS IO (useful on raspberry and embedded PC)

#include "linuxsysfsint.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QtCore>
#include <QDebug>
#include <QProcess>
#include <QString>

#define GPIO_OUT                        true
#define GPIO_IN                         false

#ifdef Q_OS_LINUX
# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
#endif

LinuxSysFsInterface::LinuxSysFsInterface()
{
	//qDebug() << "LinuxSysFsInterface::LinuxSysFsInterface()";

	//DeInstall();
	//old_portno = GetInstalled();
	fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
}

LinuxSysFsInterface::~LinuxSysFsInterface()
{
	Close();
}

#ifdef Q_OS_LINUX

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

static int gpio_open(unsigned int gpio, bool out_dir)
{
// 	char buf[MAX_BUF];
	QString buf;
	int rval;

	//trying with gpio command (you need wiringPi installed)
#if USE_QT_VERSION == 4
	buf.sprintf("export %u %s", gpio, out_dir ? "out" : "in");
#else
	buf.asprintf("export %u %s", gpio, out_dir ? "out" : "in");
#endif
// 	rval = system(buf.toLatin1().data());
	rval = QProcess::execute("gpio", buf.split(" "));

	if (rval != 0)
	{
		int fd;

		fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);

		if (fd < 0)
		{
			qWarning("Unable to open GPIO export interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			int ret, len;

			buf = QString::number(gpio);
			len = buf.length();
			ret = write(fd, buf.toLatin1().data(), len);
			close(fd);
			rval = (ret == len) ? 0 : -1;
		}

		if (rval == 0)
		{
#if USE_QT_VERSION == 4
			buf.sprintf("%s/gpio%d/direction", SYSFS_GPIO_DIR, gpio);
#else
			buf.asprintf("%s/gpio%d/direction", SYSFS_GPIO_DIR, gpio);
#endif
			fd = open(buf.toLatin1().data(), O_WRONLY);

			if (fd < 0)
			{
				qWarning("Unable to open GPIO direction interface: %s\n", strerror(errno));
				rval = -1;
			}
			else
			{
				int ret, len;

				if (out_dir)
				{
					buf = "out";
				}
				else
				{
					buf = "in";
				}

				len = buf.length();
				ret = write(fd, buf.toLatin1().constData(), len);
				close(fd);
				rval = (ret == len) ? 0 : -1;
			}
		}
	}

	//open the value interace
	if (rval == 0)
	{
		int fd;
#if USE_QT_VERSION == 4
		buf.sprintf("%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);
#else
		buf.asprintf("%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);
#endif
		fd = open(buf.toLatin1().data(), out_dir ? O_WRONLY : O_RDONLY);

		if (fd < 0)
		{
			qWarning("Unable to open GPIO set-value interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			rval = fd;
		}
	}

	qDebug() << "gpio_open(" << gpio << ", " << (out_dir ? "out" : "in") << ") rval = " << rval;

	return rval;
}

static int gpio_close(unsigned int gpio, int fd)
{
// 	char buf[MAX_BUF];
	QString buf;
	int rval = 0;

	//close value interface
	if (fd > 0)
	{
		close(fd);
	}

	//trying with gpio command (you need wiringPi installed)
#if USE_QT_VERSION == 4
	buf.sprintf("unexport %u", gpio);
#else
	buf.asprintf("unexport %u", gpio);
#endif
// 	rval = system(buf.toLatin1().data());
	rval = QProcess::execute("gpio", buf.split(" "));

	if (rval != 0)
	{
		fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);

		if (fd < 0)
		{
			qWarning("Unable to open GPIO unexport interface: %s\n", strerror(errno));
			rval = -1;
		}
		else
		{
			int ret, len;

			buf = QString().number(gpio);
			len = buf.length();
			ret = write(fd, buf.toLatin1().data(), len);
			close(fd);
			rval = (ret == len) ? 0 : -1;
		}
	}

	qDebug() << "gpio_close(" << gpio << ") rval = " << rval;

	return rval;
}
#endif

int LinuxSysFsInterface::SetPower(bool onoff)
{
	qDebug() << "LinuxSysFsInterface::SetPower(" << onoff << ")";
	return OK;
}

int LinuxSysFsInterface::InitPins(int idx)
{
	if (pins.ctrl == 0 && pins.clock == 0 && pins.datain == 0 && pins.dataout == 0)
	{
		qWarning() << "LinuxSysFsInterface::InitPins() Unconfigured pins";
		return E2ERR_OPENFAILED;
	}

	qDebug() << "LinuxSysFsInterface::InitPins Ctrl=" << pins.ctrl << ", Clock= " << pins.clock;
	qDebug() << "DataIn=" << pins.datain << ", DataOut=" << pins.dataout;

#ifdef Q_OS_LINUX
	fd_ctrl = gpio_open(pins.ctrl, GPIO_OUT);
	fd_clock = gpio_open(pins.clock, GPIO_OUT);
	fd_datain = gpio_open(pins.datain, GPIO_IN);
	fd_dataout = gpio_open(pins.dataout, GPIO_OUT);

	if (fd_ctrl < 0 || fd_clock < 0 || fd_datain < 0 || fd_dataout < 0)
	{
		DeInitPins();
		return E2ERR_OPENFAILED;
	}

#endif
	return OK;
}

void LinuxSysFsInterface::DeInitPins()
{
#ifdef Q_OS_LINUX
	gpio_close(pins.ctrl, fd_ctrl);
	gpio_close(pins.clock, fd_clock);
	gpio_close(pins.datain, fd_datain);
	gpio_close(pins.dataout, fd_dataout);
	fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
#endif
}

int LinuxSysFsInterface::Open(int port_no)
{
	qDebug() << Q_FUNC_INFO << " (" << port_no << ") IN";

	int ret_val = OK;

	if (GetInstalled() != port_no)
	{
		if ((ret_val = InitPins(port_no)) == OK)
		{
			Install(port_no);
		}
	}

	qDebug() << Q_FUNC_INFO << " = " << ret_val << " OUT";

	return ret_val;
}

void LinuxSysFsInterface::Close()
{
	qDebug() << Q_FUNC_INFO << " IN";

	if (IsInstalled())
	{
		SetPower(false);
		DeInitPins();
		DeInstall();
	}

	qDebug() << Q_FUNC_INFO << " OUT";
}

// Per l'AVR e` la linea di RESET
void LinuxSysFsInterface::SetControlLine(int res)
{
	qDebug() << Q_FUNC_INFO << " (" << res << ") *** Inst=" <<  IsInstalled() << ", fd=" << fd_ctrl;

	if (IsInstalled())
	{
#ifdef Q_OS_LINUX
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		int ret;

		if (res)
		{
			ret = write(fd_ctrl, "1", 2);
		}
		else
		{
			ret = write(fd_ctrl, "0", 2);
		}

		if (ret != 2)
		{
			qWarning("LinuxSysFsInterface::SetControlLine() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void LinuxSysFsInterface::SetDataOut(int sda)
{
	qDebug() << Q_FUNC_INFO << "(" << sda << ") *** Inst=" << IsInstalled() << ", fd=" << fd_dataout;

	if (IsInstalled())
	{
#ifdef Q_OS_LINUX
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		int ret;

		if (sda)
		{
			ret = write(fd_dataout, "1", 2);
		}
		else
		{
			ret = write(fd_dataout, "0", 2);
		}

		if (ret != 2)
		{
			qWarning("LinuxSysFsInterface::SetDataOut() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void LinuxSysFsInterface::SetClock(int scl)
{
	qDebug() << Q_FUNC_INFO << "(" << scl << ") *** Inst=" << IsInstalled() << ", fd=" << fd_clock;

	if (IsInstalled())
	{
#ifdef Q_OS_LINUX
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		int ret;

		if (scl)
		{
			ret = write(fd_clock, "1", 2);
		}
		else
		{
			ret = write(fd_clock, "0", 2);
		}

		if (ret != 2)
		{
			qWarning("LinuxSysFsInterface::SetClock() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void LinuxSysFsInterface::SetClockData()
{
	qDebug() << Q_FUNC_INFO << " *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetClock(1);
		SetDataOut(1);
	}
}

void LinuxSysFsInterface::ClearClockData()
{
	qDebug() << Q_FUNC_INFO << " *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetClock(0);
		SetDataOut(0);
	}
}

int LinuxSysFsInterface::GetDataIn()
{
	if (IsInstalled())
	{
		unsigned int val = 0;
#ifdef Q_OS_LINUX
		int ret;
		char ch;

		lseek(fd_datain, 0L, SEEK_SET);
		ret = read(fd_datain, &ch, 1);
		val = (ch == '0') ? 0 : 1;

		if (ret < 1)
		{
			qWarning("LinuxSysFsInterface::GetDataIn() read failed (%d)\n", ret);
			exit(1);
		}
		qDebug() << Q_FUNC_INFO << "=" << val << ", fd=" << fd_datain;

		if (cmdWin->GetPolarity() & DININV)
		{
			val = !val;
		}
#endif
		return val;
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int LinuxSysFsInterface::GetClock()
{
	return 1;
}

int LinuxSysFsInterface::IsClockDataUP()
{
	qDebug() << Q_FUNC_INFO << " *** Inst=" << IsInstalled();

	return GetDataIn();
}

int LinuxSysFsInterface::IsClockDataDOWN()
{
	qDebug() << Q_FUNC_INFO << " *** Inst=" << IsInstalled();

	return !GetDataIn();
}
