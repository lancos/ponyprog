//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2021   Claudio Lanconelli                           //
//                                                                         //
//  https://github.com/lancos/ponyprog                                     //
//                                                                         //
//  LibUSB implementation for PonyProg (C) 2019 Eduard Kalinowski          //
//                                                                         //
//  sources:                                                               //
//  Copyright (C) 2014 Pluto Yang (yangyj.ee@gmail.com)                    //
//  https://github.com/setarcos/ch341prog                                  //
//                                                                         //
//  Copyright (C) 2016 Eugene Hutorny (eugene@hutorny.in.ua)               //
//  https://github.com/hutorny/usbuart                                     //
//                                                                         //
//  Copyright (c) 2017 Gunar Schorcht (gunar@schorcht.net)                 //
//  https://github.com/gschorcht/spi-ch341-usb                             //
//  https://github.com/gschorcht/i2c-ch341-usb                             //
//                                                                         //
//  Copyright (c) 2018 Sarim Khan  (sarim2005@gmail.com)                   //
//  https://github.com/sarim/ch341a-bitbang-userland                       //
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



#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "ch341a.h"

// qint32 bulkin_count; // set by the callback function

#define DEBUG_CH341 0

struct sigaction saold;
int force_stop = 0;
quint32 syncackpkt;  // synch / ack flag used by BULK OUT cb function
quint16 byteoffset;  // for read eeprom function

qint16 ch341::read_completed = 0;
qint16 ch341::write_completed = 0;


/* SIGINT handler */
void sig_int(int signo)
{
	force_stop = 1;
}

/**
 * ch341 requres LSB first, reverse the bit order before send and after receive
 * for more tricks, see https://graphics.stanford.edu/~seander/bithacks.html
 */
inline quint8 ReverseByte(quint8 c)
{
	quint8 result = 0;

	for (int i = 0; i < 8; ++i)
	{
		result = result << 1;
		result |= (c & 1);
		c = c >> 1;
	}

	return result;
}


inline quint16 ReverseWord(quint16 c)
{
	quint16 result = 0;

	for (int i = 0; i < 16; ++i)
	{
		result = result << 1;
		result |= (c & 1);
		c = c >> 1;
	}

	return result;
}

#if 0
static void read_cb(libusb_transfer *transfer) noexcept
{
	switch (transfer->status)
	{
	case LIBUSB_TRANSFER_COMPLETED:
		if (transfer->user_data != NULL)
		{
			// TODO ReverseByte ???
		}
		ch341::read_completed = 1;
// 		bulkin_count++;
		break;
	default:
		qCritical("broken callback in transfer %p", transfer);
		ch341::read_completed = -1;
// 		bulkin_count = -1;
		break;
	}
}

static void write_cb(libusb_transfer *transfer) noexcept
{
//     file_channel *chnl = (file_channel *) transfer->user_data;
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		qCritical("broken callback in transfer %p", transfer);
		ch341::write_completed = -1;
	}
	else
	{
		ch341::write_completed = 1;
	}
}

void ch34x_callback(libusb_transfer *transfer)
{
	delete (unsigned char *)transfer->user_data;
	libusb_free_transfer(transfer);
}
#endif

/**
 * @brief callback function from control transport change event
 */

#if 0
static void cb_ctrl_changed(struct libusb_transfer *transfer)
{
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		qCritical("broken callback in transfer %p stat %d", transfer, transfer->status);
	}

	qCritical("async cb_ctrl_changed length=%d actual_length=%d state %d\n", transfer->length, transfer->actual_length, transfer->status);

	//libusb_free_transfer(transfer);

// 	if (next_state() < 0)
// 		request_exit(2);
}
#endif


#if 0
/**
 * callback for bulk out async transfer
 */
void cbBulkOut(struct libusb_transfer *transfer)
{
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		qCritical("cbBulkOut() error : %d", transfer->status);
	}
	syncackpkt = 1;
}

/**
 * callback for bulk in async transfer
 */
void cbBulkIn(struct libusb_transfer *transfer)
{
	switch (transfer->status)
	{
	case LIBUSB_TRANSFER_COMPLETED:

		/* the first package has cmd and address info, so discard 4 bytes */
		if (transfer->user_data != NULL)
		{
			for (int i = (bulkin_count == 0) ? 4 : 0; i < transfer->actual_length; ++i)
			{
				*((quint8 *)transfer->user_data++) = ReverseByte(transfer->buffer[i]);
			}
		}

		bulkin_count++;
		break;

	default:
		qCritical("cbBulkIn() error : %d", transfer->status);
		libusb_free_transfer(transfer);
		bulkin_count = -1;
	}

	return;
}
#endif


ch341::ch341(QObject *parent)
{
	verbose = false;
	dtr = 0;
	rts = 0;

	baudRate = DEFAULT_BAUD_RATE;

	parity = 'N';
	bits = 8;
	stops = 1;

	timeout = DEFAULT_TIMEOUT;

	devHandle = NULL;

	rtsCtsEnabled = 0;
	dtrDsrEnabled = 0;
};


ch341::~ch341()
{
	Release();
}

void ch341::Close()
{
	libusb_close(devHandle);
	devHandle = 0;
}
#if 0
void ch341::sendData(const quint8 &data, size_t len)
{
	struct libusb_transfer *transfer = libusb_alloc_transfer(0);
	unsigned char *buffer = new unsigned char[len];
	memcpy(buffer, (const void *)data, len);

	libusb_fill_bulk_transfer(transfer, devHandle, CH341_DATA_OUT, buffer,
							  len, ch34x_callback, buffer, 0);

	libusb_submit_transfer(transfer);
}
#endif

void ch341::triggerBreak(uint msecs)
{
	SetBreakControl(1);

	if (breakTimer)
	{
		breakTimer->stop();
		delete breakTimer;
	}

	breakTimer = new QTimer(this);
	breakTimer->setSingleShot(true);
	connect(breakTimer, SIGNAL(timeout()), this, SLOT(breakTimeout()));
	breakTimer->start(msecs);
}


void ch341::breakTimeout()
{
	SetBreakControl(0);
}

