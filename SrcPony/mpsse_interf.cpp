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

MpsseInterface::MpsseInterface()
	: pin_directions(0),
	  last_data(0),
	  read_data(0),
	  pin_ctrl(0),
	  pin_datain(0),
	  pin_dataout(0),
	  pin_clock(0),
	  pin_clockin(0),
	  pin_poweron(0)
{
	qDebug() << __PRETTY_FUNCTION__;

	//MpsseInterface::List();
	//DeInstall();
	//old_portno = GetInstalled();

	cmdbuf.clear();
}

MpsseInterface::~MpsseInterface()
{
	Close();
}

void MpsseInterface::List()
{
	//using namespace Ftdi;

	qDebug() << __PRETTY_FUNCTION__;

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
		if (it->open() == 0)
		{
			qDebug() << " (Open OK)";
		}
		else
		{
			qDebug() << " (Open FAILED)";
		}

		it->close();
	}
	delete list;
}

void MpsseInterface::ConfigPins(int pinum_ctrl, int pinum_datain, int pinum_dataout, int pinum_clock, int pinum_clockin, int pinum_poweron)
{
	if (pinum_ctrl < 0)
	{
		pin_ctrl = 0;
	}
	else
	{
		pin_ctrl = 1 << pinum_ctrl;
	}

	if (pinum_datain < 0)
	{
		pin_datain = 0;
	}
	else
	{
		pin_datain = 1 << pinum_datain;
	}

	if (pinum_dataout < 0)
	{
		pin_dataout = 0;
	}
	else
	{
		pin_dataout = 1 << pinum_dataout;
	}

	if (pinum_clock < 0)
	{
		pin_clock = 0;
	}
	else
	{
		pin_clock = 1 << pinum_clock;
	}

	if (pinum_clockin < 0)
	{
		pin_clockin = 0;
	}
	else
	{
		pin_clockin = 1 << pinum_clockin;
	}

	if (pinum_poweron < 0)
	{
		pin_poweron = 0;
	}
	else
	{
		pin_poweron = 1 << pinum_poweron;
	}
}

int MpsseInterface::InitPins()
{
	int result;

	result = ctx.set_bitmode(0, BITMODE_RESET);
	if (result == 0)
	{
		last_data = 0;
		pin_directions = 0;

		if (pin_ctrl == 0 && pin_clock == 0 && pin_datain == 0 && pin_dataout == 0)
		{
			ConfigPins(E2Profile::GetMpssePinCtrl(), E2Profile::GetMpssePinDataIn(), E2Profile::GetMpssePinDataOut(), E2Profile::GetMpssePinClock());
		}

		qDebug() << __PRETTY_FUNCTION__ << (hex)
				 << " Ctrl=" << pin_ctrl
				 << ", Clock=" << pin_clock << ", ClockIn=" << pin_clockin
				 << ", DataIn=" << pin_datain << ", DataOut=" << pin_dataout
				 << ", Poweron=" << pin_poweron;

		result = ctx.set_bitmode(0, BITMODE_MPSSE);
		if (result == 0)
		{
			ctx.flush();
			cmdbuf.clear();

			if (GetI2CMode())
			{
				cmdbuf.append(EN_3_PHASE);
				last_data = pin_dataout | pin_clock;	//I2C need pins high
			}
			else
			{
				cmdbuf.append(DIS_3_PHASE);
				last_data = 0;
			}

			//00011011 --> 0x1B
			int new_data = 0;
			int new_directions = pin_ctrl | pin_dataout | pin_clock | pin_poweron;
			new_directions |= (1 << 4) | (1 << 11);

			//Force update
			last_data = ~new_data & 0xffff;
			pin_directions = ~new_directions & 0xffff;
			SendPins(new_data, new_directions);	//set pins to ZERO
			result = Flush();
		}
	}

	return result;
}

void MpsseInterface::DeInitPins()
{
	//ctx.set_bitmode(0, BITMODE_RESET);
}

