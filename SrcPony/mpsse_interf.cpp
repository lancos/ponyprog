//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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
// FTDI mpsse IO

#include "mpsse_interf.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QtCore>
#include <QDebug>
#include <QProcess>
#include <QString>

//#ifdef Q_OS_LINUX
//# include <errno.h>
//# include <unistd.h>
//# include <fcntl.h>
//#endif

MpsseInterface::MpsseInterface()
{
	qDebug() << "MpsseInterface::MpsseInterface()";

	MpsseInterface::List();
	//DeInstall();
	//old_portno = GetInstalled();
	fd_ctrl = fd_clock = fd_datain = fd_dataout = -1;
}

MpsseInterface::~MpsseInterface()
{
	Close();
}

void MpsseInterface::List()
{
	//using namespace Ftdi;

	// Parse args
	//int vid = 0x0403, pid = 0xcff8;	//0x6010;

	// Print whole list
	Ftdi::List *list = Ftdi::List::find_all(ctx, usb_vid, usb_pid);
	for (Ftdi::List::iterator it = list->begin(); it != list->end(); it++)
	{
		qDebug() << "FTDI (" << &*it << "): "
			<< QString::fromStdString(it->vendor()) << ", "
			<< QString::fromStdString(it->description()) << ", "
			<< QString::fromStdString(it->serial());

		// Open test
		if(it->open() == 0)
			qDebug() << " (Open OK)";
		else
			qDebug() << " (Open FAILED)";

		it->close();
	}
	delete list;
}

int MpsseInterface::SetPower(bool onoff)
{
	qDebug() << "MpsseInterface::SetPower(" << onoff << ")";
	return OK;
}

int MpsseInterface::InitPins()
{
	int result = 0;

	pin_ctrl = E2Profile::GetMpssePinCtrl();
	pin_datain = E2Profile::GetMpssePinDataIn();
	pin_dataout = E2Profile::GetMpssePinDataOut();
	pin_clock = E2Profile::GetMpssePinClock();

	uint8_t pins_mask = (1 << pin_ctrl) | (1 << pin_dataout) | (1 << pin_clock);

	qDebug() << "MpsseInterface::InitPins mask=" << pins_mask
			<< " Ctrl=" << pin_ctrl
			<< ", Clock=" << pin_clock
			<< ", DataIn=" << pin_datain << ", DataOut=" << pin_dataout;

	result = ctx.set_bitmode(pins_mask, BITMODE_MPSSE);
	ctx.flush(ctx.Input|ctx.Output);
	return result;
}

void MpsseInterface::DeInitPins()
{
}

int MpsseInterface::Open(int port)
{
	qDebug() << "MpsseInterface::Open(" << port << ") IN";

	int ret_val = OK;

	if (GetInstalled() != port)
	{
		int result;
		result = ctx.set_interface(INTERFACE_A);
		Q_ASSERT(result != 0);
		result = ctx.open(usb_vid, usb_pid);
		if (result == 0)
		{
			ctx.reset();
			//ctx.flush();
			ctx.set_read_chunk_size(64);
			ctx.set_write_chunk_size(64);
			//ctx.set_event_char();
			//ctx.set_error_char();
			//ctx.set_usb_read_timeout(0);
			//ctx.set_usb_write_timeout(5000);
			ctx.set_latency(1);
			ctx.set_bitmode(0, BITMODE_RESET);
			result = InitPins();
			Q_ASSERT(result != 0);
			Install(port);
		}
		else
		{
			qDebug() << ctx.error_string();
			ret_val = result;
		}
	}

	qDebug() << "MpsseInterface::Open() = " << ret_val << " OUT";

	return ret_val;
}

void MpsseInterface::Close()
{
	qDebug() << "MpsseInterface::Close() IN";

	if (IsInstalled())
	{
		SetPower(false);
		DeInitPins();
		ctx.close();
		DeInstall();
	}

	qDebug() << "MpsseInterface::Close() OUT";
}

// Per l'AVR e` la linea di RESET
void MpsseInterface::SetControlLine(int res)
{
	qDebug() << "MpsseInterface::SetControlLine(" << res << ") *** Inst=" <<  IsInstalled() << ", fd=" << fd_ctrl;

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

#ifdef Q_OS_LINUX
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
			qWarning("MpsseInterface::SetControlLine() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void MpsseInterface::SetDataOut(int sda)
{
	qDebug() << "MpsseInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled() << ", fd=" << fd_dataout;

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

#ifdef Q_OS_LINUX
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
			qWarning("MpsseInterface::SetDataOut() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void MpsseInterface::SetClock(int scl)
{
	qDebug() << "MpsseInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled() << ", fd=" << fd_clock;

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

#ifdef Q_OS_LINUX
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
			qWarning("MpsseInterface::SetClock() write failed (%d)\n", ret);
			exit(1);
		}
#endif
	}
}

void MpsseInterface::SetClockData()
{
	qDebug() << "MpsseInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetClock(1);
		SetDataOut(1);
	}
}


void MpsseInterface::ClearClockData()
{
	qDebug() << "MpsseInterface::ClearClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		SetClock(0);
		SetDataOut(0);
	}
}

int MpsseInterface::GetDataIn()
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
			qWarning("MpsseInterface::GetDataIn() read failed (%d)\n", ret);
			exit(1);
		}
#endif
		qDebug() << "MpsseInterface::GetDataIn()=" << val << ", fd=" << fd_datain;

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

int MpsseInterface::GetClock()
{
	return 1;
}

int MpsseInterface::IsClockDataUP()
{
	qDebug() << "MpsseInterface::IsClockDataUP() *** Inst=" << IsInstalled();

	return GetDataIn();
}

int MpsseInterface::IsClockDataDOWN()
{
	qDebug() << "MpsseInterface::IsClockDataDOWN() *** Inst=" << IsInstalled();

	return !GetDataIn();
}

/**
int MpsseInterface::TestPort(int com_no)
{
		qDebug() << "MpsseInterface::TestPort(%d) IN"<< com_no;
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
		qDebug() << "MpsseInterface::TestPort() = %d OUT"<< ret_val;

        return ret_val;
}
**/
