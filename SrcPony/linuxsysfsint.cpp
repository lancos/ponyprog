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
// Linux SysFS IO (useful on raspberry and embedded PC)

#include "linuxsysfsint.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QDebug>
#include <QProcess>
#include <QString>

#define GPIO_OUT                        true
#define GPIO_IN                         false

#ifdef  __linux__
// # include <stdio.h>
// # include <stdlib.h>
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

#ifdef  __linux__

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

static int gpio_open(unsigned int gpio, bool out_dir)
{
// 	char buf[MAX_BUF];
	QString buf;
	int rval;

	//trying with gpio command (you need wiringPi installed)
	buf.sprintf("export %u %s", gpio, out_dir ? "out" : "in");
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
			buf.sprintf("%s/gpio%d/direction", SYSFS_GPIO_DIR, gpio);
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

		buf.sprintf("%s/gpio%d/value", SYSFS_GPIO_DIR, gpio);
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
	buf.sprintf("unexport %u", gpio);
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

int LinuxSysFsInterface::InitPins()
{
	pin_ctrl = E2Profile::GetGpioPinCtrl();
	pin_datain = E2Profile::GetGpioPinDataIn();
	pin_dataout = E2Profile::GetGpioPinDataOut();
	pin_clock = E2Profile::GetGpioPinClock();

	qDebug() << "LinuxSysFsInterface::InitPins Ctrl=" << pin_ctrl << ", Clock= " << pin_clock;
	qDebug() << "DataIn=" << pin_datain << ", DataOut=" << pin_dataout;

#ifdef  __linux__
	fd_ctrl = gpio_open(pin_ctrl, GPIO_OUT);
	fd_clock = gpio_open(pin_clock, GPIO_OUT);
	fd_datain = gpio_open(pin_datain, GPIO_IN);
	fd_dataout = gpio_open(pin_dataout, GPIO_OUT);

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
#ifdef  __linux__
	gpio_close(pin_ctrl, fd_ctrl);
	gpio_close(pin_clock, fd_clock);
	gpio_close(pin_datain, fd_datain);
	gpio_close(pin_dataout, fd_dataout);
	fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
#endif
}

int LinuxSysFsInterface::Open(int com_no)
{
	qDebug() << "LinuxSysFsInterface::Open(" << com_no << ") IN";

	int ret_val = OK;

	if (GetInstalled() != com_no)
	{
		if ((ret_val = InitPins()) == OK)
		{
			Install(com_no);
		}
	}

	qDebug() << "LinuxSysFsInterface::Open() = " << ret_val << " OUT";

	return ret_val;
}

void LinuxSysFsInterface::Close()
{
	qDebug() << "LinuxSysFsInterface::Close() IN";

	if (IsInstalled())
	{
		SetPower(false);
		DeInitPins();
		DeInstall();
	}

	qDebug() << "LinuxSysFsInterface::Close() OUT";
}

// Per l'AVR e` la linea di RESET
void LinuxSysFsInterface::SetControlLine(int res)
{
	qDebug() << "LinuxSysFsInterface::SetControlLine(" << res << ") *** Inst=" <<  IsInstalled() << ", fd=" << fd_ctrl;

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

#ifdef  __linux__
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
	qDebug() << "LinuxSysFsInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled() << ", fd=" << fd_dataout;

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

#ifdef  __linux__
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
	qDebug() << "LinuxSysFsInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled() << ", fd=" << fd_clock;

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

#ifdef  __linux__
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
	qDebug() << "LinuxSysFsInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetClock(1);
		SetDataOut(1);
	}
}


void LinuxSysFsInterface::ClearClockData()
{
	qDebug() << "LinuxSysFsInterface::ClearClockData() *** Inst=" << IsInstalled();

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
#ifdef  __linux__
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

#endif
		qDebug() << "LinuxSysFsInterface::GetDataIn()=" << val << ", fd=" << fd_datain;

		if (cmdWin->GetPolarity() & DININV)
		{
			return !val;
		}
		else
		{
			return val;
		}
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
	qDebug() << "LinuxSysFsInterface::IsClockDataUP() *** Inst=" << IsInstalled();

	return GetDataIn();
}

int LinuxSysFsInterface::IsClockDataDOWN()
{
	qDebug() << "LinuxSysFsInterface::IsClockDataDOWN() *** Inst=" << IsInstalled();

	return !GetDataIn();
}

/**
int LinuxSysFsInterface::TestPort(int com_no)
{
        qDebug() << "LinuxSysFsInterface::TestPort(%d) IN"<< com_no;
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
        qDebug() << "LinuxSysFsInterface::TestPort() = %d OUT"<< ret_val;

        return ret_val;
}
**/
