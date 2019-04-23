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

#ifndef __CH341_H__
#define __CH341_H__

#include "usb_interf.h"

#include <libusb-1.0/libusb.h>

#include <QObject>
#include <QTimer>


typedef	enum _EEPROM_TYPE
{
	ID_24C01,
	ID_24C02,
	ID_24C04,
	ID_24C08,
	ID_24C16,
	ID_24C32,
	ID_24C64,
	ID_24C128,
	ID_24C256,
	ID_24C512,
	ID_24C1024,
	ID_24C2048,
	ID_24C4096
} EEPROM_TYPE;

#define    MAX_EEPROM_SIZE              131072 /* For 24c1024*/

#define    MAX_BUFFER_LENGTH            0x1000
#define    DEFAULT_BUFFER_LEN           0x0400

#define    DEFAULT_CONFIGURATION        0x01
#define    DEFAULT_TIMEOUT              300    // 300mS for USB timeouts

// USB device standard request code
#define    CH341_CLR_FEATURE            0x01
#define    CH341_SET_FEATURE            0x03
#define    CH341_GET_STATUS             0x04
#define    CH341_SET_ADDRESS            0x05
#define    CH341_GET_DESCR              0x06
#define    CH341_SET_DESCR              0x07
#define    CH341_GET_CONFIG             0x08
#define    CH341_SET_CONFIG             0x09
#define    CH341_GET_INTERF             0x0a
#define    CH341_SET_INTERF             0x0b
#define    CH341_SYNC_FRAME             0x0c
#define    CH341_REQ_RESET              0x0d
#define    CH341_REQ_ABORT              0x0e


#define    CH341_BUF_CLEAR              0xB2		// Clear unfinished data

#define    CH341_I2C_STATUS             0x52		// Get the status of the I2C interface
#define    CH341_I2C_COMMAND            0x53		// Command to issue an I2C interface
#define    CH341_I2C_CMD_X              0x54		// Issue the I2C interface command and execute it immediately
#define    CH341_DELAY_MS               0x5E		// Delay the specified time in units of leap seconds
#define    CH341_GET_VER                0x5F		// Get the chip version


#if 0
#define    IN_BUF_SZ                    0x100
#define    EEPROM_WRITE_BUF_SZ          0x2b   // only for 24c64 / 24c32 ??
#define    EEPROM_READ_BULKIN_BUF_SZ    0x20
#define    EEPROM_READ_BULKOUT_BUF_SZ   0x65
#endif

#define    CH341_PACKET_LENGTH          0x20
#define    CH341_MAX_PACKETS            256
#define    CH341_MAX_PACKET_LEN         (CH341_PACKET_LENGTH * CH341_MAX_PACKETS)

#define    CH34x_VENDOR_ID1             0x4348
#define    CH34x_VENDOR_ID2             0x1A86

#define    CH341_PRODUCT                0x5512 // for SPI, I2C 

#define    CH341_UART_PRODUCT           0x5523
#define    CH340_UART_PRODUCT           0x7523

#define    CH341_CMD_GET_INPUT          0xA0
#define    CH341_CMD_SET_OUTPUT         0xA1
#define    CH341_CMD_IO_ADDR            0xA2
#define    CH341_CMD_PRINT_OUT          0xA3
#define    CH341_CMD_SPI_STREAM         0xA8
#define    CH341_CMD_SIO_STREAM         0xA9
#define    CH341_CMD_I2C_STREAM         0xAA
#define    CH341_CMD_UIO_STREAM         0xAB

#define    CH341_CMD_I2C_STM_STA        0x74
#define    CH341_CMD_I2C_STM_STO        0x75
#define    CH341_CMD_I2C_STM_OUT        0x80
#define    CH341_CMD_I2C_STM_IN         0xC0
#define    CH341_CMD_I2C_STM_MAX        ( min( 0x3F, CH341_PACKET_LENGTH ) )
#define    CH341_CMD_I2C_STM_SET        0x60
#define    CH341_CMD_I2C_STM_US         0x40
#define    CH341_CMD_I2C_STM_MS         0x50
#define    CH341_CMD_I2C_STM_DLY        0x0F
#define    CH341_CMD_I2C_STM_END        0x00