#if 0
void ch341::allocTransfer()
{
	ctrl_transfer = libusb_alloc_transfer(0);
	// TODO is it right???
	libusb_fill_control_setup(ctrl_buf, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS |
							  LIBUSB_RECIPIENT_INTERFACE, CH341_REQ_WRITE_REG, CH341_REG_STAT, 0, 0);

	printf("go2: ctrlbuf0-7]=0x%X-%X-%X-%X-%X-%X-%X-%X\n",
		   ctrl_buf[0],
		   ctrl_buf[1],
		   ctrl_buf[2],
		   ctrl_buf[3],
		   ctrl_buf[4],
		   ctrl_buf[5],
		   ctrl_buf[6],
		   ctrl_buf[7]);

	libusb_fill_control_transfer(ctrl_transfer, devHandle, ctrl_buf,
								 cb_ctrl_changed, NULL, 0);

	ctrl_transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;
}
#endif

/**
 * when 'verbose' enabled, print debug information
 */
void ch341::v_print(int mode, int len)   // mode: begin=0, progress = 1
{
	static int size = 0;
	static time_t started, reported;
	int dur, done;

	if (!verbose)
	{
		return;
	}

	time_t now;
	time(&now);

	switch (mode)
	{
	case 0: // setup
		size = len;
		started = reported = now;
		break;

	case 1: // progress
		if (now == started)
		{
			return;
		}

		dur = now - started;
		done = size - len;

		if (done > 0 && reported != now)
		{
			qDebug("Bytes: %d (%d%c),  Time: %d, ETA: %d   ", done,
				   (done * 100) / size, '%', dur, (int)((1.0 * dur * size) / done - dur));
			fflush(stdout);
			reported = now;
		}

		break;

	case 2: // done
		dur = now - started;

		if (dur < 1)
		{
			dur = 1;
		}

		qDebug("Total:  %d sec,  average speed  %d  bytes per second.", dur, size / dur);
		break;

		break;
	}
}


qint32 ch341::GetStatusRx()
{
	return read_completed;
}


qint32 ch341::GetStatusTx()
{
	return write_completed;
}


qint32 ch341::Probe()
{
	qint32 ret;
	quint8 lcr = CH341_LCR_ENABLE_RX | CH341_LCR_ENABLE_TX | CH341_LCR_CS8;

	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR, lcr, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_LCR\n");
		return ret;
	}

	ret = SetBaudRate(baudRate);
	if (ret < 0)
	{
		return ret;
	}

	return 0;
}

#if 0

/* Helper function for libusb_bulk_transfer, display error message with the caller name */
qint32 ch341::Read(quint8 *buf, size_t len)
{
	qint32 ret;
	int transfered;
	if (devHandle == NULL)
	{
		return -1;
	}

	read_completed = 0;

	ret = libusb_bulk_transfer(devHandle, CONTROL_READ_ENDPOINT, buf, len, &transfered, timeout);
	if (ret < 0)
	{
		qCritical("libusb_bulk_transfer read error %d\n", ret);
		return -1;
	}
	return transfered;
}

/* Helper function for libusb_bulk_transfer, display error message with the caller name */
qint32 ch341::Write(quint8 *buf, size_t len)
{
	qint32 ret;
	int transfered;
	if (devHandle == NULL)
	{
		return -1;
	}

	write_completed = 0;

	ret = libusb_bulk_transfer(devHandle, CONTROL_WRITE_ENDPOINT, buf, len, &transfered, timeout);
	if (ret < 0)
	{
		qCritical("libusb_bulk_transfer write error %d\n", ret);
		return -1;
	}
	return transfered;
}
#endif
/**
 * @breif timeouts in milliseconds
 */
qint32 ch341::SetTimeouts(qint16 t)
{
	if (t >= 50 && t <= 2000)
	{
		timeout = t;
		return 0;
	}

	return -1;
}

void ch341::SetParity(quint8 p)
{
	parity = p;
}

void ch341::SetBits(quint8 b)
{
	bits = b;
}

void ch341::SetStops(quint8 s)
{
	stops = s;
}


// TODO
void ch341::SetFlowControl(quint8 f)
{
	flow_control = f;

	// check state
	quint8 r[LIBUSB_CONTROL_SETUP_SIZE];

	int ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &r[0], 0, timeout);
// to check buffer 0x9f, 0xee

	// TODO check this
	switch (flow_control)
	{
	case 0: // NONE
		libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_NONE, NULL, 0, timeout);

		break;

	case 1: // RTS_CTS
		libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_RTS_CTS, NULL, 0, timeout);

		break;

	case 2: // DSR_DTR
		libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_DSR_DTR, NULL, 0, timeout);

		break;
	}
}

void ch341::SetVerbose()
{
	verbose = true;
}

/**
 * ch341a modem connections:
 *
 * rs232 -> ch341  description
 *     1 -> 18     data carrier detect
 *     2 -> 6      Rx
 *     3 -> 5      Tx
 *     4 -> 20     Data Terminal Ready
 *     5           Signal Ground
 *     6 -> 16     Data Set Ready
 *     7 -> 21     Request To Send
 *     8 -> 15     Clear To Send
 *     9 -> 17     Ring Indicator
 */

/**
 * @brief configure DTR, RTS for device
 *
 */
qint32 ch341::setHandshakeByte(void)
{
// 		quint8 r[CH341_INPUT_BUF_SIZE];
// 		int ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_MODEM_CTRL, NULL, 0, &r[0], CH341_INPUT_BUF_SIZE, timeout);
// 		if (ret < 0)
// 		{
// 			qCritical("Faild to get handshake byte %d\n", ret);
// 			return -1;
// 		}

	quint8 control = 0;//r[0];

	if (dtr)
	{
		control |= CH341_CONTROL_DTR;
	}

	if (rts)
	{
		control |= CH341_CONTROL_RTS;
	}
#if DEBUG_CH341
	qDebug() << "set dtr " << ((control & CH341_CONTROL_DTR) ? 1 : 0) << "rts" << ((control & CH341_CONTROL_RTS) ? 1 : 0);
#endif
	if (libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_MODEM_CTRL, ~control, 0, NULL, 0, timeout) < 0)
	{
		qCritical("Faild to set handshake byte");
		return -1;
	}

	return 0;
}

qint32 ch341::getModemState(void)
{
	quint8 r[2];

	int ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &r[0], 2, timeout);
	if (ret < 0)
	{
		qCritical("Faild to get handshake byte %d", ret);
		return -1;
	}

	quint8 st = ~r[0];  // invert
