//=========================================================================//
//                                                                         //
//  PonyProg - Serial Device Programmer                                    //
//                                                                         //
//  Copyright (C) 1997-2019   Claudio Lanconelli                           //
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
//  Copyright (c) USBIO source code, examples:                             //
//  http://usb-i2c-spi.com/down.htm                                        //
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

int32_t bulkin_count; // set by the callback function

#define DEBUG_CH341 0

struct sigaction saold;
int force_stop = 0;
uint32_t syncackpkt;  // synch / ack flag used by BULK OUT cb function
uint16_t byteoffset;  // for read eeprom function

int16_t ch341::read_completed = 0;
int16_t ch341::write_completed = 0;


/* SIGINT handler */
void sig_int(int signo)
{
	force_stop = 1;
}

/**
 * ch341 requres LSB first, reverse the bit order before send and after receive
 * for more tricks, see https://graphics.stanford.edu/~seander/bithacks.html
 */



/**
 * callback for bulk out async transfer
 */
void cbBulkOut(struct libusb_transfer *transfer)
{
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		qCritical("cbBulkOut() error : %d\n", transfer->status);
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
				*((uint8_t *)transfer->user_data++) = USB_Interface::ReverseByte(transfer->buffer[i]);
			}
		}

		bulkin_count++;
		break;

	default:
		qCritical("cbBulkIn() error : %d\n", transfer->status);
		libusb_free_transfer(transfer);
		bulkin_count = -1;
	}

	return;
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

ch341::ch341()
	: USB_Interface()
{
}

// ch341::ch341(usb_dev *p)
// {
// 	dtr = 0;
// 	rts = 0;
//
// 	baudRate = DEFAULT_BAUD_RATE;
//
// 	parity = 'N';
// 	bits = 8;
// 	stops = 1;
//
//     flow_control = 0;
//
// 	timeout = DEFAULT_TIMEOUT;
//
// 	devHandle = NULL;
//
// 	rtsCtsEnabled = 0;
// 	dtrDsrEnabled = 0;
// };
//
//
// ch341::~ch341()
// {
// 	Release();
// }


int32_t ch341::getState()
{
	return getModemState();
}

int32_t ch341::setControl()
{
	return setHandshakeByte();
}

/**
 * @brief
 */
int32_t ch341::SetMode(uint16_t mode)
{
	int res = -1;

	ChipMode = mode;

	switch (mode)
	{
	case USB_MODE_UART:
		res = initUART();
		break;

	case USB_MODE_I2C:
		res = initI2C();
		break;

	case USB_MODE_SPI:
		res = initSPI();
		break;

	default:
		break;
	}

	return res;
}


void ch341::Close()
{
	libusb_close(devHandle);
	devHandle = 0;
}

#if 0
void ch341::sendData(const uint8_t &data, size_t len)
{
	struct libusb_transfer *transfer = libusb_alloc_transfer(0);
	uchar *buffer = new uchar[len];
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


int32_t ch341::GetStatusRx()
{
	// TODO
	return read_completed;
}


int32_t ch341::GetStatusTx()
{
	return write_completed;
}


int32_t ch341::Probe()
{
	if (devHandle == 0)
	{
		return -1;
	}

	int32_t ret;
	uint8_t lcr = CH341_LCR_ENABLE_RX | CH341_LCR_ENABLE_TX | CH341_LCR_CS8;

	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR_SET, lcr, NULL, 0, timeout);
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


/**
 * @breif timeouts in milliseconds
 */
int32_t ch341::SetTimeouts(int16_t t)
{
	if (t >= 50 && t <= 2000)
	{
		timeout = t;
		return 0;
	}

	return -1;
}

void ch341::SetParity(uint8_t p)
{
	parity = p;
}

void ch341::SetBits(uint8_t b)
{
	bits = b;
}

void ch341::SetStops(uint8_t s)
{
	stops = s;
}


// TODO
int32_t ch341::SetFlowControl(uint8_t f)
{
	flow_control = f;

	if (devHandle == 0)
	{
		return -1;
	}
	// check state
	uint8_t r[LIBUSB_CONTROL_SETUP_SIZE];

	int res = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &r[0], 0, timeout);
// to check buffer 0x9f, 0xee
	if (res < 0)
	{
		qCritical("SetFlowControl() failed to get chip control information!\n");
	}

	// TODO check this
	switch (flow_control)
	{
	case 0: // NONE
		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_NONE, NULL, 0, timeout);

		break;

	case 1: // RTS_CTS
		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_RTS_CTS, NULL, 0, timeout);

		break;

	case 2: // DSR_DTR
		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, CH34X_FLOW_CONTROL_DSR_DTR, NULL, 0, timeout);

		break;
	}

	if (res < 0)
	{
		qCritical("SetFlowControl() failed to set chip control information!\n");
	}
	return res;
}


/**
 * @brief  configure DTR, RTS for device, depended from dtr and rts class variables
 *
 * @return if successful = 0
 */
int32_t ch341::setHandshakeByte(void)
{
	if (devHandle == 0)
	{
		return -1;
	}
// 		uint8_t r[CH341_INPUT_BUF_SIZE];
// 		int ret = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_MODEM_CTRL, NULL, 0, &r[0], CH341_INPUT_BUF_SIZE, timeout);
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
	if (libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_MODEM_CTRL, ~control, 0, NULL, 0, timeout) < 0)
	{
// 		qCritical("Failed to set handshake byte\n");
		return -1;
	}

	return 0;
}

/**
 * @brief get the current modem state
 *
 * @return state of modem, bits 0..4 only
 */
int32_t ch341::getModemState(void)
{
	uint8_t r[2];
	if (devHandle == 0)
	{
		return -1;
	}

	int ret = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &r[0], 2, timeout);
	if (ret < 0)
	{
		qCritical("Failed to get handshake byte %d", ret);
		return -1;
	}

	uint8_t st = ~r[0];  // invert
#if DEBUG_CH341
	qDebug() << "get cts" << ((st & CH341_UART_CTS) ? 1 : 0) << "dsr" << ((st & CH341_UART_DSR) ? 1 : 0);
#endif
	return (int32_t)(st & CH341_BITS_MODEM_STAT);
}

#if 0
int ch341::setAsync(uint8_t data)
{
	int ret = 0;
	uint8_t *buf = (uint8_t *)malloc(LIBUSB_CONTROL_SETUP_SIZE + 1);

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

	libusb_fill_control_setup(buf, CH341_CTRL_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, 1);
	buf[LIBUSB_CONTROL_SETUP_SIZE] = data;

	libusb_fill_control_transfer(transfer, devHandle, buf, cb_ctrl_changed, NULL, timeout);

	transfer->flags = LIBUSB_TRANSFER_SHORT_NOT_OK | LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER;

	ret = libusb_submit_transfer(transfer);
	return ret;
}


void ch341::updateStatus()
{
	uint8_t stat[CH341_INPUT_BUF_SIZE];
	uint8_t status;
	uint8_t delta;

	int ret; //libusb_control_transfer(devHandle, CONTROL_REQUEST_TYPE_IN, CH341_REQ_READ_REG, CH341_REG_STAT, 0, &stat[0], CH341_INPUT_BUF_SIZE, timeout);
	ret = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_MODEM_CTRL, NULL, 0, &stat[0], CH341_INPUT_BUF_SIZE, timeout);
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

/**
 * @brief set or reset of DTR and send changes to chip
 *
 * @param[in] state 0 for reset DTR, not 0 for set DTR
 * @return if successful 0
 */
int32_t ch341::SetDTR(int32_t state)
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
	int32_t r = setHandshakeByte();

	return r;
}

/**
 * @brief set or reset of RTS and send changes to chip
 *
 * @param[in] state 0 for reset RTS, not 0 for set RTS
 * @return if successful 0
 */
int32_t ch341::SetRTS(int32_t state)
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
	int32_t r = setHandshakeByte();

	return r;
}

/**
 * @brief get the current state of DSR
 *
 * @return 1 if DSR set, in other case 0
 */
int32_t ch341::GetDSR()
{
	int32_t r = getModemState();

#if DEBUG_CH341
	qDebug() << "ch341::GetDSR(" << ((r & CH341_UART_DSR) ? 1 : 0) << ") ";
#endif
	return (r & CH341_UART_DSR) ? 1 : 0;
}


/**
 * @brief get the current state of CTS
 *
 * @return 1 if CTS set, in other case 0
 */
int32_t ch341::GetCTS()
{
	int32_t r  = getModemState();

#if DEBUG_CH341
	qDebug() << "ch341::GetCTS(" << ((r & CH341_UART_CTS) ? 1 : 0) << ") ";
#endif
	return (r & CH341_UART_CTS) ? 1 : 0;
}


/**
 * @brief set or reset of RTS, DTR and send changes to chip
 *
 * @param[in] state 0 for reset RTS, DTR, not 0 for set RTS, DTR
 * @return if successful 0
 */
int32_t ch341::SetRTSDTR(int st)
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

#if DEBUG_CH341
	qDebug() << "ch341::SetRTSDTR(" << st << ") ";
#endif

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
		if (libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, 0x0606, ~control, NULL, 0, timeout) < 0)
		{
			qCritical("Faild to set handshake byte\n");
			return -1;
		}
	}
	else
	{
		if (libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_MODEM_CTRL, ~control, 0, NULL, 0, timeout) < 0)
		{
			qCritical("Faild to set handshake byte\n");
			return -1;
		}
	}
#endif
	int32_t r = setHandshakeByte();

	return r;
}


/**
 * @brief Helper function for libusb_bulk_transfer, display error message with the caller name
 *
 * @param[out] buf pointer to data array for reading
 * @param[in] len number of bytes to read
 *
 * @return number of received bytes
 */
int32_t ch341::Read(uchar *buf, size_t len)
{
	int32_t res;
	int received;
	if (devHandle == NULL)
	{
		return -1;
	}

	read_completed = 0;

	res = libusb_bulk_transfer(devHandle, CH341_DATA_IN, (uchar *)&buf, len, &received, timeout);
	if (res < 0)
	{
// 		qCritical("libusb_bulk_transfer read error %d\n", res);
		return -1;
	}
	return received;
}

/**
 * @brief Helper function for libusb_bulk_transfer, display error message with the caller name
 *
 * @param[in] buf pointer to data array for writing
 * @param[in] len number of bytes to write
 *
 * @return number of transferred bytes
 */
int32_t ch341::Write(uchar *buf, size_t len)
{
	int32_t res;
	int transfered;
	if (devHandle == NULL)
	{
		return -1;
	}

	write_completed = 0;

	res = libusb_bulk_transfer(devHandle, CH341_DATA_OUT, (uchar *)&buf, len, &transfered, timeout);
	if (res < 0)
	{
// 		qCritical("libusb_bulk_transfer write error %d\n", res);
		return -1;
	}
	return transfered;
}


/**
 * @brief Execute the data stream command, first output and then input
 *
 * @param[in] wLength length to be written
 * @param[in] wBuffer  pointer to a buffer, data to be written
 * @param[in] readStep The length of the single block to be read, the total length to be read is (iReadStep*iReadTimes)
 * @param[in] readTimes number of times to prepare for reading
 * @param[out] rLength pointer to the length unit, and returns to the actual read length
 * @param[out] rBuffer Pointer to a buffer large enough to hold the read data
 *
 * @return 0 if successful
 */
