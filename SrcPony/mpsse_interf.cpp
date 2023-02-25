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
// FTDI mpsse IO

#include "mpsse_interf.h"
#include "errcode.h"
#include "e2cmdw.h"

#include <QtCore>
#include <QDebug>
#include <QProcess>
#include <QString>

MpsseInterface::MpsseInterface()
	:	cmdbuf(),
	  pin_directions(0),
	  ignore_last_data(true),
	  last_data(0),
	  read_data(0),
	  pin_ctrl(0),
	  pin_ctrlin(0),
	  pin_datain(0),
	  pin_dataout(0),
	  pin_clock(0),
	  pin_clockin(0),
	  pin_poweron(0),
	  ftdi_port(FTDI_PORTA),
	  test_step(0)
{
	qDebug() << Q_FUNC_INFO;
}

MpsseInterface::~MpsseInterface()
{
	qDebug() << Q_FUNC_INFO;

	Close();
}

void MpsseInterface::ConfigPins(int pinum_ctrl, int pinum_datain, int pinum_dataout, int pinum_clock, int pinum_clockin, int pinum_poweron, int pinum_enbus, int pinnum_ctrlin)
{
	BusInterface::ConfigPins(pinum_ctrl, pinum_datain, pinum_dataout, pinum_clock, pinum_clockin, pinum_poweron, pinum_enbus, pinnum_ctrlin);

	if (pinum_ctrl >= 0)
	{
		pin_ctrl = 1 << pinum_ctrl;
	}

	if (pinum_datain >= 0)
	{
		pin_datain = 1 << pinum_datain;
	}

	if (pinum_dataout >= 0)
	{
		pin_dataout	= 1 << pinum_dataout;
	}

	if (pinum_clock >= 0)
	{
		pin_clock = 1 << pinum_clock;
	}
	if (pinnum_ctrlin >= 0)
	{
		pin_ctrlin = 1 << pinnum_ctrlin;
	}

	if (pinum_clockin >= 0)
	{
		pin_clockin = 1 << pinum_clockin;
	}

	if (pinum_poweron >= 0)
	{
		pin_poweron = 1 << pinum_poweron;
	}

	if (pinum_enbus >= 0)
	{
		pin_enbus = 1 << pinum_enbus;
	}
}

int MpsseInterface::InitPins()
{
	int result;

	result = ctx.set_bitmode(0, BITMODE_RESET);
	if (result == 0)
	{
		ignore_last_data = true;
		last_data = 0;
		pin_directions = 0;

		if (pin_ctrl == 0 && pin_clock == 0 && pin_datain == 0 && pin_dataout == 0)
		{
			qWarning() << Q_FUNC_INFO << " Unconfigured pins";
			return E2ERR_OPENFAILED;
		}

		qDebug() << Q_FUNC_INFO << (Qt::hex)
				 << " Ctrl=" << pin_ctrl
				 << ", Clock=" << pin_clock << ", ClockIn=" << pin_clockin
				 << ", DataIn=" << pin_datain << ", DataOut=" << pin_dataout
				 << ", Poweron=" << pin_poweron << ", EnBus=" << pin_enbus;

		result = ctx.set_bitmode(0, BITMODE_MPSSE);
		if (result == 0)
		{
			ctx.flush();
			cmdbuf.clear();

			int new_data = 0;

			if (GetI2CMode())
			{
				cmdbuf.append(EN_3_PHASE);
				new_data |= pin_dataout | pin_clock;	//I2C need pins high
				new_data &= ~pin_enbus;
			}
			else
			{
				cmdbuf.append(DIS_3_PHASE);
				new_data |= pin_enbus;
			}

			//cmdbuf.append(LOOPBACK_END);
			//cmdbuf.append(DIS_ADAPTIVE);

			//00011011 --> 0x1B
			int new_directions = pin_ctrl | pin_dataout | pin_clock | pin_poweron | pin_enbus;
			if (usb_vp == TypeToInterfVidPid(FTDI_JTAGKEY))
			{
				new_directions |= (1 << 4) | (1 << 11);		//hack
			}

			//Force update
			last_data = ~new_data & 0xffff;
			pin_directions = ~new_directions & 0xffff;
			SendPins(new_data, new_directions);	//set pins to ZERO
			result = Flush();
			WaitMsec(10);
		}
	}

	return result;
}

void MpsseInterface::DeInitPins()
{
	//ctx.set_bitmode(0, BITMODE_RESET);
	int new_data = pin_enbus;
	SendPins(new_data);	//set pins to ZERO
	Flush();
}