int MpsseInterface::SetFrequency(uint32_t freq)
{
	int32_t divisor, sysclock;

	Q_ASSERT(freq > 0);

	if (freq > 6000000)
	{
		sysclock = 60000000;
		cmdbuf.append(DIS_DIV_5);
	}
	else
	{
		sysclock = 12000000;
		cmdbuf.append(EN_DIV_5);
	}

	divisor = (sysclock / (2 * freq)) - 1;
	if (divisor < 0)
	{
		qDebug() << __PRETTY_FUNCTION__ << "Frequency high" << freq;
		divisor = 0;
	}

	if (divisor > 65535)
	{
		qDebug() << __PRETTY_FUNCTION__ << "Frequency low" << freq;
		divisor = 65535;
	}

	qDebug() << __PRETTY_FUNCTION__ << "Frequency" << (sysclock / (2 * (divisor + 1))) << ", Divisor" << divisor << ", SysClock" << sysclock;

	cmdbuf.append(TCK_DIVISOR);
	cmdbuf.append(divisor & 0xff);
	cmdbuf.append((divisor >> 8) & 0xff);

	return Flush();
}

int MpsseInterface::Open(int port)
{
	qDebug() << __PRETTY_FUNCTION__ << "(" << port << (hex) << usb_vid << usb_pid << ") IN";

	if (port < 0)
	{
		// TODO check the USB
		return -1;
	}

	int ret_val = OK;

	if (GetInstalled() != port)
	{
		QString qs = E2Profile::GetMpsseInterfacePort();
		ftdi_interface interf = INTERFACE_A;

		if (qs.compare("B", Qt::CaseInsensitive) == 0)
		{
			interf = INTERFACE_B;
		}
		else if (qs.compare("C", Qt::CaseInsensitive) == 0)
		{
			interf = INTERFACE_C;
		}
		else if (qs.compare("D", Qt::CaseInsensitive) == 0)
		{
			interf = INTERFACE_D;
		}

		int result = ctx.set_interface(interf);
		if (result == 0)
		{
			result = ctx.open(usb_vid, usb_pid);
		}
		if (result == 0)
		{
			ctx.reset();
			//ctx.flush();
			ctx.set_read_chunk_size(64);
			ctx.set_write_chunk_size(1024);
			//ctx.set_event_char();
			//ctx.set_error_char();
			//ctx.set_usb_read_timeout(0);
			//ctx.set_usb_write_timeout(5000);
			ctx.set_latency(1);
			result = InitPins();
			Q_ASSERT(result == 0);
			Install(port);
		}
		else
		{
			qWarning() << __PRETTY_FUNCTION__ << ctx.error_string();
			ret_val = E2ERR_OPENFAILED;
		}
	}

	qDebug() << __PRETTY_FUNCTION__ << "=" << ret_val << "OUT";

	return ret_val;
}

void MpsseInterface::Close()
{
	qDebug() << __PRETTY_FUNCTION__ << "IN";

	if (IsInstalled())
	{
		SetPower(false);
		DeInitPins();
		ctx.close();
		DeInstall();
	}

	qDebug() << __PRETTY_FUNCTION__ << "OUT";
}

int MpsseInterface::Flush()
{
	int ret = OK;

	if (!cmdbuf.isEmpty())
	{
		ret = ctx.write(cmdbuf.getBuffer(), cmdbuf.getSize());
		if (ret == cmdbuf.getSize())
		{
			cmdbuf.clear();
			ret = OK;
		}
		else
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed:" << ctx.error_string();
			ret = -1;
		}
	}
	return ret;
}

void MpsseInterface::SetDelay(int delay)
{
	if (delay >= 0)
	{
		uint32_t freq = 0xffffffff;	//maximum frequency

		if (delay > 0)
		{
			freq = 1000000 / (delay * 2);
		}
		SetFrequency(freq);
	}
	BusInterface::SetDelay(delay);
}

void MpsseInterface::ShotDelay(int n)
{
	if (BusInterface::GetDelay() > 0)		//avoid any delay if 0
	{
		while (n-- > 0)
		{
			cmdbuf.append(SET_BITS_LOW);
			cmdbuf.append(last_data & 0xff);
			cmdbuf.append(pin_directions & 0xff);
		}
	}
}