int32_t ch341::WriteRead(uint wLength, uchar *wBuffer, uint readStep, uint readTimes, uint *rLength, uchar *rBuffer)
{
	uchar local_buf[CH341_PACKET_LENGTH];
	uint k = 0;
	uint res;
	uint 	m_length, m_rdlen;

#if DEBUG_CH341
	qDebug() << "ch341::WriteRead(" << wLength << readStep *readTimes << ") ";
#endif

	m_rdlen = readStep * readTimes;

	if (m_rdlen == 0)
	{
		return -1;
	}

	if (devHandle == 0)
	{
		return -1;
	}

	m_length = MAX(wLength, m_rdlen);

	res = WriteData((uint *)&wBuffer, &m_length);
	if (res < 0)
	{
		return res;
	}

	do
	{
		int len = readStep;
		res = libusb_bulk_transfer(devHandle, CH341_DATA_IN, (uchar *)local_buf, len, &len, timeout);  //64: Max Packet Length usb_bulk_read( usb_dev[ index ], sUSBIO[ index ].in_ep, local_buf, readStep, timeout );
		if (res < 0)
		{
			return res;
		}
		memcpy(rBuffer, local_buf, len);
		rBuffer += len;
		k += len;
	}
	while (--readTimes);

	*rLength = k;

	return 0;
}


/**
 * @breif Direct input of data and status via CH341, higher efficiency than CH341GetStatus
 *
 * Bit 7-bit 0 corresponds to the D7-D0 pin of CH341
 * Bit 8 corresponds to the ERR#pin of CH341
 * bit 9 corresponds to the pemp pin of CH341
 * bit 10 corresponds to the INT#pin of CH341
 * bit 11 corresponds to the SLCT pin of CH341
 * bit 12 corresponds to the SDA pin of CH341
 * Bit 13 corresponds to the BUSY / WAIT#pin of CH341
 * bit 14 corresponds to the AUTOFD# / DATAS # pin of CH341
 * bit 15 corresponds to the SLCTIN#/ADDRS # pin of CH341
 */
int32_t ch341::GetInput(uint *iStatus)
{
	int res;
	uchar buf[0x28] = {0};

#if DEBUG_CH341
	qDebug() << "ch341::GetInput() ";
#endif

	if (dev_vers < 0x20)
	{
		res = GetStatus(iStatus);
	}
	else
	{
		int len = 0x0;
		buf[0] = CH341_CMD_GET_INPUT;
		res = WriteRead(1, buf, 0x20, 1, (uint *)&len, buf);
		// TODO chek the len!!!
		if (res >= 0)
		{
			*iStatus = (((uint)buf[2] & 0x80) << 8 | ((uint)buf[1] & 0xef)) << 8 | (uint)buf[0];
		}
		else
		{
			qCritical("GetInput() write error %d\n", res);
		}
	}

	return res;
}


/**
 * @brief Direct input of data and status via CH341
 * Bit 7-bit 0 corresponds to the D7-D0 pin of CH341
 * Bit 8 corresponds to the ERR#pin of CH341,
 * bit 9 corresponds to the pemp pin of CH341,
 * bit 10 corresponds to the INT#pin of CH341,
 * bit 11 corresponds to the SLCT pin of CH341,
 * bit 12 corresponds to the SDA pin of CH341
 * Bit 13 corresponds to the BUSY / WAIT#pin of CH341
 * bit 14 corresponds to the AUTOFD# / DATAS # pin of CH341
 * bit 15 corresponds to the SLCTIN#/ADDRS # pin of CH341
 */
int32_t ch341::GetStatus(uint *iStatus)
{
	uchar buf[0x28] = {0};
	uint len;
#if DEBUG_CH341
	qDebug() << "ch341::GetStatus() ";
#endif
	buf[0] = CH341_GET_STATUS; // command
	buf[4] = 8; // data len write
	buf[8] = 0xc0;
	buf[9] = 0x52;
	buf[14] = 8; // data len read

	len = 0x28;

	int res = WriteRead(0x28, buf, 0x20, 1, (uint *)&len, buf);
	if (res >= 0)
	{
		*iStatus = (((uint)(buf[9] & 0xef) | ((uint)(uchar)buf[10] & 0x80) << 8) << 8) | (uint)buf[8];
	}
	else
	{
		qCritical("GetStatus write error %d\n", res);
	}

	return res;
}


/**
 * @brief Handle I2C data stream, 2-wire interface, clock line is SCL pin, data line is SDA pin (quasi-bidirectional I/O), speed is about 56K
 * This function issue a set of packets of iWriteBuffer data
 * arg:
 * @param[in] iWriteLength : should write the length of data
 * @param[in] iWriteBuffer : input buffer
 * @param[out] oReadLength  : should read the length of data
 * @param[out] oReadBuffer  : output buffer
 */
int32_t ch341::StreamI2C(uint iWriteLength, uint *iWriteBuffer, uint iReadLength, uint *oReadBuffer)
{
	uchar mBuffer[MAX_BUFFER_LENGTH];
	uint32_t i, j, mLength;
	uchar *mWrBuf;
	int res = -1;
#if DEBUG_CH341
	qDebug() << "ch341::StreamI2C(" << iWriteLength << iReadLength << ") ";
#endif
	if (dev_vers < 0x20)
	{
		qCritical("StreamI2C() device version is too old!\n");
		return -1;
	}

	mLength = MAX(iWriteLength, iReadLength);
	if (mLength > MAX_BUFFER_LENGTH)
	{
		qCritical("StreamI2C() length is bigger than %d!\n", MAX_BUFFER_LENGTH);
		return -1;
	}
	if (mLength <= DEFAULT_BUFFER_LEN)
	{
		mWrBuf = (uchar *)mBuffer;
	}
	else
	{
		mWrBuf = new uchar[MAX_BUFFER_LENGTH];
		if (mWrBuf == NULL)
		{
			qCritical("StreamI2C() new() troubles!\n");
			return -1;
		}
	}
	i = 0;
	mWrBuf[i++] = CH341_CMD_I2C_STREAM;
	if ((StreamMode & 0x03) == 0)
	{
		mWrBuf[i++] = CH341_CMD_I2C_STM_US | 10;
		mWrBuf[i++] = CH341_CMD_I2C_STM_US | 10;
	}

	mWrBuf[i++] = CH341_CMD_I2C_STM_STA;
	if (iWriteLength)
	{
		for (j = 0; j < iWriteLength; j++)
		{
			mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
			if (mLength <= 2)
			{
				while (mLength--)
				{
					mWrBuf[i++] = CH341_CMD_I2C_STM_END;
				}
				mLength = CH341_PACKET_LENGTH;
			}
			if (mLength >= CH341_PACKET_LENGTH)
			{
				mWrBuf[i++] = CH341_CMD_I2C_STREAM;
				mLength = CH341_PACKET_LENGTH - 1;
			}
			mLength--;
			mLength--;
			if (mLength > iWriteLength - j)
			{
				mLength = iWriteLength - j;
			}
			mWrBuf[i++] = (uchar)(CH341_CMD_I2C_STM_OUT | mLength);
			while (mLength--)
			{
				mWrBuf[i++] = *((uchar *)iWriteBuffer + j++);
			}
		}

	}
	if (iReadLength)
	{
		mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
		if (mLength <= 3)
		{
			while (mLength--)
			{
				mWrBuf[i++] = CH341_CMD_I2C_STM_END;
			}
			mLength = CH341_PACKET_LENGTH;
		}
		if (mLength >= CH341_PACKET_LENGTH)
		{
			mWrBuf[i++] = CH341_CMD_I2C_STREAM;
		}
		if (iWriteLength > 1)
		{
			mWrBuf[i++] = CH341_CMD_I2C_STM_STA;
			mWrBuf[i++] = (uchar)(CH341_CMD_I2C_STM_OUT | 1);
			mWrBuf[i++] = *(uchar *)iWriteBuffer | 0x01;
		}
		else if (iWriteLength)
		{
			i--;
			mWrBuf[i++] = *(uchar *)iWriteBuffer | 0x01;
		}
		for (j = 1; j < iReadLength;)
		{
			mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
			if (mLength <= 1)
			{
				if (mLength)
				{
					mWrBuf[i++] = CH341_CMD_I2C_STM_END;
				}
				mLength = CH341_PACKET_LENGTH;
			}
			if (mLength >= CH341_PACKET_LENGTH)
			{
				mWrBuf[i++] = CH341_CMD_I2C_STREAM;
			}
			mLength = ((iReadLength - j) >= CH341_PACKET_LENGTH) ? CH341_PACKET_LENGTH : iReadLength - j;
			mWrBuf[i++] = (uchar)(CH341_CMD_I2C_STM_IN | mLength);
			j += mLength;
			if (mLength >= CH341_PACKET_LENGTH)
			{
				mWrBuf[i] = CH341_CMD_I2C_STM_END;
				i += CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
			}
		}
		mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
		if (mLength <= 1)
		{
			if (mLength)
			{
				mWrBuf[i++] = CH341_CMD_I2C_STM_END;
			}
			mLength = CH341_PACKET_LENGTH;
		}
		if (mLength >= CH341_PACKET_LENGTH)
		{
			mWrBuf[i++] = CH341_CMD_I2C_STREAM;
		}
		mWrBuf[i++] = CH341_CMD_I2C_STM_IN;
	}

	mLength = CH341_PACKET_LENGTH - i % CH341_PACKET_LENGTH;
	if (mLength <= 1)
	{
		if (mLength)
		{
			mWrBuf[i++] = CH341_CMD_I2C_STM_END;
		}
		mLength = CH341_PACKET_LENGTH;
	}

	if (mLength >= CH341_PACKET_LENGTH)
	{
		mWrBuf[i++] = CH341_CMD_I2C_STREAM;
	}

	mWrBuf[i++] = CH341_CMD_I2C_STM_STO;
	mWrBuf[i++] = CH341_CMD_I2C_STM_END;
	mLength = 0;
	if (iReadLength)
	{
		mWrBuf[i] = CH341_PACKET_LENGTH;
		mWrBuf[i + 4] = (iReadLength + CH341_PACKET_LENGTH - 1) / CH341_PACKET_LENGTH;
		i = i + 8;
	}

	if (iReadLength)
	{
		res = WriteRead(i, mWrBuf, 0x20, (iReadLength + 0x1f) >> 5, &mLength, (uchar *)&oReadBuffer);
		if (mLength != iReadLength)
		{
			qCritical("Return length is not equal to input length\n");
			res = -1;
		}
	}
	else
	{
		res = WriteData((uint *)&mWrBuf, &i);
	}

	if (res < 0)
	{
		qCritical("StreamI2C() error %d\n", res);
	}

//	printf("Return mLength is %d\n", mLength);
	if (MAX(iWriteLength, iReadLength) >= DEFAULT_BUFFER_LEN)
	{
		delete mWrBuf;
	}

	return res;
}


/**
 * @brief Read data block
 */