#if DEBUG_CH341
	qDebug() << "get cts" << ((st & CH341_UART_CTS) ? 1 : 0) << "dsr" << ((st & CH341_UART_DSR) ? 1 : 0);
#endif
	return (qint32)(st & CH341_BITS_MODEM_STAT);
}

#if 0
int ch341::setAsync(quint8 data)
{
	int ret = 0;
	quint8 *buf = (quint8 *)malloc(LIBUSB_CONTROL_SETUP_SIZE + 1);

	if (!buf)
	{
		return -2;
	}

	transfer = libusb_alloc_transfer(0);
	if (!transfer)
	{
		free(buf);
		return -2;
	}

//     qCritical("async set mode %02x\n", data);

	libusb_fill_control_setup(buf, CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, 1);
	buf[LIBUSB_CONTROL_SETUP_SIZE] = data;

	libusb_fill_control_transfer(transfer, devHandle, buf, cb_ctrl_changed, NULL, timeout);

	transfer->flags = LIBUSB_TRANSFER_SHORT_NOT_OK | LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

	ret = libusb_submit_transfer(transfer);
	return ret;
}


void ch341::updateStatus()
{
	quint8 stat[CH341_INPUT_BUF_SIZE];
	quint8 status;
	quint8 delta;

	int ret; //libusb_control_transfer(devHandle, CONTROL_REQUEST_TYPE_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &stat[0], CH341_INPUT_BUF_SIZE, timeout);
	ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_MODEM_CTRL, NULL, 0, &stat[0], CH341_INPUT_BUF_SIZE, timeout);
	if (ret < 0)
	{
		qCritical("Faild to update status %d\n", ret);
		return;
	}

	status = ~stat[2] & CH341_BITS_MODEM_STAT;
	delta = msr ^ status;
	msr = status;

	if (delta & CH341_UART_CTS)
	{
		ctsState != ctsState;
	}

	if (delta & CH341_UART_DSR)
	{
		dsrState != dsrState;
	}

	if (delta & CH341_UART_RING)
	{
		ringState != ringState;
	}

	if (delta & CH341_UART_DCD)
	{
		dcdState != dcdState;
	}
}
#endif


qint32 ch341::SetDTR(qint32 state)
{
#if DEBUG_CH341
	qDebug() << "ch341::SetDTR(" << state << ") ";
#endif
	if (state)
	{
		dtr = 1;
	}
	else
	{
		dtr = 0;
	}
	qint32 r = setHandshakeByte();

	return r;
}

qint32 ch341::SetRTS(qint32 state)
{
#if DEBUG_CH341
	qDebug() << "ch341::SetRTS(" << state << ") ";
#endif
	if (state)
	{
		rts = 1;
	}
	else
	{
		rts = 0;
	}
	qint32 r = setHandshakeByte();

	return r;
}

qint32 ch341::GetDSR()
{
	qint32 r = getModemState();

#if DEBUG_CH341
	qDebug() << "ch341::GetDSR(" << ((r & CH341_UART_DSR) ? 1 : 0) << ") ";
#endif
	return (r & CH341_UART_DSR) ? 1 : 0;
}

qint32 ch341::GetCTS()
{
	qint32 r  = getModemState();

#if DEBUG_CH341
	qDebug() << "ch341::GetCTS(" << ((r & CH341_UART_CTS) ? 1 : 0) << ") ";
#endif
	return (r & CH341_UART_CTS) ? 1 : 0;
}

qint32 ch341::SetRTSDTR(int st)
{
	quint8 control = 0;

	if (st)
	{
		dtr = 1;
		rts = 1;
	}
	else
	{
		dtr = 0;
		rts = 0;
	}

#if 0
	if (dtr)
	{
		control |= CH341_CONTROL_DTR;
	}
	if (rts)
	{
		control |= CH341_CONTROL_RTS;
	}


	qDebug() << "ch341::SetRTSDTR(" << st << ") ";

	if (dev_vers < 0x20)
	{
		// TODO for old chips is not tested!
		if (libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, 0x0606, ~control, NULL, 0, timeout) < 0)
		{
			qCritical("Faild to set handshake byte\n");
			return -1;
		}
	}
	else
	{
		if (libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_MODEM_CTRL, ~control, 0, NULL, 0, timeout) < 0)
		{
			qCritical("Faild to set handshake byte\n");
			return -1;
		}
	}
#endif
	qint32 r = setHandshakeByte();

	return r;
}


// TODO to check this
qint32 ch341::SetConfigLCR()
{
	// TODO in case of same bits for all UART devices, to move this lcr into SerialInterface
	// now set LCR
	quint8 lcr =  CH341_LCR_ENABLE_RX | CH341_LCR_ENABLE_TX;

	// TODO check this
	switch (parity)
	{
	case 'N':
		// no change in lcr
		break;

	case 'O':
		lcr |= CH341_LCR_ENABLE_PAR;
		break;

	case 'S':
		lcr |= (CH341_LCR_ENABLE_PAR | CH341_LCR_MARK_SPACE | CH341_LCR_PAR_EVEN);
		break;

	case 'E':
		lcr |= (CH341_LCR_ENABLE_PAR | CH341_LCR_PAR_EVEN);
		break;

	case 'M':
		lcr |= (CH341_LCR_ENABLE_PAR | CH341_LCR_MARK_SPACE);
		break;

	default:
		break;
	}

	if (stops == 2)
	{
		lcr |= CH341_LCR_STOP_BITS_2;
	}

	if (bits <= 5)
	{
		lcr |= CH341_LCR_CS5;
	}
	else if (bits == 6)
	{
		lcr |= CH341_LCR_CS6;
	}
	else if (bits == 7)
	{
		lcr |= CH341_LCR_CS7;
	}
	else
	{
		lcr |= CH341_LCR_CS8;
	}

#if DEBUG_CH341
	qDebug() << "ch341::SetConfigLCR(" << lcr << ") ";
#endif
	qint32 ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR, lcr, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_LCR\n");
	}

	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL\n");
	}

	return ret;
}