int MpsseInterface::SendPins(int new_data, int new_directions)
{
	int ch_data, ch_dir;

	if (cmdbuf.almostFull())
	{
		Flush();
	}

	if (new_directions >= 0)	//-1 don't change directions
	{
		ch_dir = (new_directions ^ pin_directions) & 0xffff;
	}
	else
	{
		new_directions = pin_directions;
		ch_dir = 0;
	}

	new_data &= ~((1 << 4) | (1 << 11));

	//what's changed
	ch_data = (new_data ^ last_data) & 0xffff;

	if ((ch_data & 0x00ff) != 0 || (ch_dir & 0x00ff) != 0)	//low byte
	{
		cmdbuf.append(SET_BITS_LOW);
		cmdbuf.append(new_data & 0xff);
		cmdbuf.append(new_directions & 0xff);
	}
	if ((ch_data & 0xff00) != 0 || (ch_dir & 0xff00) != 0)	//high byte
	{
		cmdbuf.append(SET_BITS_HIGH);
		cmdbuf.append((new_data >> 8) & 0xff);
		cmdbuf.append((new_directions >> 8) & 0xff);
	}

	//if (queue_mode || Flush() == OK)
	{
		last_data = new_data;
		pin_directions = new_directions;
	}

	return OK;
}

int MpsseInterface::GetPins()
{
	if (cmdbuf.almostFull(3))
	{
		Flush();
	}

	cmdbuf.append(GET_BITS_LOW);
	cmdbuf.append(GET_BITS_HIGH);
	cmdbuf.append(SEND_IMMEDIATE);

	uint8_t buf[2];
	int ret = Flush();
	if (ret == OK)
	{
		int timeout = 10000;
		do
		{
			ret = ctx.read(buf, 2);
			if (ret < 0)
			{
				qWarning() << __PRETTY_FUNCTION__ << "read failed:" << ctx.error_string();
				return -1;
			}
		}
		while (ret == 0 && --timeout > 0);

		if (timeout > 0)
		{
			ret = ((int)buf[1] << 8) | buf[0];
		}
		else
		{
			ret = E2P_TIMEOUT;
		}
	}
	return ret;
}

uint8_t MpsseInterface::xferByte(int &err, uint8_t by, int mode, int bpw, bool lsb_first)
{
	int cmd = MPSSE_BITMODE;
	int len = bpw - 1;	//0 --> 1bit, .. 7 --> 8bits
	uint8_t ret_byte = 0;

	if (lsb_first)
	{
		cmd |= MPSSE_LSB;
	}

	if ((mode & SPIMODE_MASK) == 0 || (mode & SPIMODE_MASK) == 3)
	{
		cmd |= MPSSE_WRITE_NEG;
	}

	//We accept 0 --> default R+W, SPIMODE_WRONLY --> W, SPIMODE_RDONLY --> R, (SPIMODE_WRONLY|SPIMODE_RDONLY) --> Invalid
	Q_ASSERT((mode & (xMODE_WRONLY | xMODE_RDONLY)) != (xMODE_WRONLY | xMODE_RDONLY));
	Q_ASSERT(bpw <= 8);

	if ((mode & xMODE_WRONLY) != 0)
	{
		cmd |= MPSSE_DO_WRITE;
	}
	else if ((mode & xMODE_RDONLY) != 0)
	{
		cmd |= MPSSE_DO_READ;
	}
	else
	{
		cmd |= (MPSSE_DO_WRITE | MPSSE_DO_READ);
	}

	err = OK;

	if (cmdbuf.almostFull())
	{
		Flush();
	}

	cmdbuf.append(cmd);
	cmdbuf.append(len);
	if ((cmd & MPSSE_DO_WRITE) != 0)
	{
		cmdbuf.append(by);
	}

	if ((cmd & MPSSE_DO_READ) != 0)
	{
		cmdbuf.append(SEND_IMMEDIATE);

		int ret = Flush();
		if (ret == OK)
		{
			int timeout = 1000;
			do
			{
				ret = ctx.read(&ret_byte, 1);
				if (ret < 0)
				{
					qWarning() << __PRETTY_FUNCTION__ << "read failed:" << ctx.error_string();
					err = -1;
				}
			}
			while (ret == 0 && --timeout > 0);

			if (ret == 1)
			{
				err = OK;
			}
			else
			{
				err = E2P_TIMEOUT;
			}
		}
	}

	return ret_byte;
}