int32_t ch341::ReadData(uint *oBuffer, uint *ioLength)
{
	uint *pLen;
	uint curr_len;
	uint32_t res;
	uint *lpInBuffer;
	uint int_buf [258]; // 4 bytes values, 1024 + 16 bytes
#if DEBUG_CH341
	qDebug() << "ch341::ReadData() ";
#endif
	if (devHandle == 0)
	{
		return -1;
	}

	pLen = ioLength;
	if (*ioLength <= 1024)
	{
		lpInBuffer = int_buf;
	}
	else
	{
		if (4096 < *ioLength)
		{
			*ioLength = 4096;
		}
		lpInBuffer = new uint [0x1008]; // 4096 + 8 int values
		if (lpInBuffer == NULL)
		{
			qCritical("ReadData() new() troubles!\n");
			lpInBuffer = int_buf;
			*pLen = 0x400;
		}
	}

	lpInBuffer[0] = 6; // command code, 4 bytes
	curr_len = *pLen;
	if (curr_len < 33) // min size for read
	{
		curr_len = 32;
	}
	lpInBuffer[1] = curr_len; // len, 4 bytes
	*ioLength = (curr_len + 8);

	res = libusb_bulk_transfer(devHandle, CH341_DATA_IN, (uchar *)lpInBuffer, (int) * ioLength, (int *)ioLength, timeout);

	if (res < 0)
	{
		*pLen = 0;
	}
	else
	{
		curr_len = *pLen;
		if ((uint)lpInBuffer[1] < *pLen)
		{
			curr_len = lpInBuffer[1];
		}

		memcpy(oBuffer, &lpInBuffer[2], curr_len);
	}

	if (lpInBuffer != int_buf)
	{
		delete lpInBuffer;
	}

	if (res < 0)
	{
		qCritical("ReadData error %d\n", res);
	}

	return res;
}


/**
 * @brief Write out the data block
 */
int32_t ch341::WriteData(uint *iBuffer, uint *ioLength)
{
	int32_t ret;
	uint curr_len;
	uint *lpOutBuffer;
	uint local_buf [258]; // 1024 + 8 bytes
	uint len;
	int transfered;
#if DEBUG_CH341
	qDebug() << "ch341::WriteData() ";
#endif
	if (devHandle == 0)
	{
		return -1;
	}

	if (*ioLength < 0x401)
	{
		lpOutBuffer = local_buf;
	}
	else
	{
		if (4096 < *ioLength)
		{
			*ioLength = 4096;
		}
		lpOutBuffer = new uint [0x1008]; // 4096 + 32 bytes
		if (lpOutBuffer == NULL)
		{
			qCritical("WriteData() new() troubles!\n");

			lpOutBuffer = local_buf;
			*ioLength = 0x400;
		}
	}

	*lpOutBuffer = 7;           // command
	lpOutBuffer[1] = *ioLength; // data length
	curr_len = *ioLength;

	memcpy(&lpOutBuffer[2], iBuffer, curr_len);

	len = *ioLength + 8;

	ret = libusb_bulk_transfer(devHandle, CH341_DATA_OUT, (uchar *)lpOutBuffer, len, &transfered, timeout);

	if (ret < 0)
	{
		*ioLength = 0;
	}
	else
	{
		*ioLength = lpOutBuffer[1];
	}

	if (lpOutBuffer != local_buf)
	{
		delete lpOutBuffer;
	}

	if (ret < 0)
	{
		qCritical("WriteData() error %d\n", ret);
	}

	return ret;
}


/**
 * @brief Clear CH341 buffer
 */
int32_t ch341::FlushBuffer()
{
	int ret;
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::FlushBuffer() ";
#endif
	if (dev_vers < 0x20)
	{
		qCritical("FlushBuffer() device version too old!\n");
		return -1;
	}
	else
	{
		ResetInter();
		ResetRead();
		ResetWrite();

		buf[0] = 4; // command
		buf[4] = 8; // data length
		buf[8] = 0x40;
		buf[9] = 0xb2;
		uint len = 0x28;

		ret = WriteData((uint *)&buf, &len);

		if (ret < 0)
		{
			qCritical("FlushBuffer() error %d\n", ret);
		}

		return ret;
	}
}


/**
 * @brief Read data blocks from EEPROM at a speed of approximately 56K (For I2C)
 *
 * @param[in] iEepromID 	: EEPROM TYPE
 * @param[in] iAddr 		: the start addr for read
 * @param[in] iLength  	: should read the length of data
 * @param[out] oBuffer  	: output buffer
 *
 */
int32_t ch341::ReadEEPROM(EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint *oBuffer)
{
	uint32_t mLen;
	uchar mWrBuf[4];
#if DEBUG_CH341
	qDebug() << "ch341::ReadEEPROM(" << iAddr << iLength << ") ";
#endif
	if (oBuffer == NULL)
	{
		return -1;
	}

	if (iEepromID >= ID_24C01 && iEepromID <= ID_24C16)
	{
		while (iLength)
		{
			mWrBuf[0] = (uchar)(0xA0 | ((iAddr >> 7) & 0x0E));
			mWrBuf[1] = (uchar)iAddr;
			mLen = MIN(iLength, DEFAULT_BUFFER_LEN);
			if (StreamI2C(2, (uint *)&mWrBuf, mLen, oBuffer))
			{
				return -1;
			}
			iAddr += mLen;
			iLength -= mLen;
			oBuffer += mLen;
		}
		return 0;
	}

	if (iEepromID >= ID_24C32 && iEepromID <= ID_24C4096)
	{
		while (iLength)
		{
			mWrBuf[0] = (uchar)(0xA0 | ((iAddr >> 15) & 0x0E));
			mWrBuf[1] = (uchar)(iAddr >> 8);
			mWrBuf[2] = (uchar)iAddr;
			mLen = MIN(iLength, DEFAULT_BUFFER_LEN);
			if (StreamI2C(3, (uint *)&mWrBuf, mLen, oBuffer))
			{
				return -1;
			}
			iAddr += mLen;
			iLength -= mLen;
			oBuffer += mLen;
		}
		return 0;
	}

	qCritical("ReadEEPROM error\n");
	return -1;
}


/**
 * @brief  * Write data blocks to EEPROM (For I2C)
 *
 * @param[in] iEepromID 	: EEPROM TYPE
 * @param[in] iAddr 		: the start addr for read
 * @param[in] iLength  	: should write the length of data
 * @param[in] iBuffer  	: Iutput buffer
 */
int32_t ch341::WriteEEPROM(EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint *iBuffer)
{
	uint32_t mLen;
	uchar mWrBuf[256];

	if (iBuffer == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::WriteEEPROM(" << iAddr << iLength << ") ";
#endif
	if (iEepromID >= ID_24C01 && iEepromID <= ID_24C16)
	{
		while (iLength)
		{
			mWrBuf[0] = (uchar)((0xA0 | (iAddr >> 7)) & 0x0E);
			mWrBuf[1] = (uchar)iAddr;
			mLen = iEepromID >= ID_24C04 ? 16 - (iAddr & 15) : 8 - (iAddr & 7);
			if (mLen > iLength)
			{
				mLen = iLength;
			}
			memcpy(&mWrBuf[2], iBuffer, mLen);
			if (StreamI2C(2 + mLen, (uint *)&mWrBuf, 0, NULL))
			{
				return -1;
			}
			iAddr += mLen;
			iLength -= mLen;
			iBuffer += mLen;
		}

		return 0;
	}

	if (iEepromID >= ID_24C32 && iEepromID <= ID_24C4096)
	{
		while (iLength)
		{
			mWrBuf[0] = (uchar)((0xA0 | (iAddr >> 15)) & 0x0E);
			mWrBuf[1] = (uchar)(iAddr >> 8);
			mWrBuf[2] = (uchar)iAddr;
			mLen = iEepromID >= ID_24C512 ? 128 - (iAddr & 127) : (iEepromID >= ID_24C128 ? 64 - (iAddr & 63) : 32 - (iAddr & 31));

			if (mLen > iLength)
			{
				mLen = iLength;
			}
			memcpy(&mWrBuf[3], iBuffer, mLen);

			if (StreamI2C(3 + mLen, (uint *)&mWrBuf, 0, NULL))
			{
				return -1;
			}
			iAddr += mLen;
			iLength -= mLen;
			iBuffer += mLen;
		}

		return 0;
	}

	qCritical("WriteEEPROM() error \n");
	return -1;
}


#if 0
// internal function for write/read config regs
int32_t configureReg(uchar param_2, uchar param_3, ushort param_4,
					 ushort param_5, uint *param_6, uint *param_7)
{
	BOOL BVar1;
	uint uVar2;
	uint uVar3;
	uint *puVar4;
	uint local_2c;
	uint local_28;
	uchar local_24;
	uchar local_23;
	ushort local_22;
	ushort local_20;
	ushort local_1e;

	local_24 = param_2;
	local_23 = param_3;
	local_22 = param_4;
	local_2c = 4;
	local_28 = 8;
	local_20 = param_5;
	if (param_7 == null_ptr)
	{
		local_1e = 0;
	}
	else
	{
		local_1e = *(ushort *)param_7;
	}
	_param_4 = 0x28;
	if (iIndex < (HANDLE)0x10)
	{
		iIndex = (HANDLE)(&DAT_00405000)[(int)iIndex];
	}
	BVar1 = DeviceIoControl(iIndex, 0x223cd0, &local_2c, 0x28, &local_2c, 0x28, (LPuint)&param_4,
							(LPOVERLAPPED)0x0);
	if (BVar1 == 0)
	{
		uVar2 = 0;
	}
	else
	{
		if (param_7 != null_ptr)
		{
			if (*param_7 < _param_4)
			{
				_param_4 = *param_7;
			}
			if (_param_4 != 0)
			{
				if (param_6 == null_ptr)
				{
					_param_4 = 0;
				}
				else
				{
					uVar3 = _param_4 >> 2;
					puVar4 = (uint *)&local_24;
					while (uVar3 != 0)
					{
						uVar3 = uVar3 - 1;
						*param_6 = *puVar4;
						puVar4 = puVar4 + 1;
						param_6 = param_6 + 1;
					}
					uVar3 = _param_4 & 3;
					while (uVar3 != 0)
					{
						uVar3 = uVar3 - 1;
						*(uchar *)param_6 = *(uchar *)puVar4;
						puVar4 = (uint *)((int)puVar4 + 1);
						param_6 = (uint *)((int)param_6 + 1);
					}
				}
			}
			*param_7 = _param_4;
		}
		uVar2 = 1;
	}
	return uVar2;
}
#endif

/**
 * @brief FUNCTION : Set direction and output data of D5-D0 on CH341
 * arg:
 * Data : Control direction and data
 * iSetDirOut : set io direction
 *			  -- > Bit High : Output
 *			  -- > Bit Low : Input
 * iSetDataOut : set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 */
int32_t ch341::Set_D5_D0(uchar iSetDirOut, uint iSetDataOut)
{
	int res;

#if DEBUG_CH341
	qDebug() << "ch341::Set_D5_D0(" << iSetDirOut << iSetDataOut << ") ";
#endif

	if (devHandle == 0)
	{
		return -1;
	}

	if (dev_vers < 0x20)
	{
		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, 0x1606, (((ushort)iSetDirOut & 0x3f) << 8 | (ushort)(uchar)(iSetDataOut & 0x3f)), NULL, 0, timeout);
		if (res < 0)
		{
			qCritical("Set_D5_D0 write error %d\n", res);
		}
	}
	else
	{
		uchar buf[0x28];
		buf[0] = 0xab;
		buf[1] = (uchar)(iSetDataOut & 0x3f) | 0x80;
		buf[2] = (iSetDirOut & 0x3f) | 0x40;
		buf[3] = 0x20;
		uint len = 4;

		res = WriteData((uint *)&buf, &len);
		if (res < 0 || len < 3)
		{
			qCritical("Set_D5_D0 write error %d\n", res);
		}
	}
	return res;
}