// GPIO part
#define    CH341_CMD_UIO_STM_IN         0x00
#define    CH341_CMD_UIO_STM_DIR        0x40
#define    CH341_CMD_UIO_STM_OUT        0x80
#define    CH341_CMD_UIO_STM_US         0xC0
#define    CH341_CMD_UIO_STM_END        0x20

#define    CH341_STM_I2C_20K            0x00
#define    CH341_STM_I2C_100K           0x01
#define    CH341_STM_I2C_400K           0x02
#define    CH341_STM_I2C_750K           0x03
#define    CH341_STM_SPI_DBL            0x04



/*******************************/
/* baudrate calculation factor */
/*******************************/
#define    CH341_BPS_MOD_BASE           20000000
#define    CH341_BPS_MOD_BASE_OFS       1100

//Vendor define
#define    CH341_REQ_WRITE_TYPE         0x40
#define    CH341_REQ_READ_TYPE          0xC0


#define    CH341_REG_STAT               0x0706
#define    CH341_REG_BREAK              0x1805
#define    CH341_REG_LCR_SET            0x1825
#define    CH341_REG_LCR_GET            0x1824
#define    CH341_REG_FLOW_CTRL          0x2727
#define    CH341_REG_BAUD1              0x1312
#define    CH341_REG_BAUD2              0x0f2c  /*is it right, or 0x0f14 ?? */

#define    CH341_RESET_VALUE            0x501f    /* line mode? */
#define    CH341_RESET_INDEX            0xd90a    /* baud rate? */

#define    CH341_REQ_READ_VERSION       0x5F
#define    CH341_REQ_WRITE_REG          0x9A
#define    CH341_REQ_READ_REG           0x95
#define    CH341_REQ_SERIAL_INIT        0xA1
#define    CH341_REQ_MODEM_CTRL         0xA4  /* DTR and RTS */

// ??? right
#define    CH341_NBREAK_BITS_REG        0x4001

#define    CH341_NBREAK_BITS_REG1       0x01
#define    CH341_NBREAK_BITS_REG2       0x40

#define    CH341_INPUT_BUF_SIZE         0x08

/* status returned in third interrupt answer byte, inverted in data
   from irq */
//For CMD 0xA4 CH341_REQ_MODEM_CTRL
#define    CH341_UART_CTS               0x01
#define    CH341_UART_DSR               0x02
#define    CH341_UART_RING              0x04
#define    CH341_UART_DCD               0x08
#define    CH341_BITS_MODEM_STAT        0x0f /* all bits */

#define    CH341_MULT_STAT              0x04 /* multiple status since last interrupt event */

#define    CH341_CONTROL_OUT            0x10 // TX
#define    CH341_CONTROL_DTR            0x20 // DTR, 1 << 5
#define    CH341_CONTROL_RTS            0x40 // RTS, 1 << 6

//Uart state
#define    CH341_UART_STATE             0x00
#define    CH341_UART_OVERRUN_ERROR     0x01
#define    CH341_UART_BREAK_ERROR       //no define
#define    CH341_UART_PARITY_ERROR      0x02
#define    CH341_UART_FRAME_ERROR       0x06
#define    CH341_UART_RECV_ERROR        0x02
#define    CH341_UART_STATE_TRANSIENT_MASK    0x07

// LCR
#define    CH341_LCR_ENABLE_RX          0x80
#define    CH341_LCR_ENABLE_TX          0x40
#define    CH341_LCR_MARK_SPACE         0x20
#define    CH341_LCR_PAR_EVEN           0x10
#define    CH341_LCR_ENABLE_PAR         0x08
#define    CH341_LCR_STOP_BITS_2        0x04
#define    CH341_LCR_CS8                0x03
#define    CH341_LCR_CS7                0x02
#define    CH341_LCR_CS6                0x01
#define    CH341_LCR_CS5                0x00