qint32 ch341::SetBreakControl(qint32 state)
{

#if DEBUG_CH341
	qDebug() << "ch341::SetBreakControl(" << state << ") ";
#endif
	quint16 reg_contents;
	quint8 break_reg[2] = {0, 0};

	qint32 ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_READ_REG, CH341_REG_BREAK, 0, (quint8 *)&reg_contents, 2, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_READ_REG, CH341_REG_BREAK\n");
		return ret;
	}
	if (state)
	{
		break_reg[0] &= ~CH341_NBREAK_BITS_REG1;
		break_reg[1] &= ~CH341_NBREAK_BITS_REG2;
	}
	else
	{
		break_reg[0] |= CH341_NBREAK_BITS_REG1;
		break_reg[1] |= CH341_NBREAK_BITS_REG2;
	}
	// TODO is it right???
	//reg_contents = ReverseWord((quint16)(break_reg[0] << 8) + break_reg[1]);
	reg_contents = ((quint16)(break_reg[0] << 8) + break_reg[1]);

	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BREAK, reg_contents, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BREAK\n");
		return ret;
	}

	return 0;
}

/**
 * @brief functions sets the baudrate 'speed'
 *        see BSD Kernel: uchcom.c Source File
 *        supports common baud rates:
 *        50, 75, 100, 110, 134.5, 150, 300, 600, 900, 1200, 1800, 2400, 3600, 4800, 9600, 14400, 19200,
 *        33600, 38400, 56000, 57600, 76800, 115200, 128000, 153600, 230400, 460800, 921600,
 *        1500000, 2000000 etc.
 */
qint32 ch341::SetBaudRate(qint32 speed)
{
	qint32 ret;
	int dv_prescale;
	int dv_div;
	int dv_mod;

	static QVector<uart_div> speed_divider = {{307200, 307200, 0, {7, 0xD9, 0}},
		{921600, 921600, 0, {7, 0xF3, 0}},
		{2999999, 23530, 6000000, {3, 0, 0}},
		{23529, 2942, 750000, {2, 0, 0}},
		{2941, 368, 93750, {1, 0, 0}},
		{367, 1, 11719, {0, 0, 0}}
	};

	if (!speed)
	{
		return -1;
	}

	foreach (uart_div d, speed_divider)
	{
		if (d.dvr_high >= speed && d.dvr_low <= speed)
		{
			dv_prescale = d.dvr_divider.dv_prescaler;
			if (d.dvr_base_clock == 0)
			{
				dv_div = d.dvr_divider.dv_div;
			}
			else
			{
				quint32 div = d.dvr_base_clock / speed;
				quint32 rem = d.dvr_base_clock % speed;
				if (div == 0 || div >= 0xFF)
				{
					return -1;
				}

				if ((rem << 1) >= speed)
				{
					div += 1;
				}
				dv_div = (quint8) - div;
			}

			quint32 mod = (CH341_BPS_MOD_BASE / speed) + CH341_BPS_MOD_BASE_OFS;
			mod = mod + (mod / 2);

			dv_mod = (mod + 0xFF) / 0x100;

			// calculated, now send to device
			qDebug() << "set baudrate" << speed << (hex) << (dv_div << 8) + (dv_prescale) << (quint16)(dv_mod) << (dec) ;

			ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD1, (dv_div << 8) + (dv_prescale), NULL, 0, timeout);
			if (ret < 0)
			{
				qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD1\n");
				return ret;
			}

			ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD2, (quint16)(dv_mod), NULL, 0, timeout);
			if (ret < 0)
			{
				qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD2\n");
				return ret;
			}

			ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0, NULL, 0, timeout);
			if (ret < 0)
			{
				qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL\n");
			}

			return ret;
		}
	}
	// not found
	return -1;
}

/**
 * @brief reset chip settings?
 */
qint32 ch341::ResetChip()
{
	return libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_SERIAL_INIT, CH341_RESET_VALUE, CH341_RESET_INDEX, NULL, 0, timeout);
}


qint32 ch341::ClearChip()
{
//     return libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_SERIAL_INIT, 0, 0, NULL, 0, timeout);
	return libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_SERIAL_INIT, 0xc29c, 0xb2b9, NULL, 0, timeout);
}


/**
 * Open CH341A, find the device and set the default interface.
 */
qint32 ch341::Open(quint16 vid, quint16 pid)
{
	struct libusb_device *dev;
	qint32 ret;
	struct sigaction sa;

	qDebug() << "ch341::Open(" << vid << pid << ") ";

	quint8  desc[0x12];

	if (devHandle != NULL)
	{
		qCritical() << "Call ch341Release before re-configure";
		return -1;
	}

	qDebug() << "ch341::Configure()";
	ret = libusb_init(NULL);

	if (ret < 0)
	{
		qCritical() << "Couldn't initialise libusb";
		return -1;
	}

#if LIBUSB_API_VERSION < 0x01000106
	libusb_set_debug(NULL, 3);
#else
	libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
#endif

	if (!(devHandle = libusb_open_device_with_vid_pid(NULL, vid, pid)))
	{
		qCritical("Couldn't open device [%04x:%04x].", vid, pid);
		return -1;
	}
	else
	{
		qDebug("Open device [%04x:%04x].", vid, pid);
	}

	if (!(dev = libusb_get_device(devHandle)))
	{
		qCritical() << "Couldn't get bus number and address.";
		CloseHandle();
		return -1;
	}

#ifndef Q_OS_WIN32
	if (libusb_kernel_driver_active(devHandle, 0))
	{
		ret = libusb_detach_kernel_driver(devHandle, 0);

		if (ret)
		{
			qCritical("Failed to detach kernel driver: '%s'", strerror(-ret));
			CloseHandle();
			return -1;
		}
		else
		{
			qDebug() << "Detach kernel driver";
		}
	}
#endif
	ret = libusb_claim_interface(devHandle, 0);

	if (ret)
	{
		qCritical("Failed to claim interface 0: '%s'", strerror(-ret));
		CloseHandle();
		return -1;
	}
	else
	{
		qDebug() << "Claim interface 0";
	}

	ret = libusb_get_descriptor(devHandle, LIBUSB_DT_DEVICE, 0x00, desc, 0x12);

	if (ret < 0)
	{
		qCritical("Failed to get device descriptor: '%s'", strerror(-ret));
		ReleaseInterface();
		return -1;
	}

	qDebug("Device reported its revision [%d.%02d]", desc[12], desc[13]);
	sa.sa_handler = &sig_int;
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, &saold) == -1)
	{
		perror("Error: cannot handle SIGINT"); // Should not happen
	}

	ret = init();
	if (ret < 0)
	{
		qCritical("Failed to init device %d", strerror(-ret));
		ReleaseInterface();
		return -1;
	}

	return 0;
}