/**
 * @brief Reset interrupt data read operation
 */
int32_t ch341::ResetInter()
{
	uchar buf[0x28] = {0};
	int32_t res;
#if DEBUG_CH341
	qDebug() << "ch341::ResetInter() ";
#endif
	buf[0] = CH341_REQ_RESET;  // command
	buf[4] = 4;    // length of write data
	buf[8] = 5;
	uint len = 0x28;

	res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("ResetInter() error %d\n", res);
	}

	return res;
}

/**
 * @brief reset data read operation
 */
int32_t ch341::ResetRead()
{
	uchar buf[0x28] = {0};
	int32_t res;
#if DEBUG_CH341
	qDebug() << "ch341::ResetRead() ";
#endif
	buf[0] = CH341_REQ_RESET;  // command
	buf[4] = 4;    // length of write data
	buf[8] = 6;
	uint len = 0x28;

	res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("ResetRead() error %d\n", res);
	}

	return res;
}


/**
 * @brief reset data write operation
 */
int32_t ch341::ResetWrite()
{
	uchar buf[0x28] = {0};
	int res;
#if DEBUG_CH341
	qDebug() << "ch341::ResetWrite() ";
#endif
	buf[0] = CH341_REQ_RESET;  // command
	buf[4] = 4;    // length of write data
	buf[8] = 7;
	uint len = 0x28;

	res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("ResetWrite() error %d\n", res);
	}

	return res;
}

/**
 * @brief  Set the hardware asynchronous delay, returning quickly after the call, and delay the specified number of milliseconds before the next stream operation
 *
 * @param[in] iDelay : set delay time(mSec)
 * @return if successful 0
 */
int32_t ch341::SetDelaymS(uint iDelay)
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::SetDelaymS(" << iDelay << ") ";
#endif
	if (dev_vers < 0x20)
	{
		qCritical("SetDelaymS() device version is too old!\n");
		return -1;
	}

	int res = 0;

	while (iDelay)
	{
		uint mLength = iDelay >= CH341_CMD_I2C_STM_DLY ? CH341_CMD_I2C_STM_DLY : iDelay;
		iDelay -= mLength;
		buf[0] = CH341_CMD_I2C_STREAM; // command code
		buf[1] = (uchar)(CH341_CMD_I2C_STM_MS | mLength);
		buf[2] = CH341_CMD_I2C_STM_END;

		uint len = 3;

		res = WriteData((uint *)&buf, &len);
		if (res < 0)
		{
			qCritical("SetDelaymS() error %d\n", res);
		}
	}

	return res;
}


/**
 * @brief Read one byte of data from the I2C interface
 */
int32_t ch341::ReadI2C(uchar iDevice, uchar iAddr, uchar *oByte)
{
	int res;
	uint len;
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::ReadI2C(" << iDevice << iAddr << ") ";
#endif
	if (dev_vers < 0x20)
	{
		buf[0] = 4; // command code
		buf[4] = 8;
		buf[8] = 0x40;
		buf[9] = 0x53;
		buf[10] = 0;
		(*(ushort *)&buf[12]) = (((ushort)(iDevice << 1) << 8) + iAddr) | 0x100;
		buf[14] = 0;
		len = 0x28;

		res = WriteData((uint *)&buf, &len);

		if (res == 0)
		{
			for (int i = 0; i < 100; i++) // try max 100 times
			{
				buf[0] = 4; // command code
				buf[4] = 8; // len of write data
				buf[8] = 0xc0;
				buf[9] = 0x52;
				(*(ushort *)&buf[10]) = 0;
				(*(ushort *)&buf[12]) = 0;
				buf[14] = 8; // len of read data

				len = 0x28;
				res = WriteRead(0x28, buf, 0x20, 1, &len, buf);

				if (res < 0)
				{
					qCritical("ReadI2C() error %d\n", res);
					return res;
				}

				if (((*(ushort *)&buf[12]) & 0x100) == 0) // 1 byte
				{
					*oByte = buf[12];
					return 0;
				}
			}
		}
	}
	else
	{
		buf[0] = 4; // command code
		buf[4] = 8; // len of write data
		buf[8] = 0xc0;
		buf[9] = 0x54;
		(*(ushort *)&buf[10]) = (ushort)iAddr << 8; // convert to short!!!
		(*(ushort *)&buf[12]) = (((ushort)(iDevice << 1) << 8) + 1); // short!!!
		buf[14] = 1;
		len = 0x28;
		res = WriteRead(0x28, buf, 0x20, 1, &len, buf);

// 		if ((ret == 0) && (buf[12] = buf[8], len != 0))
		// TODO wat is buf[12] = buf[8]???
		if ((res == 0) && (len != 0))
		{
			//buf[12] = buf[8]; // is it right???
			*oByte = buf[12];
			return 0;
		}
	}

	if (res < 0)
	{
		qCritical("ReadI2C() error %d\n", res);
	}
	return -1;
}


/**
 * @brief Write a byte of data to the I2C interface
 */
int32_t ch341::WriteI2C(uchar iDevice, uchar iAddr, uchar iByte)
{
	int res;
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::WriteI2C(" << iDevice << iAddr << ") ";
#endif
	buf[0] = 4; // command code
	buf[4] = 8; // len of write data
	buf[8] = 0x40;
	buf[9] = 0x53;
	buf[10] = iByte;
	(*(ushort *)&buf[12]) = (((ushort)(iDevice << 1) << 8) + iAddr); // CONCAT11(iDevice << 1, iAddr);
	buf[14] = 0; // len of read data

	uint len = 0x28;
	res = WriteData((uint *)&buf, &len);

	if (res == 0)
	{
		for (int i = 0; i < 100; i++) // try max 100 times
		{
			buf[0] = 4; // command code
			buf[4] = 8; // len of write data
			buf[8] = 0xc0;
			buf[9] = 0x52;
			(*(ushort *)&buf[10]) = 0;
			(*(ushort *)&buf[12]) = 0;
			buf[14] = 8; // len of read data

			len = 0x28;
			res = WriteRead(0x28, buf, 0x20, 1, &len, buf);

			if (res < 0)
			{
				qCritical("WriteI2C() error %d\n", res);
				return -1;
			}
			if (((*(ushort *)&buf[12]) & 0x0100) == 0)
			{
				return 0;
			}
		}
	}

	qCritical("WriteI2C() error %d\n", res);
	return -1;
}

/**
 * @brief Set a timeout for USB data read and write
 */
int32_t ch341::SetTimeout(uint iWriteTimeout, uint iReadTimeout)
{
	uchar buf[0x28] = {0};
	uint len = 0x28;
#if DEBUG_CH341
	qDebug() << "ch341::SetTimeout(" << iWriteTimeout << iReadTimeout << ") ";
#endif
	buf[0] = 9; // command code
	buf[4] = 8; // length of data to write
	(*(uint *)&buf[8]) = iWriteTimeout;
	(*(uint *)&buf[12]) = iReadTimeout;

	int res = WriteData((uint *)&buf, &len);

	if (res < 0)
	{
		qCritical("SetTimeout() error %d\n", res);
	}

	return res;
}


/**
 * @brief Set exclusive use of current CH341 device
 */
int32_t ch341::SetExclusive(uint iExclusive)
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::SetExclusive(" << iExclusive << ") ";
#endif
	buf[0] = 0xb; // command code
	buf[4] = 1;   // length of written data
	buf[8] = (iExclusive != 0) ? 1 : 0;
	uint len = 0x28;

	int res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("SetExclusive() error %d\n", res);
	}

	return res;
}

/**
 * @breif
 */
int32_t ch341::ResetDevice()
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::ResetDevice() ";
#endif
	buf[0] = 0xc;  // command code
	buf[4] = 0;    // length of written data
	uint len = 0x28;

	int res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("ResetDevice() error %d\n", res);
	}

	return res;
}


/**
 * @brief
 */
int32_t ch341::AbortRead()
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::AbortRead() ";
#endif
	buf[0] = CH341_REQ_ABORT;  // command code
	buf[4] = 4;    // length of written data
	buf[8] = 6;
	uint len = 0x28;

	int res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("AbortRead() error %d\n", res);
	}

	return res;
}


/**
 *
 */
int32_t ch341::AbortWrite()
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::AbortWrite() ";
#endif
	buf[0] = CH341_REQ_ABORT;  // command code
	buf[4] = 4;    // length of written data
	buf[8] = 7;
	uint len = 0x28;

	int res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("AbortWrite() error %d\n", res);
	}

	return res;
}


/**
 * @brief Processing SPI data stream, support quasi-bidirectional single-input single-out / 3-wire interface, single-input single-out / 4-wire interface, dual-input and double-out/5-line interface
 * chip_select: Chip select control
 * bit 7 is 0 to ignore chip select control, bit 7 is 1 to enable parameter: Bit 1 bit 0 is 00/01/10 select D0/D1/D2 pin as active low Chip Select
 * length: the number of data bytes to be transferred
 * buffer: points to a buffer, puts the data to be written from SDO, and returns the data read from SDI.
 * buffer2: points to the second buffer of the dual-input and double-out 5-wire interface. 1 is a single-input single-out 4-wire interface mode, and 0 is a SIO quasi-bidirectional 3-wire interface mode.
 */