unsigned long MpsseInterface::xferWord(int &err, unsigned long word_out, int mode, int bpw, bool lsb_first)
{
	int nbit, nshift;
	unsigned long word_in = 0;

	if (lsb_first)
	{
		nshift = 0;
		do
		{
			nbit = (bpw > 8) ? 8 : bpw;
			word_in |= (xferByte(err, word_out & 0xff, mode, nbit, true) << nshift);
			word_out >>= 8;
			bpw -= nbit;
			nshift += nbit;
		}
		while (bpw > 0);
	}
	else
	{
		nbit = bpw % 8;
		nshift = (bpw / 8) * 8;
		if (nbit > 0)
		{
			word_in <<= 8;
			word_in |= xferByte(err, (word_out >> nshift) & 0xff, mode, nbit, false);
			bpw -= nbit;
		}
		while (bpw > 0)
		{
			nshift -= 8;
			word_in <<= 8;
			word_in |= xferByte(err, (word_out >> nshift) & 0xff, mode, 8, false);
			bpw -= 8;
		}
	}

	return word_in;
}

int MpsseInterface::SetPower(bool onoff)
{
	qDebug() << __PRETTY_FUNCTION__ << "(" << onoff << ")";

	if (IsInstalled() && pin_poweron > 0)
	{
		if ((cmdWin->GetPolarity() & POWERINV))
		{
			onoff = !onoff;
		}

		int val = onoff ? 1 : 0;

		SendPins(OutDataMask(pin_poweron, val));
	}

	return OK;
}

void MpsseInterface::SetControlLine(int res)
{
	//qDebug() << __PRETTY_FUNCTION__ << "(" << res << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		if (SendPins(OutDataMask(pin_ctrl, (res != 0))) != OK)
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed.";
		}
	}
}

void MpsseInterface::SetDataOut(int sda)
{
	//qDebug() << __PRETTY_FUNCTION__ << "(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		if (SendPins(OutDataMask(pin_dataout, (sda != 0))) != OK)
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed.";
		}
	}
}

void MpsseInterface::SetClock(int scl)
{
	//qDebug() << __PRETTY_FUNCTION__ << "(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		if (SendPins(OutDataMask(pin_clock, (scl != 0))) != OK)
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed.";
		}
	}
}

void MpsseInterface::SetClockData()
{
	//qDebug() << __PRETTY_FUNCTION__ << "*** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int scl = 1, sda = 1;

		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		int n_data = OutDataMask(pin_clock, scl);
		if (SendPins(OutDataMask(n_data, pin_dataout, sda)) != OK)
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed.";
		}
	}
}

void MpsseInterface::ClearClockData()
{
	//qDebug() << __PRETTY_FUNCTION__ << "*** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int scl = 0, sda = 0;

		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		int n_data = OutDataMask(pin_clock, scl);
		if (SendPins(OutDataMask(n_data, pin_dataout, sda)) != OK)
		{
			qWarning() << __PRETTY_FUNCTION__ << "write failed.";
		}
	}
}

int MpsseInterface::GetDataIn()
{
	if (IsInstalled())
	{
		int val = GetPins();
		if (val < 0)
		{
			qWarning() << __PRETTY_FUNCTION__ << "read failed.";
			return val;
		}
		else
		{
			qDebug() << __PRETTY_FUNCTION__ << "=" << val;
			val = (val & pin_datain) ? 1 : 0;

			if (cmdWin->GetPolarity() & DININV)
			{
				return !val;
			}
			else
			{
				return val;
			}
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
	//qDebug() << __PRETTY_FUNCTION__ << "*** Inst=" << IsInstalled();

	return GetDataIn();
}

int MpsseInterface::IsClockDataDOWN()
{
	//qDebug() << __PRETTY_FUNCTION__ << "*** Inst=" << IsInstalled();

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