qint32 ch341::init(void)
{
	qint32 ret;

	quint8 buf[2];

	/* expect two bytes 0x27 0x00 */
	ret = libusb_control_transfer(devHandle, CTRL_IN, CH341_REQ_READ_VERSION, 0, 0, buf, 2, timeout);
	if (ret < 0)
	{
		qCritical("can not read version\n");
		return ret;
	}

#if DEBUG_CH341
	qDebug("Chip version: 0x%02x\n", buf[0]);
#endif
	dev_vers = buf[0];

	// send init to controller (reset)
	ret = ClearChip();
	if (ret < 0)
	{
		qCritical("failed control transfer 0xa1\n");
		return ret;
	}

	dtr = 1;
	ret = setHandshakeByte();
	if (ret < 0)
	{
		qCritical("failed set dtr\n");
		return ret;
	}

	rts = 1;
	ret = setHandshakeByte();
	if (ret < 0)
	{
		qCritical("failed set rts \n");
		return ret;
	}

	// check state not implemented


	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0x0000, NULL, 0, 1000);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL\n");
		return ret;
	}

	ret = SetBaudRate(baudRate);
	if (ret < 0)
	{
		return ret;
	}

	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR, 0x00c3, NULL, 0, 1000);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_LCR\n");
		return ret;
	}

	// check state not implemented

	ret = libusb_control_transfer(devHandle, CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0x0000, NULL, 0, 1000);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL\n");
		return ret;
	}

	return ret;

}

void ch341::ReleaseInterface(void)
{
	if (devHandle)
	{
		libusb_release_interface(devHandle, 0);
		CloseHandle();
	}
}


void ch341::CloseHandle(void)
{
	if (devHandle)
	{
		libusb_close(devHandle);
		devHandle = NULL;
	}
}

// void ch341::updateStatus(quint8 *data, size_t l)
// {
// }

/**
 * release libusb structure and ready to exit
 */
qint32 ch341::Release(void)
{
	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::Release()";

	libusb_release_interface(devHandle, 0);
	libusb_close(devHandle);
	libusb_exit(NULL);
	devHandle = NULL;
	sigaction(SIGINT, &saold, NULL);
	return 0;
}

#if 0
/**
 * Helper function for libusb_bulk_transfer, display error message with the caller name
 */
qint32 ch341::usbTransfer(const char *func, quint8 type, quint8 *buf, int len)
{
	qint32 ret;
	int transfered;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::usbTransfer()";

	ret = libusb_bulk_transfer(devHandle, type, buf, len, &transfered, timeout);

	if (ret < 0)
	{
		qCritical("%s: Failed to %s %d bytes '%s'", func,
				  (type == BULK_WRITE_ENDPOINT) ? "write" : "read", len, strerror(-ret));
		return -1;
	}

	return transfered;
}

// --------------------------------------------------------------------------
// ch341writeEEPROM()
//      write n bytes to 24c32/24c64 device (in packets of 32 bytes)
qint32 ch341::writeEEPROM(quint8 *buffer, quint32 bytesum)
{

	quint8 ch341outBuffer[EEPROM_WRITE_BUF_SZ], *outptr, *bufptr;
	qint32 ret = 0, i;
	quint16 byteoffset = 0, bytes = bytesum;
	quint8 addrbytecount = 3;  // 24c32 and 24c64 (and other 24c??) use 3 bytes for addressing
	qint32 actuallen = 0;

	bufptr = buffer;

	while (bytes)
	{
		outptr    = ch341outBuffer;
		*outptr++ = CH341_CMD_I2C_STREAM;
		*outptr++ = CH341_CMD_I2C_STM_STA;
		*outptr++ = CH341_CMD_I2C_STM_OUT + addrbytecount + MIN(bytes, 25);
		*outptr++ = 0xa0;                                   // EEPROM device address
		*outptr++ = (quint8)(byteoffset >> 8 & 0xff);      // MSB (big-endian) byte address
		*outptr++ = (quint8)(byteoffset & 0xff);           // LSB of 16-bit    byte address

		memcpy(outptr, bufptr, MIN(bytes, 25));             // payload has two parts: 25 bytes & up to 7 more bytes

		outptr += MIN(bytes, 25);
		bufptr += MIN(bytes, 25);
		bytes  -= MIN(bytes, 25);

		*outptr++ = CH341_CMD_I2C_STM_END;

		if (bytes)
		{
			*outptr++ = CH341_CMD_I2C_STREAM;
			*outptr++ = CH341_CMD_I2C_STM_OUT + MIN(bytes, 7);
			memcpy(outptr, bufptr, MIN(bytes, 7));

			outptr += MIN(bytes, 7);
			bufptr += MIN(bytes, 7);
			bytes  -= MIN(bytes, 7);
		}

		*outptr++ = CH341_CMD_I2C_STM_STO;
		*outptr   = CH341_CMD_I2C_STM_END;

		byteoffset += 0x20;

		for (i = 0; i < EEPROM_WRITE_BUF_SZ; i++)
		{
			if (!(i % 0x10))
			{
				qDebug("\n%04x : ", i);
			}
			qDebug("%02x ", ch341outBuffer[i]);
		}
		qDebug("\n");

		ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT,
								   ch341outBuffer, EEPROM_WRITE_BUF_SZ, &actuallen, timeout);

		if (ret < 0)
		{
			qCritical("Failed to write to EEPROM: '%s'\n", strerror(-ret));
			return -1;
		}

		qDebug("Writing [aa 5a 00] to EEPROM\n");    // Magic CH341a packet! Undocumented, unknown purpose

		outptr    = ch341outBuffer;
		*outptr++ = CH341_CMD_I2C_STREAM;
		*outptr++ = 0x5a;                           // what is this 0x5a??
		*outptr++ = CH341_CMD_I2C_STM_END;

		ret = libusb_bulk_transfer(devHandle, BULK_WRITE_ENDPOINT, ch341outBuffer, 3, &actuallen, timeout);

		if (ret < 0)
		{
			qCritical("Failed to write to EEPROM: '%s'\n", strerror(-ret));
			return -1;
		}
		qDebug("Written [%d] of [%d] bytes      \r", bytes, bytesum);
	}
	return 0;
}