//Flow control values
#define    CH34X_FLOW_CONTROL_NONE      0x0000
#define    CH34X_FLOW_CONTROL_RTS_CTS   0x0101
#define    CH34X_FLOW_CONTROL_DSR_DTR   0x0202

//Port state
#if 0
#define    CH341_PORTA_STATE            0x01
#define    CH341_PORTB_STATE            0x02
#define    CH341_PORTC_STATE            0x03
#endif

//CH34x Baud Rate
#define    CH341_BAUDRATE_FACTOR        1532620800
#define    CH341_BAUDRATE_DIVMAX        3

/**
 * @brief
 *
 * LIBUSB_ENDPOINT_IN         0x80     device to host
 * LIBUSB_ENDPOINT_OUT        0x00     host to device
 * LIBUSB_REQUEST_TYPE_CLASS  0x40
 * LIBUSB_REQUEST_TYPE_VENDOR 0x80
 * LIBUSB_RECIPIENT_ENDPOINT  0x02
 * LIBUSB_RECIPIENT_INTERFACE 0x01
 */

#define    CH341_DATA_IN                (0x2 | LIBUSB_ENDPOINT_IN)
#define    CH341_DATA_OUT               (0x2 | LIBUSB_ENDPOINT_OUT)
#define    CH341_CTRL_IN                (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN )
#define    CH341_CTRL_OUT               (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT )
#define    CH341_CTRL_IN_ENDPOINT       (LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_ENDPOINT)
#define    CH341_CTRL_OUT_ENDPOINT      (LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_ENDPOINT)
#define    DEFAULT_BAUD_RATE            9600


// das hier ausprobieren
// #define    CONTROL_REQUEST_TYPE_IN     ( LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE )
// #define    CONTROL_REQUEST_TYPE_OUT    ( LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE )

#define    MAX_CONTROL_IN_TRANSFER_SIZE 2
#define    MAX_CONTROL_OUT_TRANSFER_SIZE 2


class USB_Interface;

class ch341 : public QObject, public USB_Interface
{
	Q_OBJECT

  public:
	ch341();
// 	virtual ~ch341();

	virtual int32_t Open(uint16_t vid, uint16_t pid, uint8_t mode);
	virtual void    Close();
	virtual int32_t SetMode(uint16_t mode = USB_MODE_NONE);

	int32_t SetStream(uint32_t speed);
	int32_t SpiCapacity(void);
	int32_t SpiRead(uint *buf, uint32_t add, uint32_t len);
	int32_t ReadStatus(void);
	int32_t WriteStatus(uint8_t status);
	int32_t EraseChip(void);
	int32_t SpiWrite(uint *buf, uint32_t add, uint32_t len);

	int32_t GetInput(uint *iStatus);
	int32_t SetDelaymS(uint iDelay);
	int32_t GetStatus(uint *iStatus);

	virtual int32_t Release(void);
	virtual int32_t SetBaudRate(int32_t baudRate);
	virtual int32_t SetTimeouts(int16_t t);
	virtual int32_t SetBreakControl(int32_t state);
	virtual void    SetParity(uint8_t p);
	virtual void    SetBits(uint8_t b);
	virtual void    SetStops(uint8_t s);
	virtual void    SetFlowControl(uint8_t f);
	virtual int32_t SetDTR(int32_t dtr);
	virtual int32_t SetRTS(int32_t dtr);
	virtual int32_t GetDSR();
	virtual int32_t GetCTS();
	virtual int32_t SetRTSDTR(int state);
	virtual int32_t SetConfigLCR();
	virtual int32_t Probe();
	virtual int32_t ClearChip();
	virtual int32_t ResetChip();

// 	virtual int32_t GetStatus();
	virtual int32_t GetStatusRx();
	virtual int32_t GetStatusTx();

