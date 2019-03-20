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
{
	qDebug() << "MpsseInterface::MpsseInterface()";

	MpsseInterface::List();
	//DeInstall();
	//old_portno = GetInstalled();
	last_data = read_data = 0;

	pin_ctrl = (1 << 9);
	pin_datain = 4;
	pin_dataout = 2;
	pin_clock = 1;
	pin_clockin = 0;

	cmdbuf.clear();

	queue_mode = false;
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

	last_data = 0;
	pin_directions = 0;

	pin_ctrl = 1 << E2Profile::GetMpssePinCtrl();
	pin_datain = 1 << E2Profile::GetMpssePinDataIn();
	pin_dataout = 1 << E2Profile::GetMpssePinDataOut();
	pin_clock = 1 << E2Profile::GetMpssePinClock();

	qDebug() << "MpsseInterface::InitPins"
			<< " Ctrl=" << pin_ctrl
			<< ", Clock=" << pin_clock
			<< ", DataIn=" << pin_datain << ", DataOut=" << pin_dataout;

	result = ctx.set_bitmode(0, BITMODE_MPSSE);
	if (result == 0)
	{
		ctx.flush();

		//00011011 --> 0x1B
		int new_data = 0;
		int new_directions = pin_ctrl | pin_dataout | pin_clock;
		new_directions |= (1 << 4) | (1 << 11);

		//Force update
		last_data = ~new_data & 0xffff;
		pin_directions = ~new_directions & 0xffff;
		SendPins(new_data, new_directions);	//set pins to ZERO
	}

	return result;
}

void MpsseInterface::DeInitPins()
{
}

int MpsseInterface::SetFrequency(uint32_t freq)
{
	int32_t divisor;
	uint8_t buf[3];

	Q_ASSERT(freq > 0);

	divisor = (6000000 / freq) - 1;
	if (divisor < 0)
	{
		qDebug() << "Frequency high " << freq;
		divisor = 0;
	}

	if (divisor > 65535)
	{
		qDebug() << "Frequency low " << freq;
		divisor = 65535;
	}

	qDebug() <<  "Frequency " << (6000000/(divisor+1)) << ", Divisor " << divisor;

	buf[0] = TCK_DIVISOR;
	buf[1] = (uint8_t)divisor;
	buf[2] = (uint8_t)(divisor >> 8);

	return ctx.write(buf, sizeof(buf));
}


int MpsseInterface::Open(int port)
{
	qDebug() << "MpsseInterface::Open(" << port << ") IN";

	int ret_val = OK;

	if (GetInstalled() != port)
	{
		queue_mode = false;

		QString qs = E2Profile::GetMpsseInterfacePort();
		ftdi_interface interf = INTERFACE_A;

		if (qs.compare("B", Qt::CaseInsensitive) == 0)
			interf = INTERFACE_B;
		else if (qs.compare("C", Qt::CaseInsensitive) == 0)
			interf = INTERFACE_C;
		else if (qs.compare("D", Qt::CaseInsensitive) == 0)
			interf = INTERFACE_D;

		int result = ctx.set_interface(interf);
		if (result == 0)
			result = ctx.open(usb_vid, usb_pid);
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
			ctx.set_bitmode(0, BITMODE_RESET);
			result = InitPins();
			Q_ASSERT(result == 0);
			queue_mode = true;
			TestPins();
			Install(port);
		}
		else
		{
			qWarning() << "MpsseInterface::Open() " << ctx.error_string();
			ret_val = E2ERR_OPENFAILED;
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
			qWarning("MpsseInterface::Flush() write failed (%s)\n", ctx.error_string());
			ret = -1;
		}
	}
	return ret;
}

int MpsseInterface::SendPins(int new_data, int new_directions)
{
	int ch_data, ch_dir;

	Q_ASSERT(!cmdbuf.almostFull());

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

	if (queue_mode || Flush() == OK)
	{
		last_data = new_data;
		pin_directions = new_directions;
	}

	return OK;
}

void MpsseInterface::GetPinsCommit(int data_mask)
{
	if (data_mask < 0)
		data_mask = pin_datain;

	Q_ASSERT(!cmdbuf.almostFull(2));

	if (data_mask & 0x00ff)
		cmdbuf.append(GET_BITS_LOW);
	if (data_mask & 0xff00)
		cmdbuf.append(GET_BITS_HIGH);
}