int32_t ch341::StreamSPI(unsigned long chip_select, unsigned long length, uchar *buffer, uchar *buffer2)
{
	uchar m_buffer[DEFAULT_BUFFER_LEN];
	uint i, j, m_length, m_select;
	uchar	*m_write_buffer;
	uchar c1, c2;
	int res;
#if DEBUG_CH341
	qDebug() << "ch341::StreamSPI(" << chip_select << length << ") ";
#endif
	if (length > MAX_BUFFER_LENGTH)
	{
		return -1;
	}
	if (length <= DEFAULT_BUFFER_LEN / 2)
	{
		m_write_buffer = m_buffer;
	}
	else
	{
		m_write_buffer = new uchar [MAX_BUFFER_LENGTH];
		if (m_write_buffer == NULL)
		{
			qCritical("StreamSPI() new() troubles!\n");
			return -1;
		}
	}
	i = 0;
	if (chip_select & 0x80) //Chip selection control
	{
		m_write_buffer[i++] = CH341_CMD_UIO_STREAM;
		switch (chip_select & 0x03)
		{
		case 0x00:
			m_select = 0x36;
			break;
		case 0x01:
			m_select = 0x35;
			break;
		case 0x02:
			m_select = 0x33;
			break;
		default:
			m_select = 0x27;
			break;
		}
		m_write_buffer[i++] = (uchar)CH341_CMD_UIO_STM_OUT | m_select;
		m_write_buffer[i++] = (uchar)CH341_CMD_UIO_STM_DIR | 0x3F;
		m_write_buffer[i++] = CH341_CMD_UIO_STM_END;
		i = CH341_PACKET_LENGTH;
	}
	if (length)
	{
		if ((unsigned long)buffer2 < 4) //Single entry
		{
			for (j = 0; j < length;)	//The number of bytes the user requested to write
			{
				m_write_buffer[i++] = buffer2 ? CH341_CMD_SPI_STREAM : CH341_CMD_SIO_STREAM;
				m_length = CH341_PACKET_LENGTH - 1;
				if (m_length > length - j)
				{
					m_length = length - j;
				}
				if (StreamMode & 0x80) //High position
				{
					while (m_length--)
					{
						m_write_buffer[i++] = ReverseByte(*((uchar *)buffer + j++));
					}
				}
				else while (m_length--)
					{
						m_write_buffer[i++] = *((uchar *)buffer + j++);    //Low position
					}

				if (i % CH341_PACKET_LENGTH == 0)
				{
					m_write_buffer[i] = m_write_buffer[i + 1] = 0;
					i += CH341_PACKET_LENGTH;
				}
			}
		}
		else//Double entry and double exit
		{
			for (j = 0; j < length;)
			{
				m_write_buffer[i++] = CH341_CMD_SPI_STREAM;
				m_length = CH341_PACKET_LENGTH - 1;
				if (m_length > length - j)
				{
					m_length = length - j;
				}
				if (StreamMode & 0x80)
				{
					while (m_length--) //Copy data
					{
						c1 = ReverseByte(*((uchar *)buffer + (j >> 1)));
						c2 = ReverseByte(*((uchar *)buffer2 + (j >> 1)));
						m_write_buffer[i++] = (uchar)((j & 0x01) ? ((c1 & 0xF0) | ((c2 >> 4) & 0xF0)) : (((c1 << 4) & 0xF0) | (c2 & 0xF0)));
						j++;
					}
				}
				else//Low position
				{
					while (m_length--) //Copy data
					{
						c1 = *((uchar *)buffer + (j >> 1));
						c2 = *((uchar *)buffer2 + (j >> 1));
						m_write_buffer[i++] = (uchar)((j & 0x01) ? ((c1 & 0xF0) | ((c2 >> 4) & 0xF0)) : (((c1 << 4) & 0xF0) | (c2 & 0xF0)));
						j++;
					}
				}
				if (i % CH341_PACKET_LENGTH == 0)
				{
					m_write_buffer[i] = m_write_buffer[i + 1] = 0;
					i += CH341_PACKET_LENGTH;
				}
			}
		}
	}

	m_length = 0;

	uchar *p_read_buf = *buffer2 < 4 ? buffer : m_write_buffer;

	res = WriteRead(i, m_write_buffer, CH341_PACKET_LENGTH - 1, (length + CH341_PACKET_LENGTH - 1 - 1) / (CH341_PACKET_LENGTH - 1), &m_length, p_read_buf);
	if (res < 0 || m_length != length)
	{
		res = -1;
	}

	if (m_write_buffer != m_buffer)
	{
		delete m_write_buffer;
	}

	j = m_length;

	if (chip_select & 0x80)
	{
		m_buffer[0] = CH341_CMD_UIO_STREAM;
		m_buffer[1] = (uchar)(CH341_CMD_UIO_STM_OUT | 0x37);
		m_buffer[2] = CH341_CMD_UIO_STM_END;
		m_length = 3;
		if (WriteData((uint *)&m_buffer, &m_length))
		{
			if (m_length < 2)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	if (res == 0) // no errors
	{
		if ((unsigned long)buffer2 < 4)
		{
			if (StreamMode & 0x80)
			{
				for (i = 0; i < length; i++)
				{
					m_write_buffer[i] = ReverseByte(*((uchar *)buffer + j++));      //Copy data TODO to check this!
				}
			}
		}
		else
		{
			for (i = 0; i < length; i += 2)
			{
				c1 = m_write_buffer[i];
				c2 = m_write_buffer[i + 1];
				if (StreamMode & 0x80)
				{
					*((uchar *) buffer + (i >> 1)) = ReverseByte(((c1 >> 4) & 0x0F) | (c2 & 0xF0));
					*((uchar *) buffer2 + (i >> 1)) = ReverseByte((c1 & 0x0F) | ((c2 << 4) & 0xF0));
				}
				else
				{
					*((uchar *) buffer + (i >> 1)) = (uchar)((c1 >> 4) & 0x0F) | (c2 & 0xF0);
					*((uchar *) buffer2 + (i >> 1)) = (uchar)(c1 & 0x0F) | ((c2 << 4) & 0xF0);
				}
			}
		}
	}

	return res;
}


/**
 * Processing SPI bit data stream, 4-wire/5-wire interface
 * clock line is DCK / D3 pin
 * output data line is DOUT/DOUT2 pin
 * input data line is DIN / DIN2 pin
 * chip Select line is D0/D1/D2, speed about 8K*2
 *
 * iLength
 *
 * ioBuffer
 *
 * SPI timing: the DCK / D3 pin is the clock output, which defaults to low,
 * the DOUT/D5 and DOUT2/D4 pins are output low before the clock rising edge,
 * and the DIN/D7 and DIN2/D6 pins are input high before the clock falling edge.
 * a byte in ioBuffer corresponds to a total of 8 bits of the D7-D0 pin,
 * bit 5 is output to DOUT,
 * bit 4 is output to DOUT2,
 * bit 2-bit 0 is output to D2-D0,
 * bit 7 is input from DIN,
 * bit 6 is input from DIN2,
 * bit 3 data is ignored
 * Before calling the API, you should call CH341Set_D5_D0 to set the I/O direction of the Ch341 D5-D0 pin and set the default level of the pin
 */
int32_t ch341::BitStreamSPI(uint iLength, uint *ioBuffer)
{
	int res;
	uint offs;
	uint *pBuff;
	uint local_buf [323]; // 1292 bytes
#if DEBUG_CH341
	qDebug() << "ch341::BitStreamSPI(" << iLength << ") ";
#endif

	uint curr_len = iLength;

	// TODO to decode this
	if (dev_vers < 0x20)
	{
		return -1;
	}

	if ((iLength == 0) || (iLength > 1024))
	{
		return -1;
	}

	if (iLength <= 256)
	{
		pBuff = local_buf;
	}
	else
	{
		pBuff = new uint[5130];
		if (pBuff == NULL)
		{
			qCritical("BitStreamSPI() new() troubles!\n");
			return -1;
		}
	}
	offs = 0;

	for (uint i = 0; ; i < iLength)
	{
		*(uchar *)(pBuff + offs) = 0xab;
		uchar j = 7;
		if (iLength - i < 7)
		{
			j = iLength - i;
		}
		while (j != 0)
		{
			curr_len = j - 1;

			uchar val = *(uchar *)(ioBuffer + i) & 0x37;
			i++;
			val = val | 0x80;
			*(uchar *)(pBuff + offs + 1) = val;
			*(uchar *)(pBuff + offs + 2) = val | 0x88;
			*(uchar *)(pBuff + offs + 3) = 0;
			offs += 4;
			*(uchar *)(pBuff + offs) = val;
			j = curr_len;
		}

		*(uchar *)(pBuff + offs + 1) = 0x20;
		offs += 4;
	}

	uint len = 0;
	res = WriteRead(offs, (uchar *)pBuff, 7, (iLength + 6) / 7, &len, (uchar *)ioBuffer);

	if ((res < 0) || (len != iLength))
	{
		res = -1;
	}
	if (pBuff != local_buf)
	{
		delete pBuff;
	}

	if (res < 0)
	{
		qCritical("BitStreamSPI() error %d\n", res);
	}

	return res;
}


/**
 * Set the internal buffer upload mode
 * iEnableOrClear 0 is prohibited internal buffer upload mode, use direct upload, non-0 is enabled internal buffer upload mode and clear the existing data in the buffer
 * If the internal buffer upload mode is enabled,then the CH341 driver creates a thread that automatically receives the USB upload data to the internal buffer, and clears the existing data in the buffer, and immediately returns the existing data in the buffer when the application calls CH341ReadData
 *
 * iEnableOrClear 0 is prohibited internal buffer upload mode, use direct upload, non-0 is enabled internal buffer upload mode and clear the existing data in the buffer
 */
int32_t ch341::SetBufUpload(uint iEnableOrClear)
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::SetBufUpload(" << iEnableOrClear << ") ";
#endif
	buf[0] = 0x20;
	buf[4] = 1;
	(*(uint *)&buf[8]) = -(iEnableOrClear != 0) & 2;

	uint len = 0x28;

	int res = WriteData((uint *)buf, &len);

	if (res < 0)
	{
		qCritical("SetBufUpload() error %d\n", res);
	}

	return res;
}


/**
 * @brief Query the internal upload buffer the number of existing data packets, the number of packets returned successfully, an error returns -1
 */
int32_t ch341::QueryBufUpload()
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::QueryBufUpload() ";
#endif
	buf[0] = 0x20;
	buf[4] = 1;
	buf[8] = 1;

	uint len = 0x28;

	int res = WriteRead(0x28, buf, 0x20, 1, (uint *)&len, buf);
	if (res < 0)
	{
		qCritical("QueryBufUpload() error %d\n", res);
		return -1;
	}

	return (*(int *)&buf[8]); // convert to int
}


/**
 * @brief Set the internal buffer downstream mode
 *
 * If you enable the internal buffer downstream mode, then when the application calls CH341WriteData will simply be the USB downstream data into the internal buffer and returned immediately, and the thread created by the ch341 driver is sent automatically until finished
 * iEnableOrClear 0 is prohibited internal buffer downstream mode, use the direct downstream, non-0 is enabled internal buffer downstream mode and clear the existing data in the buffer
 */
int32_t ch341::SetBufDownload(uint iEnableOrClear)
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::SetBufDownload(" << iEnableOrClear << ") ";
#endif
	buf[0] = 0x21;
	buf[4] = 1;
	(*(uint *)&buf[8]) = -(iEnableOrClear != 0) & 2;

	uint len = 0x28;

	int res = WriteData((uint *)buf, &len);
	if (res < 0)
	{
		qCritical("SetBufDownload() error %d\n", res);
	}

	return res;
}



/**
 * @brief Query the internal upload buffer the number of existing data packets, the number of packets returned successfully, an error returns -1
 */
int32_t ch341::QueryBufDownload()
{
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::QueryBufDownload() ";
#endif
	buf[0] = 0x21;
	buf[4] = 1;
	buf[4] = 1;
	int len = 0x28;

	int res = WriteRead(0x28, buf, 0x20, 1, (uint *)&len, buf);
	if (res < 0) // wenn error
	{
		qCritical("QueryBufDownload() error %d\n", res);
		return -1;
	}

	return (*(int *)&buf[8]); // convert to int
}


/** Process SPI data stream, 4-wire interface:
 * The clock line is the DCK/D3 pin;
 * The output data line is the DOUT/D5 pin;
 * The input data line is DIN/D7 pin;
 * The chip select line is D0/D1/D2 and the speed is about 110K.
 * SPI timing: DCK/D3 pin is the clock output, the default is low level;
 * The DOUT/D5 pin is output during the low period before the rising edge of the clock;
 * The DIN/D7 pin is input during the high period before the falling edge of the clock.
 * Operation succeeded. Failed to return 0.
 * chip_select: Chip select control
 * bit 7 is 0 to ignore chip select control
 * bit 7 is 1 to enable parameter: Bit 1 bit 0 is 00/01/10 select D0/D1/D2 pin as active low Chip Select
 * length: the number of data bytes to be transferred
 * buffer: points to a buffer, puts the data to be written from DOUT, and returns the data read from DIN.
 */