// --------------------------------------------------------------------------
// ch341readEEPROM()
//      read n bytes from device (in packets of 32 bytes)
qint32 ch341::readEEPROM(quint8 *buffer, quint32 bytestoread)
{

	quint8 ch341outBuffer[EEPROM_READ_BULKOUT_BUF_SZ];
	quint8 ch341inBuffer[IN_BUF_SZ];               // 0x100 bytes
	qint32 ret = 0, readpktcount = 0;
	struct libusb_transfer *xferBulkIn, *xferBulkOut;
	struct timeval tv = {0, 100};                   // our async polling interval

	xferBulkIn  = libusb_alloc_transfer(0);
	xferBulkOut = libusb_alloc_transfer(0);

	bulkin_count = 0;

	if (!xferBulkIn || !xferBulkOut)
	{
		qCritical("Couldnt allocate USB transfer structures\n");
		return -1;
	}

	qDebug("Allocated USB transfer structures\n");

	memset(ch341inBuffer, 0, EEPROM_READ_BULKIN_BUF_SZ);
	memcpy(ch341outBuffer, CH341_EEPROM_READ_SETUP_CMD, EEPROM_READ_BULKOUT_BUF_SZ);

	libusb_fill_bulk_transfer(xferBulkIn,  devHandle, BULK_READ_ENDPOINT, ch341inBuffer,
							  EEPROM_READ_BULKIN_BUF_SZ, cbBulkIn, NULL, timeout);

	libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT,
							  ch341outBuffer, EEPROM_READ_BULKOUT_BUF_SZ, cbBulkOut, NULL, timeout);

	qDebug("Filled USB transfer structures\n");

	libusb_submit_transfer(xferBulkIn);
	qDebug("Submitted BULK IN start packet\n");

	libusb_submit_transfer(xferBulkOut);
	qDebug("Submitted BULK OUT setup packet\n");

//     readbuf = buffer;

	byteoffset = 0;

	while (1)
	{
		qDebug("Read [%d] of [%d] bytes      \r", byteoffset, bytestoread);
		ret = libusb_handle_events_timeout(NULL, &tv);

		if (ret < 0 || bulkin_count == -1)            // indicates an error
		{
			qCritical("ret from libusb_handle_timeout = %d\n", ret);
			qCritical("bulkin_count = %d\n", bulkin_count);
			if (ret < 0)
			{
				qCritical("USB read error : %s\n", strerror(-ret));
			}
			libusb_free_transfer(xferBulkIn);
			libusb_free_transfer(xferBulkOut);
			return -1;
		}
		if (bulkin_count)                        // callback function reports a new BULK IN packet received
		{
			bulkin_count = 0;                         //   reset the flag
			readpktcount++;                         //   increment the read packet counter
			byteoffset += EEPROM_READ_BULKIN_BUF_SZ;
			if (byteoffset == bytestoread)
			{
				break;
			}

			qDebug("\nRe-submitting transfer request to BULK IN endpoint\n");
			libusb_submit_transfer(xferBulkIn);     // re-submit request for next BULK IN packet of EEPROM data
			if (syncackpkt)
			{
				syncackpkt = 0;
			}
			// if 4th packet received, we are at end of 0x80 byte data block,
			// if it is not the last block, then resubmit request for data
			if (readpktcount == 4)
			{
				qDebug("\nSubmitting next transfer request to BULK OUT endpoint\n");
				readpktcount = 0;

				memcpy(ch341outBuffer, CH341_EEPROM_READ_NEXT_CMD, CH341_EEPROM_READ_CMD_SZ);
				ch341outBuffer[4] = (quint8)(byteoffset >> 8 & 0xff);      // MSB (big-endian) byte address
				ch341outBuffer[5] = (quint8)(byteoffset & 0xff);           // LSB of 16-bit    byte address

				libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, ch341outBuffer,
										  EEPROM_READ_BULKOUT_BUF_SZ, cbBulkOut, NULL, timeout);

				libusb_submit_transfer(xferBulkOut);// update transfer struct (with new EEPROM page offset)
				// and re-submit next transfer request to BULK OUT endpoint
			}
		}
	}

// out_deinit:
	libusb_free_transfer(xferBulkIn);
	libusb_free_transfer(xferBulkOut);
	return 0;
}


/**
 *   set the i2c bus speed speed(b1b0):
 *              0 = 20kHz   CH341_STM_I2C_20K
 *              1 = 100kHz  CH341_STM_I2C_100K
 *              2 = 400kHz  CH341_STM_I2C_400K
 *              3 = 750kHz  CH341_STM_I2C_750K
 *   set the spi bus data width speed(b2):
 *              0 = Single
 *              1 = Double  CH341_STM_SPI_DBL
 */
qint32 ch341::SetStream(quint32 speed)
{
	quint8 buf[3];

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::SetStream()";

	buf[0] = CH341_CMD_I2C_STREAM;
	buf[1] = CH341_CMD_I2C_STM_SET | (speed & 0x7);
	buf[2] = CH341_CMD_I2C_STM_END;

	return usbTransfer(__func__, BULK_WRITE_ENDPOINT, buf, 3);
}


/**
 * assert or deassert the chip-select pin of the spi device
 */
void ch341::SpiChipSelect(quint8 *ptr, bool selected)
{
	qDebug() << "ch341::SpiChipSelect()";

	*ptr++ = CH341_CMD_UIO_STREAM;
	*ptr++ = CH341_CMD_UIO_STM_OUT | (selected ? 0x36 : 0x37);

	if (selected)
	{
		*ptr++ = CH341_CMD_UIO_STM_DIR | 0x3F;    // pin direction
	}

	*ptr++ = CH341_CMD_UIO_STM_END;
}

/**
 * transfer len bytes of data to the spi device
 */