	virtual int32_t Read(uchar *buf, size_t len);
	virtual int32_t Write(uchar *buf, size_t len);

	virtual void    ReleaseInterface(void);
	virtual void    CloseHandle();

  signals:
	void breakChanged(bool set);

  public slots:
	void triggerBreak(uint msecs);

  private slots:
	void breakTimeout();

  public:
	// for callback
	static int16_t read_completed;
	static int16_t write_completed;

  private:
	int32_t getModemState(void);
	int32_t setHandshakeByte(void);
	int32_t SetupSerial(uint32_t mode, uint32_t baud);

	virtual int32_t getState();
	virtual int32_t setControl();
	virtual int32_t init(uint8_t mode);

	int32_t initUART();
	int32_t initSPI();
	int32_t initI2C();

	int32_t WriteRead(uint wlength, uchar *wBuffer, uint readStep, uint readTimes, uint *rLength, uchar *rBuffer);
	int32_t SetTimeout(uint iWriteTimeout, uint iReadTimeout);
	int32_t SetExclusive(uint iExclusive);
	int32_t ResetDevice();
	int32_t ResetWrite();
	int32_t ResetRead();
	int32_t AbortRead();
	int32_t AbortWrite();
	int32_t SetOutput(uint iEnable, uint iSetDirOut, uint iSetDataOut);
	int32_t WriteI2C(char iDevice, uchar iAddr, uchar iByte);
	int32_t ReadI2C(uint iDevice, uchar iAddr, uint *oByte);
	int32_t Set_D5_D0(uchar iSetDirOut, uchar iSetDataOut);
	int32_t FlushBuffer();
	int32_t ReadData(uint *oBuffer, uint *ioLength);
	int32_t WriteData(uint *iBuffer, uint *ioLength);

	int32_t StreamSPI4(uint chip_select, uint length, uchar *buffer);
	int32_t StreamSPI5(uint chip_select, uint length, uchar *buffer, uchar *buffer2);
	int32_t StreamSPI(unsigned long chip_select, unsigned long length, uchar *buffer, uchar *buffer2);

	int32_t StreamI2C(uint iWriteLength, uint *iWriteBuffer, uint iReadLength, uint *oReadBuffer);
	int32_t WriteEEPROM(EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint *iBuffer);
	int32_t ReadEEPROM(EEPROM_TYPE iEepromID, uint32_t iAddr, uint32_t iLength, uint *oBuffer);

// 	int setAsync(unsigned char data);

	void    updateStatus(uint8_t *data, size_t l);

	void    SpiChipSelect(uint8_t *ptr, bool selected);
	int32_t SpiStream(uint *out, uint *in, uint32_t len);

  private:
	struct dv
	{
		uint8_t	dv_prescaler;
		uint8_t	dv_div;
		uint8_t	dv_mod;
	};

	struct uart_div
	{
		uint32_t dvr_high;
		uint32_t dvr_low;
		uint32_t dvr_base_clock;
		struct dv dvr_divider;
	};
#if 0
	// MODEM output lines
	uint8_t dtr; // modem line
	uint8_t rts; // modem line

	uint8_t rtsCtsEnabled;
	uint8_t dtrDsrEnabled;

	// UART settings
	uint8_t parity;
	uint8_t bits;
	uint8_t stops;
	uint8_t flow_control;
	int32_t baudRate;
	int16_t timeout;

//     struct libusb_transfer *ctrl_transfer;

//     uint8_t ctrl_buf[LIBUSB_CONTROL_SETUP_SIZE];

	// MODEM input lines
	uint8_t    ctsState;
	uint8_t    dsrState;
	uint8_t    dcdState;
	uint8_t    ringState;

	uint8_t lcr; // line control register
	uint8_t lsr; // line status register
	uint8_t msr; // modem status register
	uint8_t mcr; // modem control register
#endif
	uint8_t dev_vers;
	uint8_t StreamMode;

	QTimer *breakTimer;
	int force_stop = 0;
};


#endif