int32_t ch341::StreamSPI4(uint chip_select, uint length, uchar *buffer)
{
	int res = -1;
#if DEBUG_CH341
	qDebug() << "ch341::StreamSPI4() ";
#endif
	if (StreamMode & 0x40)
	{
		res = SetStreamMode(StreamMode & 0xFB);
		if (res < 0)
		{
			qCritical("StreamSPI4() error in SetStreamMode %d\n", res);
			return -1;
		}
	}
	res = StreamSPI(chip_select, length, buffer, (uchar *)1);
	if (res < 0)
	{
		qCritical("StreamSPI4() error %d\n", res);
		return -1;
	}

	return res;
}

/** Processing SPI data stream, 5-wire interface
 * clock line is DCK/D3 pin
 * output data line is DOUT/D5 and DOUT2/D4 pin
 * input data line is DIN/D7 and DIN2/D6 pin
 * chip select line is D0/D1/D2
 * speed about 30K*2
 * SPI Timing: The DCK/D3 pin is the clock output. The default is low.
 * The DOUT/D5 and DOUT2/D4 pins are output during the low period before the rising edge of the clock. DIN/D7 and DIN2/D6 pins.
 * Enter during the high level before the falling edge of the clock
 */
int32_t ch341::StreamSPI5(uint chip_select, uint length, uchar *buffer, uchar *buffer2)
{
	int res = -1;
#if DEBUG_CH341
	qDebug() << "ch341::StreamSPI5() ";
#endif
	if (StreamMode & 0x40)
	{
		res = SetStreamMode(StreamMode | 4);
		if (res < 0)
		{
			qCritical("StreamSPI5() error in SetStreamMode %d\n", res);
			return -1;
		}
	}
	res = StreamSPI(chip_select, length * 2, buffer, buffer2);
	if (res < 0)
	{
		qCritical("StreamSPI5() error %d\n", res);
		return -1;
	}
	return res;
}


/**
 * @brief * FUNCTION : Set direction and output data of CH341
 * arg:
 * Data :	Control direction and data
 *
 * iEnbale : set direction and data enable
 * 			   --> Bit16 High :	effect on Bit15~8 of iSetDataOut
 * 			   --> Bit17 High :	effect on Bit15~8 of iSetDirOut
 * 			   --> Bit18 High :	effect on Bit7~0 of iSetDataOut
 * 			   --> Bit19 High :	effect on Bit7~0 of iSetDirOut
 *			   --> Bit20 High :	effect on Bit23~16 of iSetDataOut
 * iSetDirOut : set io direction
 *			  -- > Bit High : Output
 *			  -- > Bit Low : Input
 * iSetDataOut : set io data
 * 			 Output:
 *			  -- > Bit High : High level
 *			  -- > Bit Low : Low level
 * Note:
 * Bit7~Bit0<==>D7-D0
 * Bit8<==>ERR#    Bit9<==>PEMP    Bit10<==>INT#    Bit11<==>SLCT    Bit13<==>WAIT#    Bit14<==>DATAS#/READ#    Bit15<==>ADDRS#/ADDR/ALE
 * The pins below can only be used in output mode:
 * Bit16<==>RESET#    Bit17<==>WRITE#    Bit18<==>SCL    Bit29<==>SDA
 *
 */
int32_t ch341::SetOutput(uint iEnable, uint iSetDirOut, uint iSetDataOut)
{
	ushort uVar1;
	uint uVar2;
	int res = -1;
	ushort reg_nr;
	uchar buf[0x28] = {0};
#if DEBUG_CH341
	qDebug() << "ch341::SetOutput(" << iEnable << iSetDirOut << iSetDataOut << ") ";
#endif
	if (devHandle == 0)
	{
		return -1;
	}

	uVar2 = iSetDirOut;

	buf[3] = (uchar)((uint)iSetDataOut >> 8);
	buf[4] = (uchar)(iSetDirOut >> 8);
	buf[6] = (uchar)iSetDirOut;

	if (dev_vers > 0x1f)
	{
		buf[0] = CH341_CMD_SET_OUTPUT;
		buf[1] = 0x6a;
		buf[2] = (uchar)iEnable & 0x1f;
		buf[3] &= 0xef;
		buf[4] |= 0x10;
		buf[5] = (uchar)iSetDataOut;
		buf[7] = (uchar)((uint)iSetDataOut >> 0x10) & 0xf;
		buf[8] = 0;
		buf[9] = 0;
		buf[10] = 0;

		iSetDirOut = 0xb;

		res = WriteData((uint *)&buf, &iSetDirOut);

		if (res < 0)
		{
			qCritical("SetOutput() error %d\n", res);
			return -1;
		}

		return 0;
	}

	// for old versions, smaller 0x20
	if ((iEnable & 3) != 0)
	{
		ushort val;
		if ((iEnable & 3) == 3)
		{
			reg_nr = 0x1606;
			val = ((ushort)(buf[6] << 8) + (uchar)iSetDataOut);
		}
		else
		{
			if ((iEnable & 2) != 0)
			{
				reg_nr = 0x1616;
				iSetDataOut = buf[6];
				val = ((ushort)(buf[6] << 8) + (uchar)iSetDataOut);
			}
			reg_nr = 0x0606;
			val = ((ushort)(iSetDataOut << 8) + (uchar)iSetDataOut);
		}

		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, reg_nr, val, NULL, 0, timeout);
		if (res < 0)
		{
			qCritical("SetOutput() error %d\n", res);
			return res;
		}
	}

	uVar1 = (ushort)((uint)iSetDataOut >> 8);
	if ((iEnable & 0xc) != 0)
	{
		ushort val;
		if ((iEnable & 0xc) == 0xc)
		{
			reg_nr = 0x1505;
			val = ((ushort)((buf[4] << 8) + (uchar)buf[3]) & 0xffef) | 0x1000;
		}
		else
		{
			if ((iEnable & 8) == 0)
			{
				reg_nr = 0x0505;
				val = (ushort)(iSetDataOut & 0xef00) | (uVar1 & 0xef);
			}
			else
			{
				reg_nr = 0x1515;
				val = (ushort)(uVar2 & 0xff10) | (ushort)((uVar2 >> 8) & 0xef) | 0x1010;
			}
		}

		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, reg_nr, val, NULL, 0, timeout);
		if (res < 0)
		{
			qCritical("SetOutput() error %d\n", res);
			return res;
		}
	}

	if ((iEnable & 0x10) != 0)
	{
		res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, 0x707, (ushort)(((uint)iSetDataOut >> 0x10) & 0xf) | (uVar1 & 0xf00), NULL, 0, timeout);
		if (res < 0)
		{
			qCritical("SetOutput() error %d\n", res);
		}
	}

	return res;
}

/**
 * @brief
 */
int32_t ch341::SetupSerial(uint32_t paritymode, uint32_t baude)
{
	int dv_prescale;
	int dv_div;
	int dv_mod;
	int res = -1;
#if DEBUG_CH341
	qDebug() << "ch341::SetupSerial(" << paritymode << baude << ") ";
#endif
	static QVector<uart_div> speed_divider = {{307200, 307200, 0, {7, 0xD9, 0}},
		{921600, 921600, 0, {7, 0xF3, 0}},
		{2999999, 23530, 6000000, {3, 0, 0}},
		{23529, 2942, 750000, {2, 0, 0}},
		{2941, 368, 93750, {1, 0, 0}},
		{367, 1, 11719, {0, 0, 0}}
	};

	if (devHandle == 0)
	{
		return -1;
	}

	if (((paritymode < 5) && (baude >= 50)) && (baude <= 3000000))
	{
		uchar buf[2];
		uint sz = 2;
		uchar coef = 0;

		res = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_READ_REG, CH341_REG_LCR_GET, 0, &buf[0], sz, timeout);
		if (res < 0)
		{
			qCritical("SetupSerial() error %d\n", res);
			return res;
		}

		if ((sz == 2) && ((buf[0] & 0x80) != 0))
		{
			if (dev_vers < 0x30)
			{
				buf[1] &= 0x50;
				switch (paritymode)
				{
				case 1:
					buf[1] |= 0x80;
					coef = 6;
					break;

				case 2:
					buf[1] |= 0x80;
					coef = 7;
					break;

				case 3:
					buf[1] |= 0x80;
					coef = 5;
					break;

				case 4:
					buf[1] |= 0x80;
					coef = 4;
					break;

				default:
					break;
				}
			}
			else
			{
				switch (paritymode)
				{
				case 1:
					buf[1] = 0xcb;
					break;

				case 2:
					buf[1] = 0xdb;
					break;

				case 3:
					buf[1] = 0xeb;
					break;

				case 4:
					buf[1] = 0xfb;
					break;

				default:
					buf[1] = 0xc3;
					break;
				}
			}

			res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR_SET, ((buf[1] << 8) | (ushort)coef), NULL, 0, timeout);
			if (res < 0)
			{
				qCritical("SetupSerial() error %d\n", res);
				return res;
			}

			foreach (uart_div d, speed_divider)
			{
				if (d.dvr_high >= baude && d.dvr_low <= baude)
				{
					dv_prescale = d.dvr_divider.dv_prescaler;
					if (d.dvr_base_clock == 0)
					{
						dv_div = d.dvr_divider.dv_div;
					}
					else
					{
						uint32_t div = d.dvr_base_clock / baude;
						uint32_t rem = d.dvr_base_clock % baude;
						if (div == 0 || div >= 0xFF)
						{
							qCritical("SetupSerial() error , div is 0!\n");
							return -1;
						}

						if ((rem << 1) >= baude)
						{
							div += 1;
						}
						dv_div = (uint8_t) - div;
					}

					uint32_t mod = (CH341_BPS_MOD_BASE / baude) + CH341_BPS_MOD_BASE_OFS;
					mod = mod + (mod >> 1);

					dv_mod = (mod + 0xFF) / 0x100;
					// now set the registers!

					// calculated, now send to device
					dv_prescale |= 0x80;

					qDebug() << "set baudrate" << baude << (hex) << (dv_div << 8) + (dv_prescale) << (uint16_t)(dv_mod) << (dec) ;

					res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD1, (uint16_t)((dv_div << 8) + (dv_prescale)), NULL, 0, timeout);
					if (res < 0)
					{
						qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD1\n");
						return res;
					}

					res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD2, (uint16_t)(dv_mod << 8), NULL, 0, timeout);
					if (res < 0)
					{
						qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD2\n");
						return res;
					}

					return res;
				}
			}
		}
	}

	qCritical("SetupSerial() error %d, not found\n", res);
	return -1;
}