qint32 ch341::SpiStream(quint8 *out, quint8 *in, quint32 len)
{
	quint8 *inBuf, *outBuf, *inPtr, *outPtr;
	qint32 ret, packetLen;
	bool done;
	bool err = false;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::SpiStream()";

	outBuf = new quint8[CH341_PACKET_LENGTH];

	SpiChipSelect(outBuf, true);
	ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 4);

	if (ret < 0)
	{
		delete outBuf;
		return -1;
	}

	inBuf = new quint8[CH341_PACKET_LENGTH];

	inPtr = in;

	do
	{
		done = true;
		packetLen = len + 1; // STREAM COMMAND + data length

		if (packetLen > CH341_PACKET_LENGTH)
		{
			packetLen = CH341_PACKET_LENGTH;
			done = false;
		}

		outPtr = outBuf;
		*outPtr++ = CH341_CMD_SPI_STREAM;

		for (int i = 0; i < packetLen - 1; ++i)
		{
			*outPtr++ = ReverseByte(*out++);
		}

		ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, packetLen);

		if (ret < 0)
		{
			err = true;
			break;
		}

		ret = usbTransfer(__func__, BULK_READ_ENDPOINT, inBuf, packetLen - 1);

		if (ret < 0)
		{
			err = true;
			break;
		}

		len -= ret;

		for (int i = 0; i < ret; ++i)   // swap the buffer
		{
			*inPtr++ = ReverseByte(inBuf[i]);
		}
	}
	while (!done);

	if (!err)
	{
		SpiChipSelect(outBuf, false);
		ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);
	}
	else
	{
		ret = -1;
	}

	delete outBuf;
	delete inBuf;

	if (ret < 0)
	{
		return -1;
	}

	return 0;
}

#define JEDEC_ID_LEN 0x52    // additional byte due to SPI shift

/**
 * read the JEDEC ID of the SPI Flash
 */
qint32 ch341::SpiCapacity(void)
{
	quint8 *outBuf;
	quint8 *inBuf, *ptr, cap;
	qint32 ret;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::SpiCapacity()";

	outBuf = new quint8[JEDEC_ID_LEN];
	ptr = outBuf;
	*ptr++ = 0x9F; // Read JEDEC ID

	for (int i = 0; i < JEDEC_ID_LEN - 1; ++i)
	{
		*ptr++ = 0x00;
	}

	inBuf = new quint8[JEDEC_ID_LEN];

	ret = SpiStream(outBuf, inBuf, JEDEC_ID_LEN);

	if (ret < 0)
	{
		delete inBuf;
		delete outBuf;
		return ret;
	}

	if (!(inBuf[1] == 0xFF && inBuf[2] == 0xFF && inBuf[3] == 0xFF))
	{
		qDebug("Manufacturer ID: %02x", inBuf[1]);
		qDebug("Memory Type: %02x%02x", inBuf[2], inBuf[3]);

		if (inBuf[0x11] == 'Q' && inBuf[0x12] == 'R' && inBuf[0x13] == 'Y')
		{
			cap = inBuf[0x28];
			qDebug("Reading device capacity from CFI structure");
		}
		else
		{
			cap = inBuf[3];
			qDebug("No CFI structure found, trying to get capacity from device ID. Set manually if detection fails.");
		}

		qDebug("Capacity: %02x", cap);
	}
	else
	{
		qDebug("Chip not found or missed in ch341a. Check connection");
		delete inBuf;
		delete outBuf;
		exit(0);
	}

	delete inBuf;
	delete outBuf;

	return cap;
}


/**
 * read status register
 */
qint32 ch341::ReadStatus(void)
{
	quint8 out[2];
	quint8 in[2];
	qint32 ret;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::ReadStatus()";

	out[0] = 0x05; // Read status
	ret = SpiStream(out, in, 2);

	if (ret < 0)
	{
		return ret;
	}

	return (in[1]);
}

/**
 * write status register
 */
qint32 ch341::WriteStatus(quint8 status)
{
	quint8 out[2];
	quint8 in[2];
	qint32 ret;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::WriteStatus()";

	out[0] = 0x06; // Write enable
	ret = SpiStream(out, in, 1);

	if (ret < 0)
	{
		return ret;
	}

	out[0] = 0x01; // Write status
	out[1] = status;
	ret = SpiStream(out, in, 2);

	if (ret < 0)
	{
		return ret;
	}

	out[0] = 0x04; // Write disable
	ret = SpiStream(out, in, 1);

	if (ret < 0)
	{
		return ret;
	}

	return 0;
}

/**
 * chip erase
 */
qint32 ch341::EraseChip(void)
{
	quint8 out[1];
	quint8 in[1];
	qint32 ret;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::EraseChip()";

	out[0] = 0x06; // Write enable
	ret = SpiStream(out, in, 1);

	if (ret < 0)
	{
		return ret;
	}

	out[0] = 0xC7; // Chip erase
	ret = SpiStream(out, in, 1);

	if (ret < 0)
	{
		return ret;
	}

	out[0] = 0x04; // Write disable
	ret = SpiStream(out, in, 1);

	if (ret < 0)
	{
		return ret;
	}

	return 0;
}

/**
 * read the content of SPI device to buf, make sure the buf is big enough before call
 */