int MpsseInterface::ReadQueuedPins()
{
	Q_ASSERT(!cmdbuf.almostFull(1));

	cmdbuf.append(SEND_IMMEDIATE);
	in_datacount = cmdbuf.getDataInCount();		//store datacount before the flush

	Q_ASSERT(in_datacount > 0 && in_datacount <= sizeof(in_buffer));

	int ret = Flush();
	if (ret == OK)
	{
		int count, remain, timeout = 10000;
		count = 0;
		remain = in_datacount;
		do {
			ret = ctx.read(in_buffer + count, remain);
			if (ret < 0)
			{
				qWarning("MpsseInterface::ReadPins() read failed (%s)\n", ctx.error_string());
				return -1;
			}
			else
			{
				remain -= ret;
				count += ret;
			}
		} while (remain > 0 && --timeout > 0);

		if (remain == 0)
			ret = OK;
		else
			ret = E2P_TIMEOUT;
	}
	return ret;
}

QBitArray MpsseInterface::ParseQueuedPin(int data_mask)
{
	if (data_mask < 0)
		data_mask = pin_datain;

	Q_ASSERT(in_datacount > 0);

	QBitArray ba(in_datacount);

	for (unsigned int idx = 0; idx < in_datacount; idx++)
	{
		if (in_buffer[idx] & data_mask)
			ba.setBit(idx, true);
	}
	return ba;
}

int MpsseInterface::TestPins()
{
	for (int k = 0; k < 8; k++)
	{
		SendPins(OutDataMask(pin_clock, 2));
		SendPins(OutDataMask(pin_clock, 2));
		GetPinsCommit();
	}
	if (ReadQueuedPins() != OK)
	{
		qWarning("MpsseInterface::TestPins() read failed\n");
		return -1;
	}
	else
	{
		QBitArray ba = ParseQueuedPin();
		qDebug() << "Readpin: " << ba;
	}

	return OK;
}

void MpsseInterface::SetControlLine(int res)
{
	//qDebug() << "MpsseInterface::SetControlLine(" << res << ") *** Inst=" <<  IsInstalled();

	if (IsInstalled())
	{
		if (cmdWin->GetPolarity() & RESETINV)
			res = !res;

		if (SendPins(OutDataMask(pin_ctrl, (res != 0))) != OK)
		{
			qWarning("MpsseInterface::SetControlLine() write failed\n");
		}
	}
}

void MpsseInterface::SetDataOut(int sda)
{
	//qDebug() << "MpsseInterface::SetDataOut(" << sda << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & DOUTINV))
			sda = !sda;

		if (SendPins(OutDataMask(pin_dataout, (sda != 0))) != OK)
		{
			qWarning("MpsseInterface::SetDataOut() write failed\n");
		}
	}
}

void MpsseInterface::SetClock(int scl)
{
	//qDebug() << "MpsseInterface::SetClock(" << scl << ") *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		if ((cmdWin->GetPolarity() & CLOCKINV))
			scl = !scl;

		if (SendPins(OutDataMask(pin_clock, (scl != 0))) != OK)
		{
			qWarning("MpsseInterface::SetClock() write failed\n");
		}
	}
}

void MpsseInterface::SetClockData()
{
	//qDebug() << "MpsseInterface::SetClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int scl = 1, sda = 1;

		if ((cmdWin->GetPolarity() & CLOCKINV))
			scl = !scl;

		if ((cmdWin->GetPolarity() & DOUTINV))
			sda = !sda;

		int n_data = OutDataMask(pin_clock, scl);
		if (SendPins(OutDataMask(n_data, pin_dataout, sda)) != OK)
		{
			qWarning("MpsseInterface::SetClockData() write failed\n");
		}
	}
}


void MpsseInterface::ClearClockData()
{
	//qDebug() << "MpsseInterface::ClearClockData() *** Inst=" << IsInstalled();

	if (IsInstalled())
	{
		int scl = 0, sda = 0;

		if ((cmdWin->GetPolarity() & CLOCKINV))
			scl = !scl;

		if ((cmdWin->GetPolarity() & DOUTINV))
			sda = !sda;

		int n_data = OutDataMask(pin_clock, scl);
		if (SendPins(OutDataMask(n_data, pin_dataout, sda)) != OK)
		{
			qWarning("MpsseInterface::SetClockData() write failed\n");
		}
	}
}

int MpsseInterface::GetDataIn()
{
	if (IsInstalled())
	{
		GetPinsCommit();
		if (!queue_mode)
		{
			if (ReadQueuedPins() != OK)
			{
				qWarning("MpsseInterface::GetDataIn() read failed\n");
				return -1;
			}
			else
			{
				QBitArray ba = ParseQueuedPin();
				unsigned int val = 0;

				if (!ba.isEmpty())
					val = ba.testBit(0);
				qDebug() << "MpsseInterface::GetDataIn()=" << val;

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
			return NOT_READY;
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