// TODO to check this
int32_t ch341::SetConfigLCR()
{
#if DEBUG_CH341
	qDebug() << "ch341::SetConfigLCR() ";
#endif
	if (devHandle == 0)
	{
		return -1;
	}
	// TODO in case of same bits for all UART devices, to move this lcr into SerialInterface
	// now set LCR
	uint8_t lcr =  CH341_LCR_ENABLE_RX | CH341_LCR_ENABLE_TX;

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

	int32_t ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR_SET, lcr, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_LCR\n");
		return ret;
	}

	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0, NULL, 0, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL\n");
	}

	return ret;
}


int32_t ch341::SetBreakControl(int32_t state)
{
#if DEBUG_CH341
	qDebug() << "ch341::SetBreakControl(" << state << ") ";
#endif

	if (devHandle == 0)
	{
		return -1;
	}
// 	uint16_t reg_contents;
	uint8_t break_reg[2] = {0, 0};

	int32_t ret = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_READ_REG, CH341_REG_BREAK, 0, break_reg, 2, timeout);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_READ_REG, CH341_REG_BREAK\n");
		return ret;
	}
	if (state)
	{
		break_reg[0] &= ~CH341_NBREAK_BITS_REG1; // NBREAK BIT
		break_reg[1] &= ~CH341_NBREAK_BITS_REG2; // TX BIT
	}
	else
	{
		break_reg[0] |= CH341_NBREAK_BITS_REG1; // NBREAK BIT
		break_reg[1] |= CH341_NBREAK_BITS_REG2; // TX BIT
	}
	// TODO is it right???
	//reg_contents = ReverseWord((uint16_t)(break_reg[0] << 8) + break_reg[1]);
// 	reg_contents = ((uint16_t)(break_reg[0] << 8) + break_reg[1]);

	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BREAK, *(uint16_t *)break_reg, NULL, 0, timeout);
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
int32_t ch341::SetBaudRate(uint32_t speed)
{
	int32_t ret;
	int dv_prescale;
	int dv_div;
	int dv_mod;
#if DEBUG_CH341
	qDebug() << "ch341::SetBaudRate(" << speed << ") ";
#endif
	static QVector<uart_div> speed_divider = {{307200, 307200, 0, {7, 0xD9, 0}},
		{921600, 921600, 0, {7, 0xF3, 0}},
		{2999999, 23530, 6000000, {3, 0, 0}},
		{23529, 2942, 750000, {2, 0, 0}},
		{2941, 368, 93750, {1, 0, 0}},
		{367, 1, 11719, {0, 0, 0}}
	};

	if (devHandle == 0)
	{
		return -1;
	}

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
				uint32_t div = d.dvr_base_clock / speed;
				uint32_t rem = d.dvr_base_clock % speed;
				if (div == 0 || div >= 0xFF)
				{
					return -1;
				}

				if ((rem << 1) >= speed)
				{
					div += 1;
				}
				dv_div = (uint8_t) - div;
			}

			uint32_t mod = (CH341_BPS_MOD_BASE / speed) + CH341_BPS_MOD_BASE_OFS;
			mod = mod + (mod / 2);

			dv_mod = (mod + 0xFF) / 0x100;

			// calculated, now send to device
			qDebug() << "set baudrate" << speed << (hex) << (dv_div << 8) + (dv_prescale) << (uint16_t)(dv_mod) << (dec) ;

			ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD1, (dv_div << 8) + (dv_prescale), NULL, 0, timeout);
			if (ret < 0)
			{
				qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD1\n");
				return ret;
			}

			ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_BAUD2, (uint16_t)(dv_mod), NULL, 0, timeout);
			if (ret < 0)
			{
				qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_BAUD2\n");
				return ret;
			}

			ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0, NULL, 0, timeout);
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
int32_t ch341::ResetChip()
{
#if DEBUG_CH341
	qDebug() << "ch341::ResetChip() ";
#endif
	if (devHandle == 0)
	{
		return -1;
	}

	int res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_SERIAL_INIT, CH341_RESET_VALUE, CH341_RESET_INDEX, NULL, 0, timeout);
	if (res < 0)
	{
		qCritical("ResetChip() error %d\n", res);
	}
	return res;
}


int32_t ch341::ClearChip()
{
	if (devHandle == 0)
	{
		return -1;
	}
//     return libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_SERIAL_INIT, 0, 0, NULL, 0, timeout);
	int res = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_SERIAL_INIT, 0xc29c, 0xb2b9, NULL, 0, timeout);
	if (res < 0)
	{
		qCritical("ClearChip() error %d\n", res);
	}
	return res;
}


/**
 * Open CH341A, find the device and set the default interface.
 */
int32_t ch341::Open(uint16_t vid, uint16_t pid)
{
	struct libusb_device *dev;
	int32_t ret;
	struct sigaction sa;
#if DEBUG_CH341
	qDebug() << "ch341::Open(" << vid << pid << ") ";
#endif
	uint8_t  desc[0x12];

	if (devHandle != NULL)
	{
		qCritical() << "Call ch341Release before re-configure";
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::Configure()";
#endif
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
		qCritical("Couldn't open device [%04x:%04x].\n", vid, pid);
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
			qCritical("Failed to detach kernel driver: '%s'\n", strerror(-ret));
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
		qCritical("Failed to claim interface 0: '%s'\n", strerror(-ret));
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
		qCritical("Failed to get device descriptor: '%s'\n", strerror(-ret));
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

	ChipMode = USB_MODE_NONE;

// 	ret = init(mode);
// 	if (ret < 0)
// 	{
// 		qCritical("Failed to init device %d\n", -ret);
// 		ReleaseInterface();
// 		return -1;
// 	}

	return 0;
}
#if 0
int32_t ch341::init(uint8_t mode)
{
	int res = -1;

	ChipMode = mode;

	switch (mode)
	{
	case USB_MODE_UART:
		res = initUART();
		break;

	case USB_MODE_I2C:
		res = initI2C();
		break;

	case USB_MODE_SPI:
		res = initSPI();
		break;

	default:
		break;
	}

	return res;
}
#endif
// TODO get from ch341a_spi.c
int32_t ch341::initSPI()
{
	return 0;
}

int32_t ch341::initI2C()
{
	return 0;
}

int32_t ch341::initUART()
{
	int32_t ret;

	uint8_t buf[2];

	if (devHandle == 0)
	{
		return -1;
	}

	/* expect two bytes 0x27 0x00 */
	ret = libusb_control_transfer(devHandle, CH341_CTRL_IN, CH341_REQ_READ_VERSION, 0, 0, buf, 2, timeout);
	if (ret < 0)
	{
		qCritical("can not read version\n");
		return ret;
	}

#if DEBUG_CH341
	qDebug("Chip version: 0x%02x\n", buf[0]);
#endif
	dev_vers = buf[0];
// 	printf("device version %x \n", dev_vers);
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


	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0x0000, NULL, 0, 1000);
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

	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_LCR_SET, 0x00c3, NULL, 0, 1000);
	if (ret < 0)
	{
		qCritical("failed control transfer CH341_REQ_WRITE_REG, CH341_REG_LCR\n");
		return ret;
	}

	// check state not implemented

	ret = libusb_control_transfer(devHandle, CH341_CTRL_OUT, CH341_REQ_WRITE_REG, CH341_REG_FLOW_CTRL, 0x0000, NULL, 0, 1000);
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


/**
 * release libusb structure and ready to exit
 */
int32_t ch341::Release(void)
{
	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::Release()";
#endif
	libusb_release_interface(devHandle, 0);
	libusb_close(devHandle);
	libusb_exit(NULL);
	devHandle = NULL;
	sigaction(SIGINT, &saold, NULL);
	return 0;
}


/**
 * Helper function for libusb_bulk_transfer, display error message with the caller name
 */
#if 0
int32_t ch341::usbTransfer(const char *func, uint8_t type, uint *buf, int len)
{
	int32_t ret;
	int transfered;

	if (devHandle == NULL)
	{
		return -1;
	}

	qDebug() << "ch341::usbTransfer()";

	ret = libusb_bulk_transfer(devHandle, type, (uchar *)&buf, len, &transfered, timeout);

	if (ret < 0)
	{
		qCritical("%s: Failed to %s %d bytes '%s'", func,
				  (type == CH341_DATA_OUT) ? "write" : "read", len, strerror(-ret));
		return -1;
	}

	return transfered;
}
#endif

/**
 * @brief Set the serial port flow mode
 *
 * @param[in] Mode : Set Stream Mode
 * -> bit0~1 : set I2C SCL rate
 * 			   --> 00 :	Low Rate /20KHz
 * 			   --> 01 : Default Rate /100KHz
 * 			   --> 10 : Fast Rate /400KHz
 * 			   --> 11 : Full Rate /750KHz
 * -> bit2 : set spi mode
 * 			   --> 0 : one in one out(D3 :clk/ D5 :out/ D7 :in)
 * 			   --> 1 : two in two out(D3 :clk/ D4,D5 :out/ D6,D7 :in)
 * -> bit7 : set spi data mode
 * 			   --> 0 : low bit first
 *       	   --> 1 : high bit first
 * other bits must keep 0
 */
int32_t ch341::SetStreamMode(uint32_t mode)
{
	uint8_t buf[CH341_PACKET_LENGTH];

	if (devHandle == NULL)
	{
		return -1;
	}

	if (dev_vers < 0x20)
	{
		return -1;
	}
	uchar mask = (ChipMode == USB_MODE_I2C) ? 0x03 : 0x0f;

	StreamMode = mode & 0x8F;
#if DEBUG_CH341
	qDebug() << "ch341::SetStreamMode()";
#endif
	buf[0] = CH341_CMD_I2C_STREAM;
	buf[1] = CH341_CMD_I2C_STM_SET | (StreamMode & mask);
	buf[2] = CH341_CMD_I2C_STM_END;
	uint len = 3;

	int res = WriteData((uint *)&buf, &len);
	if (res < 0)
	{
		qCritical("SetStreamMode() error %d\n", res);
	}
	return res;
}

/**
 * assert or deassert the chip-select pin of the spi device
 */
void ch341::ChipSelectSPI(uint8_t *ptr, bool selected)
{
	qDebug() << "ch341::ChipSelectSPI()";

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
int32_t ch341::StreamSPI(uint *out, uint *in, uint32_t len)
{
	uint8_t *inBuf, *outBuf;
	uint *inPtr, *outPtr;
	int32_t ret;
	uint packetLen;
	bool done;
	bool err = false;

	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::StreamSPI()";
#endif
	outBuf = new uint8_t[CH341_PACKET_LENGTH];
	if (outBuf == NULL)
	{
		qCritical("StreamSPI() new() troubles!\n");
		return -1;
	}

	ChipSelectSPI(outBuf, true);
	int ret_len = 4;

	ret = libusb_bulk_transfer(devHandle, CH341_DATA_OUT, (uchar *)&outBuf, 4, &ret_len, timeout);

	if (ret < 0)
	{
		qCritical("StreamSPI() can not write, error %d!\n", ret);
		delete outBuf;
		return -1;
	}

	inBuf = new uint8_t[CH341_PACKET_LENGTH];
	if (inBuf == NULL)
	{
		qCritical("StreamSPI() new() troubles!\n");
		return -1;
	}

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

		outPtr = (uint *)&outBuf;
		*outPtr++ = CH341_CMD_SPI_STREAM;

		for (uint i = 0; i < packetLen - 1; ++i)
		{
			*outPtr++ = ReverseByte(*out++);
		}

		ret = libusb_bulk_transfer(devHandle, CH341_DATA_OUT, (uchar *)&outBuf, (int)packetLen, &ret_len, timeout);

		if (ret < 0)
		{
			err = true;
			break;
		}
		ret = libusb_bulk_transfer(devHandle, CH341_DATA_IN, (uchar *)&inBuf, packetLen - 1, &ret_len, timeout);

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
		ChipSelectSPI(outBuf, false);
		len = 3;
		ret = libusb_bulk_transfer(devHandle, CH341_DATA_OUT, (uchar *)&outBuf, 3, &ret_len, timeout);
	}
	else
	{
		ret = -1;
	}

	delete outBuf;
	delete inBuf;

	if (ret < 0)
	{
		qCritical("StreamSPI() error %d!\n", ret);
		return -1;
	}

	return 0;
}