qint32 ch341::SpiRead(quint8 *buf, quint32 add, quint32 len)
{
	quint8 *outBuf;
	quint8 *inBuf;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::SpiRead()";

	/* what subtracted is: 1. first cs package, 2. leading command for every other packages,
	 * 3. second package contains read flash command and 3 bytes address */
	const quint32 max_payload = CH341_MAX_PACKET_LEN - CH341_PACKET_LENGTH - CH341_MAX_PACKETS + 1 - 4;
	quint32 tmp, pkg_len, pkg_count;
	struct libusb_transfer *xferBulkIn, *xferBulkOut;
	quint32 idx = 0;
	qint32 ret;
	qint32 old_counter;
	struct timeval tv = {0, 100};
	v_print(0, len);  // verbose

	outBuf = new quint8[CH341_MAX_PACKET_LEN];

	memset(outBuf, 0xff, CH341_MAX_PACKET_LEN);

	for (int i = 1; i < CH341_MAX_PACKETS; ++i)   // fill CH341_CMD_SPI_STREAM for every packet
	{
		outBuf[i * CH341_PACKET_LENGTH] = CH341_CMD_SPI_STREAM;
	}

	inBuf = new quint8[CH341_PACKET_LENGTH];

	memset(inBuf, 0x00, CH341_PACKET_LENGTH);
	xferBulkIn  = libusb_alloc_transfer(0);
	xferBulkOut = libusb_alloc_transfer(0);

	qDebug("Read started!");

	while (len > 0)
	{
		v_print(1, len);  // verbose
		fflush(stdout);
		SpiChipSelect(outBuf, true);
		idx = CH341_PACKET_LENGTH + 1;
		outBuf[idx++] = 0xC0; // byte swapped command for Flash Read
		tmp = add;

		for (int i = 0; i < 3; ++i)   // starting address of next read
		{
			outBuf[idx++] = ReverseByte((tmp >> 16) & 0xFF);
			tmp <<= 8;
		}

		if (len > max_payload)
		{
			pkg_len = CH341_MAX_PACKET_LEN;
			pkg_count = CH341_MAX_PACKETS - 1;
			len -= max_payload;
			add += max_payload;
		}
		else
		{
			pkg_count = (len + 4) / (CH341_PACKET_LENGTH - 1);

			if ((len + 4) % (CH341_PACKET_LENGTH - 1))
			{
				pkg_count ++;
			}

			pkg_len = (pkg_count) * CH341_PACKET_LENGTH + ((len + 4) % (CH341_PACKET_LENGTH - 1)) + 1;
			len = 0;
		}

		bulkin_count = 0;
		libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, inBuf,
								  CH341_PACKET_LENGTH, cbBulkIn, buf, timeout);
		buf += max_payload; // advance user's pointer
		libusb_submit_transfer(xferBulkIn);
		libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, outBuf,
								  pkg_len, cbBulkOut, NULL, timeout);
		libusb_submit_transfer(xferBulkOut);
		old_counter = bulkin_count;

		while (bulkin_count < pkg_count)
		{
			libusb_handle_events_timeout(NULL, &tv);

			if (bulkin_count == -1)   // encountered error
			{
				len = 0;
				ret = -1;
				break;
			}

			if (old_counter != bulkin_count)   // new package came
			{
				if (bulkin_count != pkg_count)
				{
					libusb_submit_transfer(xferBulkIn);    // resubmit bulk in request
				}

				old_counter = bulkin_count;
			}
		}

		SpiChipSelect(outBuf, false);
		ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);

		if (ret < 0)
		{
			break;
		}

		if (force_stop == 1)   // user hit ctrl+C
		{
			force_stop = 0;

			if (len > 0)
			{
				qCritical() << "User hit Ctrl+C, reading unfinished.";
			}

			break;
		}
	}

	delete outBuf;
	delete inBuf;

	libusb_free_transfer(xferBulkIn);
	libusb_free_transfer(xferBulkOut);
	v_print(2, 0);
	return ret;
}

#define WRITE_PAYLOAD_LENGTH 301 // 301 is the length of a page(256)'s data with protocol overhead

/**
 * write buffer(*buf) to SPI flash
 */
qint32 ch341::SpiWrite(quint8 *buf, quint32 add, quint32 len)
{
	quint8 *outBuf;
	quint8 *inBuf;
	quint32 tmp, pkg_count;
	struct libusb_transfer *xferBulkIn, *xferBulkOut;
	quint32 idx = 0;
	quint32 ret;
	qint32 old_counter;
	struct timeval tv = {0, 100};

	v_print(0, len); // verbose

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::SpiWrite()";

	outBuf = new quint8[WRITE_PAYLOAD_LENGTH];

	memset(outBuf, 0xff, WRITE_PAYLOAD_LENGTH);
	xferBulkIn  = libusb_alloc_transfer(0);
	xferBulkOut = libusb_alloc_transfer(0);

	inBuf = new quint8[CH341_PACKET_LENGTH];

	qDebug("Write started!");

	while (len > 0)
	{
		v_print(1, len);

		outBuf[0] = 0x06; // Write enable
		ret = SpiStream(outBuf, inBuf, 1);
		SpiChipSelect(outBuf, true);
		idx = CH341_PACKET_LENGTH;
		outBuf[idx++] = CH341_CMD_SPI_STREAM;
		outBuf[idx++] = 0x40; // byte swapped command for Flash Page Write
		tmp = add;

		for (int i = 0; i < 3; ++i)   // starting address of next write
		{
			outBuf[idx++] = ReverseByte((tmp >> 16) & 0xFF);
			tmp <<= 8;
		}

		tmp = 0;
		pkg_count = 1;

		while ((idx < WRITE_PAYLOAD_LENGTH) && (len > tmp))
		{
			if (idx % CH341_PACKET_LENGTH == 0)
			{
				outBuf[idx++] = CH341_CMD_SPI_STREAM;
				pkg_count ++;
			}
			else
			{
				outBuf[idx++] = ReverseByte(*buf++);
				tmp++;

				if (((add + tmp) & 0xFF) == 0)   // cross page boundary
				{
					break;
				}
			}
		}

		len -= tmp;
		add += tmp;
		bulkin_count = 0;
		libusb_fill_bulk_transfer(xferBulkIn, devHandle, BULK_READ_ENDPOINT, inBuf,
								  CH341_PACKET_LENGTH, cbBulkIn, NULL, timeout);
		libusb_submit_transfer(xferBulkIn);
		libusb_fill_bulk_transfer(xferBulkOut, devHandle, BULK_WRITE_ENDPOINT, outBuf,
								  idx, cbBulkOut, NULL, timeout);
		libusb_submit_transfer(xferBulkOut);
		old_counter = bulkin_count;
		ret = 0;

		while (bulkin_count < pkg_count)
		{
			libusb_handle_events_timeout(NULL, &tv);

			if (bulkin_count == -1)   // encountered error
			{
				ret = -1;
				break;
			}

			if (old_counter != bulkin_count)   // new package came
			{
				if (bulkin_count != pkg_count)
				{
					libusb_submit_transfer(xferBulkIn);    // resubmit bulk in request
				}

				old_counter = bulkin_count;
			}
		}

		if (ret < 0)
		{
			break;
		}

		SpiChipSelect(outBuf, false);
		ret = usbTransfer(__func__, BULK_WRITE_ENDPOINT, outBuf, 3);

		if (ret < 0)
		{
			break;
		}

		outBuf[0] = 0x04; // Write disable
		ret = SpiStream(outBuf, inBuf, 1);

		do
		{
			ret = ReadStatus();

			if (ret != 0)
			{
				libusb_handle_events_timeout(NULL, &tv);
			}
		}
		while (ret != 0);

		if (force_stop == 1)   // user hit ctrl+C
		{
			force_stop = 0;

			if (len > 0)
			{
				qCritical() << "User hit Ctrl+C, writing unfinished.";
			}

			break;
		}
	}

	libusb_free_transfer(xferBulkIn);
	libusb_free_transfer(xferBulkOut);

	delete outBuf;
	delete inBuf;

	v_print(2, 0);
	return ret;
}
#endif