int MpsseInterface::SetFrequency(quint32 freq)
{
	qint32 divisor, sysclock;

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
		qDebug() << Q_FUNC_INFO << "Frequency high" << freq;
		divisor = 0;
	}

	if (divisor > 65535)
	{
		qDebug() << Q_FUNC_INFO << "Frequency low" << freq;
		divisor = 65535;
	}

	qDebug() << Q_FUNC_INFO << "Frequency" << (sysclock / (2 * (divisor + 1))) << ", Divisor" << divisor << ", SysClock" << sysclock;

	cmdbuf.append(TCK_DIVISOR);
	cmdbuf.append(divisor & 0xff);
	cmdbuf.append((divisor >> 8) & 0xff);

	return Flush();
}

int MpsseInterface::Open(int port)
{
	qDebug() << Q_FUNC_INFO << "(" << port << (Qt::hex) << usb_vp.vid << ":" << usb_vp.pid << ") IN";

	int ret_val = OK;

	if (GetInstalled() != port)
	{
		ftdi_port = TypeToInterfPort(cmdWin->GetInterfaceType());

		ftdi_interface interf = INTERFACE_A;

		if (ftdi_port == FTDI_PORTB)
		{
			interf = INTERFACE_B;
		}
		else if (ftdi_port == FTDI_PORTC)
		{
			interf = INTERFACE_C;
		}
		else if (ftdi_port == FTDI_PORTD)
		{
			interf = INTERFACE_D;
		}

		int result = ctx.set_interface(interf);
		if (result == 0)
		{
			std::string sdesc, sser;

			//result = ctx.open(usb_vp.vid, usb_vp.pid);
			result = ctx.open(usb_vp.vid, usb_vp.pid, sdesc, sser, port);
		}
		if (result == 0)
		{
			ctx.reset();
			//ctx.flush();
			ctx.set_read_chunk_size(256);
			ctx.set_write_chunk_size(1024);
			//ctx.set_event_char();
			//ctx.set_error_char();
			//ctx.set_usb_read_timeout(0);
			//ctx.set_usb_write_timeout(5000);
			ctx.set_latency(1);
			result = InitPins();
			if (result == 0)
			{
				Install(port);
			}
			else
			{
				ctx.close();
				ret_val = E2ERR_OPENFAILED;
			}
		}
		else
		{
			qWarning() << Q_FUNC_INFO << ctx.error_string();
			ret_val = E2ERR_OPENFAILED;
		}
	}

	qDebug() << Q_FUNC_INFO << "=" << ret_val << "OUT";

	return ret_val;
}

void MpsseInterface::Close()
{
	qDebug() << Q_FUNC_INFO << "IN - Installed: " << IsInstalled();

	if (IsInstalled())
	{
		SetPower(false);
		DeInitPins();
		ctx.close();
		DeInstall();
	}
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
			qWarning() << Q_FUNC_INFO << "write failed:" << ctx.error_string();
			ret = -1;
		}
	}
	return ret;
}

void MpsseInterface::SetDelay(int delay)
{
	if (delay >= 0)
	{
		quint32 freq = 0xffffffff;	//maximum frequency

		if (delay > 0)
		{
			freq = 1000000 / (delay * 2);
			if (GetI2CMode())
			{
				freq = freq * 3 / 2;		//3PHASE_CLOCKING
			}
		}
		SetFrequency(freq);
	}
	BusInterface::SetDelay(delay);
}