#define JEDEC_ID_LEN 0x52    // additional byte due to SPI shift

/**
 * read the JEDEC ID of the SPI Flash
 */
int32_t ch341::CapacitySPI(void)
{
	uint8_t *outBuf;
	uint8_t *inBuf, *ptr, cap;
	int32_t ret;

	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::CapacitySPI()";
#endif
	outBuf = new uint8_t[JEDEC_ID_LEN];
	if (outBuf == NULL)
	{
		qCritical("CapacitySPI() new() troubles!\n");
		return -1;
	}

	ptr = outBuf;
	*ptr++ = 0x9F; // Read JEDEC ID

	for (int i = 0; i < JEDEC_ID_LEN - 1; ++i)
	{
		*ptr++ = 0x00;
	}

	inBuf = new uint8_t[JEDEC_ID_LEN];
	if (outBuf == NULL)
	{
		qCritical("CapacitySPI() new() troubles!\n");
		return -1;
	}

	ret = StreamSPI((uint *)&outBuf, (uint *)&inBuf, JEDEC_ID_LEN);

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
int32_t ch341::ReadStatusSPI(void)
{
	uint8_t out[2];
	uint8_t in[2];
	int32_t ret;

	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::ReadStatusSPI()";
#endif
	out[0] = 0x05; // Read status
	ret = StreamSPI((uint *)&out, (uint *)&in, 2);

	if (ret < 0)
	{
		qCritical("ReadStatusSPI() error %d!\n", ret);
		return ret;
	}

	return ((int)in[1]);
}

/**
 * write status register
 */
int32_t ch341::WriteStatusSPI(uint8_t status)
{
	uint8_t out[2];
	uint8_t in[2];
	int32_t ret;

	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::WriteStatusSPI()";
#endif
	out[0] = 0x06; // Write enable
	ret = StreamSPI((uint *)&out, (uint *)&in, 1);

	if (ret < 0)
	{
		qCritical("WriteStatusSPI() error %d!\n", ret);
		return ret;
	}

	out[0] = 0x01; // Write status
	out[1] = status;
	ret = StreamSPI((uint *)&out, (uint *)&in, 2);

	if (ret < 0)
	{
		qCritical("WriteStatusSPI() error %d!\n", ret);
		return ret;
	}

	out[0] = 0x04; // Write disable
	ret = StreamSPI((uint *)&out, (uint *)&in, 1);

	if (ret < 0)
	{
		qCritical("WriteStatusSPI() error %d!\n", ret);
		return ret;
	}

	return 0;
}

/**
 * chip erase
 */
int32_t ch341::EraseChipSPI(void)
{
	uint8_t out[1];
	uint8_t in[1];
	int32_t ret;

	if (devHandle == NULL)
	{
		return -1;
	}
#if DEBUG_CH341
	qDebug() << "ch341::EraseChipSPI()";
#endif
	out[0] = 0x06; // Write enable
	ret = StreamSPI((uint *)&out, (uint *)&in, 1);

	if (ret < 0)
	{
		return ret;
	}

	out[0] = 0xC7; // Chip erase
	ret = StreamSPI((uint *)&out, (uint *)&in, 1);

	if (ret < 0)
	{
		qCritical("EraseChipSPI() error %d!\n", ret);
		return ret;
	}

	out[0] = 0x04; // Write disable
	ret = StreamSPI((uint *)&out, (uint *)&in, 1);

	if (ret < 0)
	{
		qCritical("EraseChipSPI() error %d!\n", ret);
		return ret;
	}

	return 0;
}

/**
 * read the content of SPI device to buf, make sure the buf is big enough before call
 */
int32_t ch341::ReadSPI(uint *buf, uint32_t add, uint32_t len)
{
	uint8_t out[CH341_MAX_PACKET_LEN];
	uint8_t in[CH341_PACKET_LENGTH];

	if (devHandle == NULL)
	{
		return -1;
	}
	/* what subtracted is: 1. first cs package, 2. leading command for every other packages,
	 * 3. second package contains read flash command and 3 bytes address */
	const uint32_t max_payload = CH341_MAX_PACKET_LEN - CH341_PACKET_LENGTH - CH341_MAX_PACKETS + 1 - 4;
	uint32_t tmp, pkg_len, pkg_count;
	struct libusb_transfer *xferBulkIn, *xferBulkOut;
	uint32_t idx = 0;
	uint32_t res = -1;
	int32_t old_counter;
	struct timeval tv = {0, 100};

	memset(out, 0xff, CH341_MAX_PACKET_LEN);
	for (int i = 1; i < CH341_MAX_PACKETS; ++i) // fill CH341A_CMD_SPI_STREAM for every packet
	{
		out[i * CH341_PACKET_LENGTH] = CH341_CMD_SPI_STREAM;
	}
	memset(in, 0x00, CH341_PACKET_LENGTH);
	xferBulkIn  = libusb_alloc_transfer(0);
	xferBulkOut = libusb_alloc_transfer(0);

// 	printf("Read started!\n");
	while (len > 0)
	{
		fflush(stdout);
		ChipSelectSPI(out, true);
		idx = CH341_PACKET_LENGTH + 1;
		out[idx++] = 0xC0; // byte swapped command for Flash Read
		tmp = add;
		for (int i = 0; i < 3; ++i)   // starting address of next read
		{
			out[idx++] = ReverseByte((tmp >> 16) & 0xFF);
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
		libusb_fill_bulk_transfer(xferBulkIn, devHandle, CH341_DATA_IN, in,
								  CH341_PACKET_LENGTH, cbBulkIn, buf, timeout);

		buf += max_payload; // advance user's pointer
		libusb_submit_transfer(xferBulkIn);
		libusb_fill_bulk_transfer(xferBulkOut, devHandle, CH341_DATA_OUT, out,
								  pkg_len, cbBulkOut, NULL, timeout);

		libusb_submit_transfer(xferBulkOut);
		old_counter = bulkin_count;
		while (bulkin_count < (int)pkg_count)
		{
			libusb_handle_events_timeout(NULL, &tv);
			if (bulkin_count == -1)   // encountered error
			{
				len = 0;
				res = -1;
				break;
			}
			if (old_counter != bulkin_count)   // new package came
			{
				if (bulkin_count != (int)pkg_count)
				{
					libusb_submit_transfer(xferBulkIn);    // resubmit bulk in request
				}
				old_counter = bulkin_count;
			}
		}
		ChipSelectSPI(out, false);
		uint curr_len = 3;
		res = WriteData((uint *)&out, &curr_len);

		if (res < 0)
		{
			qCritical("ReadSPI() error %d!\n", res);
			break;
		}
		if (force_stop == 1)   // user hit ctrl+C
		{
			force_stop = 0;
			if (len > 0)
			{
				qCritical("User hit Ctrl+C, reading unfinished.\n");
			}
			break;
		}
	}
	libusb_free_transfer(xferBulkIn);
	libusb_free_transfer(xferBulkOut);

	return res;
}

#define WRITE_PAYLOAD_LENGTH 301 // 301 is the length of a page(256)'s data with protocol overhead

/**
 * write buffer(*buf) to SPI flash
 */
int32_t ch341::WriteSPI(uint *buf, uint32_t add, uint32_t len)
{
	uint8_t out[WRITE_PAYLOAD_LENGTH];
	uint8_t in[CH341_PACKET_LENGTH];
	uint32_t tmp, pkg_count;
	struct libusb_transfer *xferBulkIn, *xferBulkOut;
	uint32_t idx = 0;
	uint32_t ret;
	int32_t old_counter;
	struct timeval tv = {0, 100};

	if (devHandle == NULL)
	{
		return -1;
	}
	memset(out, 0xff, WRITE_PAYLOAD_LENGTH);
	xferBulkIn  = libusb_alloc_transfer(0);
	xferBulkOut = libusb_alloc_transfer(0);

// 	printf("Write started!\n");
	while (len > 0)
	{
		out[0] = 0x06; // Write enable
		ret = StreamSPI((uint *)&out, (uint *)&in, 1);
		ChipSelectSPI(out, true);
		idx = CH341_PACKET_LENGTH;
		out[idx++] = CH341_CMD_SPI_STREAM;
		out[idx++] = 0x40; // byte swapped command for Flash Page Write
		tmp = add;
		for (int i = 0; i < 3; ++i)   // starting address of next write
		{
			out[idx++] = ReverseByte((tmp >> 16) & 0xFF);
			tmp <<= 8;
		}
		tmp = 0;
		pkg_count = 1;

		while ((idx < WRITE_PAYLOAD_LENGTH) && (len > tmp))
		{
			if (idx % CH341_PACKET_LENGTH == 0)
			{
				out[idx++] = CH341_CMD_SPI_STREAM;
				pkg_count ++;
			}
			else
			{
				out[idx++] = ReverseByte(*buf++);
				tmp++;
				if (((add + tmp) & 0xFF) == 0) // cross page boundary
				{
					break;
				}
			}
		}
		len -= tmp;
		add += tmp;
		bulkin_count = 0;
		libusb_fill_bulk_transfer(xferBulkIn, devHandle, CH341_DATA_IN, in,
								  CH341_PACKET_LENGTH, cbBulkIn, NULL, timeout);
		libusb_submit_transfer(xferBulkIn);
		libusb_fill_bulk_transfer(xferBulkOut, devHandle, CH341_DATA_OUT, out,
								  idx, cbBulkOut, NULL, timeout);
		libusb_submit_transfer(xferBulkOut);
		old_counter = bulkin_count;
		ret = 0;
		while (bulkin_count < (int)pkg_count)
		{
			libusb_handle_events_timeout(NULL, &tv);
			if (bulkin_count == -1)   // encountered error
			{
				ret = -1;
				break;
			}
			if (old_counter != bulkin_count)   // new package came
			{
				if (bulkin_count != (int)pkg_count)
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
		ChipSelectSPI(out, false);

		uint curr_len = 3;
		ret = WriteData((uint *)&out, &curr_len);

		if (ret < 0)
		{
			break;
		}
		out[0] = 0x04; // Write disable
		ret = StreamSPI((uint *)&out, (uint *)&in, 1);
		do
		{
			ret = ReadStatusSPI();
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
				qCritical("User hit Ctrl+C, writing unfinished.\n");
			}
			break;
		}
	}
	libusb_free_transfer(xferBulkIn);
	libusb_free_transfer(xferBulkOut);

	if (ret < 0)
	{
		qCritical("WriteSPI() error %d!\n", ret);
	}
	return ret;
}