void MpsseInterface::ShotDelay(int n)
{
	if (BusInterface::GetDelay() > 0)		//avoid any delay if 0
	{
		if (GetI2CMode())
		{
			n *= 3;	//I2CBus use 3_PHASE clock
		}
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

	//new_data &= ~((1 << 4) | (1 << 11));

	//what's changed
	ch_data = (new_data ^ last_data) & 0xffff;
	if (ignore_last_data)
	{
		ch_data |= 0xff;
	}

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
		ignore_last_data = false;
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

	int ret = Flush();
	if (ret == OK)
	{
		quint8 buf[2];
		int timeout = 10000;
		do
		{
			ret = ctx.read(buf, 2);
			if (ret < 0)
			{
				qWarning() << Q_FUNC_INFO << "read failed:" << ctx.error_string();
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

int MpsseInterface::GetLowPinsMulti(int bufsiz, quint8 *buf, int len)
{
	if (len > bufsiz)
	{
		len = bufsiz;
	}

	if (cmdbuf.almostFull(len))
	{
		Flush();
	}

	for (int k = 0; k < len; k++)
	{
		cmdbuf.append(GET_BITS_LOW);
	}
	cmdbuf.append(SEND_IMMEDIATE);

	int ret = Flush();
	if (ret == OK)
	{
		int timeout = 10000;
		do
		{
			ret = ctx.read(buf, len);
			if (ret < 0)
			{
				qWarning() << Q_FUNC_INFO << "read failed:" << ctx.error_string();
				return -1;
			}
		}
		while (ret == 0 && --timeout > 0);

		if (timeout == 0)
		{
			ret = E2P_TIMEOUT;
		}
	}
	return ret;
}

bool MpsseInterface::CheckDataLines(int len, int sda, int scl)
{
	quint8 buf[512];
	bool test = false;

	Q_ASSERT(len > 0);
	Q_ASSERT(len <= (int)sizeof(buf));

	if (GetLowPinsMulti(sizeof(buf), buf, len) == len)
	{
		test = true;

		for (int k = 0; test && k < len; k++)
		{
			if (sda == 0 && scl == 0)
			{
				test = IsClockDataDOWN(buf[k]);
			}
			else if (sda > 0 && scl > 0)
			{
				test = IsClockDataUP(buf[k]);
			}
			else
			{
				bool test_sda = true, test_scl = true;

				if (sda > 0)
				{
					test_sda = GetDataIn(buf[k]);
				}
				else if (sda == 0)
				{
					test_sda = !GetDataIn(buf[k]);
				}

				if (scl > 0)
				{
					test_scl = GetClock(buf[k]);
				}
				else if (scl == 0)
				{
					test_scl = !GetClock(buf[k]);
				}

				test = (test_sda && test_scl);
			}
		}
	}

	return test;
}


int MpsseInterface::xferBit(int &err, int b, int mode)
{
	int cmd = MPSSE_BITMODE;
	int len = 0;	//0 --> 1bit
	int by = (b != 0) ? 0xff : 0;
	quint8 ret_byte = 0;

	//We accept 0 --> default R+W, SPIMODE_WRONLY --> W, SPIMODE_RDONLY --> R, (SPIMODE_WRONLY|SPIMODE_RDONLY) --> Invalid
	Q_ASSERT((mode & (xMODE_WRONLY | xMODE_RDONLY)) != (xMODE_WRONLY | xMODE_RDONLY));

	if ((mode & xMODE_WRONLY) != 0)
	{
		cmd |= MPSSE_DO_WRITE;

		if ((mode & SPIMODE_MASK) == 0 || (mode & SPIMODE_MASK) == 3)
		{
			cmd |= MPSSE_WRITE_NEG;
		}
	}
	else if ((mode & xMODE_RDONLY) != 0)
	{
		cmd |= MPSSE_DO_READ;

		if ((mode & SPIMODE_MASK) == 1 || (mode & SPIMODE_MASK) == 2)
		{
			cmd |= MPSSE_READ_NEG;
		}
	}
	else
	{
		cmd |= (MPSSE_DO_WRITE | MPSSE_DO_READ);

		if ((mode & SPIMODE_MASK) == 0 || (mode & SPIMODE_MASK) == 3)
		{
			cmd |= MPSSE_WRITE_NEG;
		}
		else
		{
			cmd |= MPSSE_READ_NEG;
		}
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
					qWarning() << Q_FUNC_INFO << "read failed:" << ctx.error_string();
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
	ignore_last_data = true;

	return (ret_byte & 1);
}

quint8 MpsseInterface::xferByte(int &err, quint8 by, int mode, int bpw, bool lsb_first)
{
	int cmd = MPSSE_BITMODE;
	int len = bpw - 1;	//0 --> 1bit, .. 7 --> 8bits
	quint8 ret_byte = 0;

	if (lsb_first)
	{
		cmd |= MPSSE_LSB;
	}

	//We accept 0 --> default R+W, SPIMODE_WRONLY --> W, SPIMODE_RDONLY --> R, (SPIMODE_WRONLY|SPIMODE_RDONLY) --> Invalid
	Q_ASSERT((mode & (xMODE_WRONLY | xMODE_RDONLY)) != (xMODE_WRONLY | xMODE_RDONLY));
	Q_ASSERT(bpw <= 8);

	if ((mode & xMODE_WRONLY) != 0)
	{
		cmd |= MPSSE_DO_WRITE;

		if ((mode & SPIMODE_MASK) == 0 || (mode & SPIMODE_MASK) == 3)
		{
			cmd |= MPSSE_WRITE_NEG;
		}
	}
	else if ((mode & xMODE_RDONLY) != 0)
	{
		cmd |= MPSSE_DO_READ;

		if ((mode & SPIMODE_MASK) == 1 || (mode & SPIMODE_MASK) == 2)
		{
			cmd |= MPSSE_READ_NEG;
		}
	}
	else
	{
		cmd |= (MPSSE_DO_WRITE | MPSSE_DO_READ);

		if ((mode & SPIMODE_MASK) == 0 || (mode & SPIMODE_MASK) == 3)
		{
			cmd |= MPSSE_WRITE_NEG;
		}
		else
		{
			cmd |= MPSSE_READ_NEG;
		}
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
					qWarning() << Q_FUNC_INFO << "read failed:" << ctx.error_string();
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
	ignore_last_data = true;

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
			word_in |= xferByte(err, ((word_out >> nshift) & 0xff) << (8 - nbit), mode, nbit, false);
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
	qDebug() << Q_FUNC_INFO << "(" << onoff << ")";

	if (IsInstalled() && pin_poweron > 0)
	{
		if ((cmdWin->GetPolarity() & POWERINV))
		{
			onoff = !onoff;
		}

		int val = onoff ? 1 : 0;

		SendPins(OutDataMask(pin_poweron, val));
		Flush();
	}

	return OK;
}

void MpsseInterface::SetControlLine(int res)
{
	//qDebug() << Q_FUNC_INFO << "(" << res << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
		{
			res = !res;
		}

		if (SendPins(OutDataMask(pin_ctrl, (res != 0))) != OK)
		{
			qWarning() << Q_FUNC_INFO << "write failed.";
		}
	}
}

void MpsseInterface::SetDataOut(int sda)
{
	//qDebug() << Q_FUNC_INFO << "(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
		{
			sda = !sda;
		}

		if (SendPins(OutDataMask(pin_dataout, (sda != 0))) != OK)
		{
			qWarning() << Q_FUNC_INFO << "write failed.";
		}
	}
}

void MpsseInterface::SetClock(int scl)
{
	//qDebug() << Q_FUNC_INFO << "(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
		{
			scl = !scl;
		}

		if (SendPins(OutDataMask(pin_clock, (scl != 0))) != OK)
		{
			qWarning() << Q_FUNC_INFO << "write failed.";
		}
	}
}

void MpsseInterface::SetClockData()
{
	//qDebug() << Q_FUNC_INFO << "*** Inst=" << IsInstalled();

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
			qWarning() << Q_FUNC_INFO << "write failed.";
		}
	}
}

void MpsseInterface::ClearClockData()
{
	//qDebug() << Q_FUNC_INFO << "*** Inst=" << IsInstalled();

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
			qWarning() << Q_FUNC_INFO << "write failed.";
		}
	}
}

int MpsseInterface::GetCtrlIn(int val)
{
	if (pin_ctrlin == 0)
	{
		return -1;
	}
	else
	{
		return (val & pin_ctrlin) ? 1 : 0;
	}
}

int MpsseInterface::GetDataIn(int val)
{
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

int MpsseInterface::GetDataIn()
{
	if (IsInstalled())
	{
		int val = GetPins();
		if (val < 0)
		{
			qWarning() << Q_FUNC_INFO << "read failed.";
			return val;
		}
		else
		{
			return GetDataIn(val);
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int MpsseInterface::GetClock(int val)
{
	if (pin_clockin == 0)
	{
		return 1;
	}
	else
	{
		return (val & pin_clockin) ? 1 : 0;
	}
}

int MpsseInterface::GetClock()
{
	if (pin_clockin == 0)
	{
		return 1;
	}
	else
	{
		if (IsInstalled())
		{
			int val = GetPins();
			if (val < 0)
			{
				qWarning() << Q_FUNC_INFO << "read failed.";
				return val;
			}
			else
			{
				return GetClock(val);
			}
		}
		else
		{
			return E2ERR_NOTINSTALLED;
		}
	}
}

int MpsseInterface::IsClockDataUP(int val)
{
	if (pin_clockin == 0)
	{
		return GetDataIn(val);
	}
	else
	{
		return (GetClock(val) && GetDataIn(val));
	}
}

int MpsseInterface::IsClockDataUP()
{
	if (IsInstalled())
	{
		int val = GetPins();
		if (val < 0)
		{
			qWarning() << Q_FUNC_INFO << "read failed.";
			return val;
		}
		else
		{
			return IsClockDataUP(val);
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int MpsseInterface::IsClockDataDOWN(int val)
{
	if (pin_clockin == 0)
	{
		return !GetDataIn(val);
	}
	else
	{
		return (!GetClock(val) && !GetDataIn(val));
	}
}

int MpsseInterface::IsClockDataDOWN()
{
	if (IsInstalled())
	{
		int val = GetPins();
		if (val < 0)
		{
			qWarning() << Q_FUNC_INFO << "read failed.";
			return val;
		}
		else
		{
			return IsClockDataDOWN(val);
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

int MpsseInterface::GetPresence(int mask, int val)
{
	qDebug() << Q_FUNC_INFO << "() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int pinval = GetPins();
		if (pinval < 0)
		{
			qWarning() << Q_FUNC_INFO << "read failed.";
			return val;
		}
		else
		{
			return ((pinval & mask) == val) ? OK : E2ERR_OPENFAILED;
		}
	}
	else
	{
		return E2ERR_NOTINSTALLED;
	}
}

#include <QMessageBox>

int MpsseInterface::TestPort(int port_no)
{
#if 1
	test_step = 0;
#endif
	qDebug() << Q_FUNC_INFO  << "(port_no:" << port_no << ", test_step:" << test_step << ") IN";

	int ret_val = TestSave(port_no);

	if (ret_val == OK)
	{
		if (TypeToInterfVidPid(PONYPROG_FT) == usb_vp)
		{
			if (test_step++ > 0)
			{
				bool test_inv = false;
				int errcount = 0;

				if (test_step > 2)
				{
					QMessageBox mbox;
					mbox.setText("Passo " + QString::number(test_step) + " Insert check-board");
					mbox.exec();

					test_step = 0;
					SendPins(OutDataMask(pin_enbus, 0));
				}
				else
				{
					QMessageBox mbox;
					mbox.setText("Passo " + QString::number(test_step) + " Remove check-board");
					mbox.exec();

					SendPins(OutDataMask(pin_enbus, 1));
				}

				int val, pmask_clkin = (1 << 4), seto;

				SetPower(true);
				w.WaitMsec(150);

				for (seto = 1; seto >= 0; seto--)
				{
					int geti = test_inv ? !seto : seto;

					SendPins(OutDataMask(pin_dataout, seto));
					w.WaitMsec(100);
					val = GetPins();
					if (val < 0)
					{
						return E2ERR_NOTINSTALLED;
					}
					if ((val & pin_datain) != 0)
					{
						val = 1;
					}
					else
					{
						val = 0;
					}
					if (val != geti)
					{
						qWarning() << Q_FUNC_INFO  << " Data Write " << seto << " read " << val << "(" << geti << ")";
						errcount++;
					}
				}

				for (seto = 1; seto >= 0; seto--)
				{
					int geti = test_inv ? !seto : seto;

					SendPins(OutDataMask(pin_clock, seto));
					w.WaitMsec(100);
					val = GetPins();
					if (val < 0)
					{
						return E2ERR_NOTINSTALLED;
					}
					if ((val & pmask_clkin) != 0)
					{
						val = 1;
					}
					else
					{
						val = 0;
					}
					if (val != geti)
					{
						qWarning() << Q_FUNC_INFO  << " Clock Write " << seto << " read " << val << "(" << geti << ")";
						errcount++;
					}
				}

				SendPins(OutDataMask(pin_enbus, 1));
				SendPins(OutDataMask(pin_ctrl | pin_dataout | pin_clock, 0));
				SetPower(false);

				if (errcount > 0)
				{
					ret_val = E2ERR_NOTINSTALLED;
				}
			}
			else
			{
				//PonyProgFT
				w.WaitMsec(10);
				ret_val = GetPresence(0x0700, 0);

				if (ret_val == OK)
				{
					SendPins(OutDataMask(pin_enbus, 1));	//en_bus active low
					SendPins(OutDataMask(pin_ctrl | pin_dataout | pin_clock, 0));
					SetPower(true);
					w.WaitMsec(150);
					int val = GetPins();
					if (val < 0 || GetCtrlIn(val) != 1)
					{
						if (GetDataIn(val) != 1)
						{
							ret_val = E2ERR_IOTEST;
						}
						else
						{
							ret_val = E2ERR_NOTINSTALLED;
						}
					}
					else
					{
						SendPins(OutDataMask(pin_ctrl, 1));
						w.WaitMsec(100);
						val = GetPins();
						if (val < 0 || GetCtrlIn(val) != 0)
						{
							ret_val = E2ERR_NOTINSTALLED;
						}
					}
					SendPins(OutDataMask(pin_ctrl | pin_dataout | pin_clock, 0));
					SetPower(false);
				}
			}
		}
		else if (TypeToInterfVidPid(FTDI_JTAGKEY) == usb_vp)
		{
			//JtagKey
			ret_val = OK;
		}
		else
		{
			ret_val = E2ERR_NOTINSTALLED;
		}
	}

	TestRestore();

	qDebug() << Q_FUNC_INFO << "() = " << ret_val << " OUT";

	return ret_val;
}
